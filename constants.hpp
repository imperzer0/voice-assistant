//
// Created by imper on 12/29/22.
//

#ifndef ASSISTANT_CONSTANTS_HPP
#define ASSISTANT_CONSTANTS_HPP


# define _STR(s) #s
# define MACRO_STR(v) _STR(v)


# ifndef VERSION
#  define VERSION "(devel)"
# endif

# ifndef APPNAME
#  define APPNAME "voice-assistant"
# endif

# ifndef DEFAULT_CONFIG_FILE
#  define DEFAULT_CONFIG_FILE "/etc/voice-assistant.conf"
# endif

# ifndef TEMPORARY_VOICE_COMMAND_FILE
#  define TEMPORARY_VOICE_COMMAND_FILE "/tmp/assistant_last_voice_command.wav"
# endif

# ifndef SOUND_SAMPLE_RATE
#  define SOUND_SAMPLE_RATE 44100
# endif

#endif //ASSISTANT_CONSTANTS_HPP
