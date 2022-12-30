// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#ifndef ASSISTANT_ASSISTANT_H
#define ASSISTANT_ASSISTANT_H

#include <string>


extern void ListenMicrophone();

extern int RunThresholdTest();

extern void RecognizeVoiceCommand();

extern int ExecuteVoiceCommand();

#endif //ASSISTANT_ASSISTANT_H
