#include <iostream>
#include <getopt.h>
#include <cstring>
#include "config.h"
#include "constants.hpp"
#include "cmdline.h"
#include "assistant.h"


static constexpr const char* short_args = "c:t:TVvh";
static constexpr struct option long_args[] = {
		{ "config",         required_argument, nullptr, 'c' },
		{ "threshold",      required_argument, nullptr, 't' },
		{ "threshold-test", no_argument,       nullptr, 'T' },
		{ "verbose",        no_argument,       nullptr, 'V' },
		{ "version",        no_argument,       nullptr, 'v' },
		{ "help",           no_argument,       nullptr, 'h' },
		{ nullptr, 0,                          nullptr, 0 }
};


inline static void help()
{
	::printf(APPNAME " v" VERSION "\n");
	::printf("Usage: " APPNAME " [OPTIONS]...\n");
	::printf("Runs http server.\n");
	::printf("Options:\n");
	::printf(" --config          | c  <file>    Enable hex dump.\n");
	::printf(" --threshold       | t  <volume>  Voice threshold volume.\n");
	::printf(" --threshold-test  | T            Run threshold test (15 seconds).\n");
	::printf(" --verbose         | V            Print verbose information.\n");
	::printf(" --version         | v            Show version information.\n");
	::printf(" --help            | h            Show this help message.\n\n");
	
	::exit(34);
}

int main(int argc, char** argv)
{
	const char* config = DEFAULT_CONFIG_FILE;
	bool run_threshold_test = false;
	int option_index, option;
	while ((option = ::getopt_long(argc, argv, short_args, long_args, &option_index)) > 0)
	{
		switch (option)
		{
			case 'c':
				config = ::strdup(optarg);
				break;
			case 't':
				sound_threshold = ::strtoul(optarg, nullptr, 10);
				break;
			case 'T':
				run_threshold_test = true;
				break;
			case 'V':
				verbose = true;
				break;
			case 'v':
				::printf(APPNAME "version: " VERSION "\n");
				break;
			case 'h':
				help();
				break;
			default:
				help();
		}
	}
	
	if (run_threshold_test)
		return RunThresholdTest();
	
	
	if (parse_config(config))
		return -1;
	
	if (verbose) print_config();
	
	while (true)
	{
		ListenMicrophone();
		
		RecognizeVoiceCommand();
		
		if (ExecuteVoiceCommand())
			return -2;
	}
}
