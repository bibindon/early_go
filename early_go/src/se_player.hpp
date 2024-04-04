#pragma once
#include "stdafx.hpp"

#include <dsound.h>

namespace early_go
{
class se_player
{
public:
    static std::shared_ptr<se_player> single_ton_;
    static std::shared_ptr<se_player> get_ton();
    static void initialize(HWND hwnd);
    static void finalize(); // For memory leak check.

    bool load(const std::string& filename);
    void play(const std::string& filename, const int a_volume = 100);
    void stop(const std::string& filename);
private:
    // hide ctor
    se_player(HWND hwnd);
    se_player(const se_player&) { }
    void operator=(const se_player&) { }

    bool open_wave(
        const std::string& filepath,
        WAVEFORMATEX& waveFormatEx,
        std::vector<char>* ppData,
        DWORD& dataSize);

    int per_to_decibel(const int percent);

    std::shared_ptr<IDirectSound8> dx8sound_ { };
    std::unordered_map<std::string, std::shared_ptr<IDirectSoundBuffer8>> dx8sound_buffers_ { };
};
}

