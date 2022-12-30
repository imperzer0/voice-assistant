// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#include "assistant.h"
#include "config.h"
#include "constants.hpp"
#include "cmdline.h"
#include <cpprest/filestream.h>
#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include <SFML/Audio.hpp>
#include <sstream>
#include <sys/wait.h>


void ListenMicrophone()
{
	// Create a sound recorder and start recording
	sf::SoundBufferRecorder recorder;
	recorder.setChannelCount(2);
	recorder.start(SOUND_SAMPLE_RATE);
	
	sf::OutputSoundFile outFile;
	outFile.openFromFile(TEMPORARY_VOICE_COMMAND_FILE, SOUND_SAMPLE_RATE, 2);
	
	float prev_volume = 0.f;
	sf::SoundBuffer prev_buffer;
	// Continuously record audio and check the volume
	for (size_t i = 0; i < 5; ++i)
	{
		// Record a small chunk of audio
		sf::sleep(sf::seconds(1));
		recorder.stop();
		sf::SoundBuffer buffer = recorder.getBuffer();
		recorder.start(SOUND_SAMPLE_RATE);
		
		// Check the volume of the audio
		float volume = 0.f;
		const sf::Int16* samples = buffer.getSamples();
		for (std::size_t i = 0; i < buffer.getSampleCount(); ++i)
		{
			volume += std::abs(samples[i]);
		}
		if (buffer.getSampleCount() != 0)
			volume /= buffer.getSampleCount();
		
		if (verbose) std::clog << "Chunk volume: [" << volume << "].\n";
		
		// If the volume exceeds the threshold, save the audio to the file
		if (volume > sound_threshold || prev_volume > sound_threshold)
		{
			if (prev_volume < sound_threshold)
				outFile.write(prev_buffer.getSamples(), prev_buffer.getSampleCount());
			
			outFile.write(buffer.getSamples(), buffer.getSampleCount());
			i = 0;
		}
		
		prev_volume = volume;
		prev_buffer = buffer;
	}
}


int RunThresholdTest()
{
	std::cout << "You ran threshold test. In the next 15 seconds program will\n"
	             "record test sound and calculate it's average volume. Tell something\n"
	             "using normal voice volume. First second will be discarded.\n"
	             "Press Enter to begin: " << std::flush;
	
	// Wait for Enter key
	std::cin.get();
	
	// Create a sound recorder and start recording
	sf::SoundBufferRecorder recorder;
	recorder.setChannelCount(2);
	recorder.start(SOUND_SAMPLE_RATE);
	
	float average_volume = 0.f, average_high_volume = 0.f;
	size_t high_volume_amount = 0;
	
	// Discard first second
	if (verbose) std::clog << "Discarding first second...\n";
	sf::sleep(sf::seconds(1));
	recorder.stop();
	recorder.start(SOUND_SAMPLE_RATE);
	
	// Continuously record audio and check the volume
	for (size_t i = 0; i < 13; ++i)
	{
		// Record a small chunk of audio
		sf::sleep(sf::seconds(1));
		recorder.stop();
		sf::SoundBuffer buffer = recorder.getBuffer();
		recorder.start(SOUND_SAMPLE_RATE);
		
		// Check the volume of the audio
		float volume = 0.f;
		const sf::Int16* samples = buffer.getSamples();
		for (std::size_t i = 0; i < buffer.getSampleCount(); ++i)
		{
			volume += std::abs(samples[i]);
		}
		volume /= buffer.getSampleCount();
		
		average_volume += volume;
		
		if (volume > average_volume / float(i + 1))
		{
			++high_volume_amount;
			average_high_volume += volume;
		}
		
		if (verbose) std::clog << "Chunk volume: [" << volume << "].\n";
	}
	
	average_volume /= 100.f;
	average_volume = std::floor(average_volume);
	
	average_high_volume /= float(high_volume_amount);
	average_high_volume = std::floor(average_high_volume);
	
	if (average_volume == 0.f)
	{
		std::cout << "\nIt seems you didn't say anything.\nCheck your microphone and try again.\n";
	}
	else
	{
		std::cout << "\nAverage volume of sound was [" << average_volume << "].\nWe recommend use it as threshold.\n";
	}
	
	return 0;
}


pplx::task<void> SendEOSMessage(web::websockets::client::websocket_callback_client client)
{
	if (verbose) std::clog << "Audio sent.\n";
	web::websockets::client::websocket_outgoing_message closeMsg;
	closeMsg.set_utf8_message("EOS");
	return client.send(closeMsg);
}

void SendAudio(web::websockets::client::websocket_callback_client client, const std::string& media_file)
{
	try
	{
		if (verbose) std::clog << "Sending audio...\n";
		web::websockets::client::websocket_outgoing_message msg;
		auto dd = Concurrency::streams::file_stream<unsigned char>::open_istream(media_file, std::ios::binary);
		
		msg.set_binary_message(dd.get());
		client.send(msg).then([ = ]() { SendEOSMessage(client); });
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

static std::stringstream last_result;

void RecognizeVoiceCommand()
{
	std::string const websocketEndpoint = "wss://api.rev.ai/speechtotext/v1/stream";
	std::string const accessToken = APIToken;
	std::string const contentType = "&content_type=audio/x-wav";
	std::string url = websocketEndpoint + "?access_token=" + accessToken + contentType;
	
	try
	{
		bool keepListening = true;
		bool gotResult = false;
		web::websockets::client::websocket_callback_client client;
		client.connect(U(url)).wait();
		
		client.set_message_handler(
				[ & ](const web::websockets::client::websocket_incoming_message& msg)
				{
					std::string response = msg.extract_string().get();
					web::json::value data = web::json::value::parse(response);
					
					if (data["type"].as_string() == "connected")
					{
						if (verbose) std::clog << "WebSocket Connected.\n";
						
						// Now that the socket is connected send the data
						SendAudio(client, TEMPORARY_VOICE_COMMAND_FILE);
					}
					
					if (data["type"].as_string() == "final")
					{
						// go thru the response and output the values
						web::json::array elements = data["elements"].as_array();
						for (size_t index = 0; index < elements.size(); ++index)
						{
							web::json::object element = elements[index].as_object();
							std::string value = element["value"].as_string();
							last_result << value << " ";
						}
						
						if (verbose) std::clog << "Got result.\n";
						gotResult = true;
					}
				}
		);
		
		client.set_close_handler(
				[ & ](
						web::websockets::client::websocket_close_status close_status,
						const utility::string_t& reason,
						const std::error_code& error
				)
				{
					if (verbose) std::clog << "Closing Connection...\n";
					client.close();
					keepListening = false;
				}
		);
		
		while (keepListening)
		{
			if (gotResult)
				client.close();
			sf::sleep(sf::milliseconds(100));
		}
		
		if (verbose) std::clog << "Streaming Complete.\n";
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}


int ExecuteVoiceCommand()
{
	auto action = match_command(last_result.str());
	last_result.str("");
	if (action == match_command_failed())
	{
		std::cerr << "Command \"" << last_result.str() << "\" not recognized. Try again.\n";
		return false;
	}
	
	if (verbose) std::cerr << "Executing command \"" << last_result.str() << "\"...\n";
	
	char* argv[] = { "/bin/bash", "-c", action->second.data(), nullptr };
	
	errno = 0;
	pid_t pid = ::fork();
	if (pid == 0) // child process
		::execvp(argv[0], argv);
	
	if (pid < 0)
	{
		std::cerr << "Couldn't execute fork(). Error "
		          << strerrorname_np(errno) << " (" << errno << ") " << strerrordesc_np(errno) << ".\n";
		return true;
	}
	
	return false;
}