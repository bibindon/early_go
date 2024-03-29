#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment ( lib, "winmm.lib" )

#include "bgm_player.hpp"

#include <mmsystem.h>

using std::vector;
using std::string;
using std::shared_ptr;

namespace early_go
{
std::shared_ptr<bgm_player> bgm_player::single_ton_;
std::shared_ptr<bgm_player> bgm_player::get_ton()
{
    if (single_ton_.get() == nullptr)
    {
        single_ton_.reset(new_crt bgm_player { });
    }
    return single_ton_;
}

// Reference
// http://marupeke296.com/DSSMP_No2_GetSoundFromWave.html
bool bgm_player::load(HWND hwnd, const std::string& filename)
{
    // Open wave file.
    WAVEFORMATEX _waveformatex { };
    char* _wave_data { nullptr };
    DWORD _wave_size { 0 };
    if (!open_wave(filename, _waveformatex, &_wave_data, _wave_size))
    {
        return false;
    }

    // Create sound device.
    LPDIRECTSOUND8 _dx8sound { nullptr };
    DirectSoundCreate8(nullptr, &_dx8sound, nullptr);
    dx8sound_.reset(_dx8sound, custom_deleter { });
    dx8sound_->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);

    DSBUFFERDESC DSBufferDesc { };
    DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
    DSBufferDesc.dwFlags = 0;
    DSBufferDesc.dwBufferBytes = _wave_size;
    DSBufferDesc.dwReserved = 0;
    DSBufferDesc.lpwfxFormat = &_waveformatex;
    DSBufferDesc.guid3DAlgorithm = GUID_NULL;

    // Create secondary buffer.
    IDirectSoundBuffer* _temp_buffer { nullptr };
    IDirectSoundBuffer8* _temp_buffer8 { nullptr };
    dx8sound_->CreateSoundBuffer(&DSBufferDesc, &_temp_buffer, nullptr);
    _temp_buffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&_temp_buffer8);
    _temp_buffer->Release();
    if (_temp_buffer8 == 0)
    {
        return false;
    }
    dx8sound_buffer_.reset(_temp_buffer8, custom_deleter { });

    // Write wave data to secondary buffer.
    LPVOID secondary_buffer { nullptr };
    DWORD _length { 0 };
    if (DS_OK == dx8sound_buffer_->Lock(
        0, 0, &secondary_buffer, &_length, nullptr, nullptr, DSBLOCK_ENTIREBUFFER))
    {
        memcpy(secondary_buffer, _wave_data, _length);
        dx8sound_buffer_->Unlock(secondary_buffer, _length, nullptr, 0);
    }
    delete[] _wave_data;

    return true;
}

void bgm_player::play()
{
    dx8sound_buffer_->Play(0, 0, 0);
}

void bgm_player::stop()
{
    dx8sound_buffer_->Stop();
}

bgm_player::bgm_player()
{
}
bool bgm_player::open_wave(
    const std::string& filepath, WAVEFORMATEX& waveformatex, char** buff, DWORD& wave_size)
{
    if (filepath.empty())
    {
        return false;
    }

    HMMIO _hmmio { nullptr };
    MMIOINFO mmioInfo = { 0 };

    vector<char> bgm_raw = util::get_sound_resource(filepath);
    mmioInfo.pchBuffer = &bgm_raw[0];
    mmioInfo.fccIOProc = FOURCC_MEM;
    mmioInfo.cchBuffer = static_cast<LONG>(bgm_raw.size());
    _hmmio = mmioOpen(nullptr, &mmioInfo, MMIO_READ);

    if (_hmmio == nullptr)
    {
        return false;
    }

    // Search RIFF chunk.
    MMRESULT mmresult { 0 };
    MMCKINFO riff_chunk { };
    riff_chunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    mmresult = mmioDescend(_hmmio, &riff_chunk, NULL, MMIO_FINDRIFF);
    if (mmresult != MMSYSERR_NOERROR)
    {
        mmioClose(_hmmio, 0);
        return false;
    }

    // Search format chunk
    MMCKINFO format_chunk { };
    format_chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    mmresult = mmioDescend(_hmmio, &format_chunk, &riff_chunk, MMIO_FINDCHUNK);
    if (mmresult != MMSYSERR_NOERROR)
    {
        mmioClose(_hmmio, 0);
        return false;
    }
    DWORD fmsize { format_chunk.cksize };
    DWORD size { (DWORD)mmioRead(_hmmio, (HPSTR)&waveformatex, fmsize) };
    if (size != fmsize)
    {
        mmioClose(_hmmio, 0);
        return false;
    }

    mmioAscend(_hmmio, &format_chunk, 0);

    // Search data chunk.
    MMCKINFO data_chunk { };
    data_chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    mmresult = mmioDescend(_hmmio, &data_chunk, &riff_chunk, MMIO_FINDCHUNK);
    if (mmresult != MMSYSERR_NOERROR)
    {
        mmioClose(_hmmio, 0);
        return false;
    }
    *buff = new_crt char[data_chunk.cksize];
    size = mmioRead(_hmmio, (HPSTR)*buff, data_chunk.cksize);
    if (size != data_chunk.cksize)
    {
        delete[] * buff;
        return false;
    }
    wave_size = size;

    mmioClose(_hmmio, 0);

    return true;
}
}
