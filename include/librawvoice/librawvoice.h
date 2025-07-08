#pragma once

void LibRawVoice_Initialize(const char* path);
std::vector<int16_t> LibRawVoice_GetNextVoiceSamples(size_t length);
void LibRawVoice_SetCurrentSamples(const std::vector<int16_t>& samples);

extern "C" {
#include <stdbool.h>

bool LibRawVoice_IsVoiceRegistered(int32_t voiceId);
void LibRawVoice_SetCurrentVoiceId(int32_t voiceId);
int32_t LibRawVoice_GetCurrentVoiceId();
int32_t LibRawVoice_GetCurrentVoiceStatus();
}