#pragma once
#include "stdafx.hpp"

#include <dsound.h>

namespace early_go
{
class bgm_player
{
public:
    static std::shared_ptr<bgm_player> single_ton_;
    static std::shared_ptr<bgm_player> get_ton();
    static void initialize(HWND hwnd);

    bool load(const std::string& filename);
    void play();
    void stop();
private:
    // hide ctor
    bgm_player(HWND hwnd);
    bgm_player(const bgm_player&) { }
    void operator=(const bgm_player&) { }

    bool open_wave(
        const std::string& filepath,
        WAVEFORMATEX& waveFormatEx,
        std::vector<char>* ppData,
        DWORD& dataSize);

    std::shared_ptr<IDirectSound8> dx8sound_ { };
    std::shared_ptr<IDirectSoundBuffer8> dx8sound_buffer_ { };
};
}
