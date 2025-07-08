#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <format>

#include <librawvoice/librawvoice.h>

std::unordered_map<std::string, std::vector<int16_t>> pcmSamples;
int32_t currentVoiceId = 0;
std::vector<int16_t> currentSamples;
uint32_t begin = 0;
const std::vector<int16_t> emptySampleVector;

void LibRawVoice_Initialize(const char* path) {
    if (!std::filesystem::exists(path)) {
        return;
    }
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::ifstream stream(entry.path(), std::ios::binary | std::ios::ate);
        if (!stream) {
            continue;
        }

        std::streamsize size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<int16_t> samples(size/2);
        stream.read((char*) samples.data(), size);

        pcmSamples[std::filesystem::path(entry.path()).filename().string()] = samples;
    }
}

std::vector<int16_t> LibRawVoice_GetNextVoiceSamples(size_t length) {
    std::vector<int16_t>& samples = pcmSamples.at(std::format("{}.raw", currentVoiceId));
    size_t samples_size = samples.size();
    if (begin > samples_size) {
        return emptySampleVector;
    }
    size_t remaining_samples = samples_size - begin;
    size_t actual_length = std::min(length, remaining_samples);
    size_t end = begin + actual_length;
    if (remaining_samples == 0)
    {
        begin = 0;
        currentVoiceId = 0;
        return emptySampleVector;
    }
    std::vector<int16_t> subsamples(samples.begin() + begin, samples.begin() + end);
    begin += actual_length;
    return subsamples;
}

extern "C" {
bool LibRawVoice_IsVoiceRegistered(int32_t voiceId) {
    std::string identifier = std::format("{}.raw", voiceId);
    return pcmSamples.find(identifier) != pcmSamples.end();
}

void LibRawVoice_SetCurrentVoiceId(int32_t voiceId) {
    currentVoiceId = voiceId;
    begin = 0;
}

int32_t LibRawVoice_GetCurrentVoiceId() {
    return currentVoiceId;
}

int32_t LibRawVoice_GetCurrentVoiceStatus() {
    for (int16_t currentSample : currentSamples) {
        if (currentSample != 0) {
            return 1;
        }
    }
    return 0;
}
}

void LibRawVoice_SetCurrentSamples(const std::vector<int16_t>& samples) {
    currentSamples = samples;
}