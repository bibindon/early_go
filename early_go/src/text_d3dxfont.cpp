#include "text_d3dxfont.hpp"

using std::shared_ptr;
using std::string;

namespace early_go
{
text_d3dxfont::text_d3dxfont(const shared_ptr<IDirect3DDevice9>& d3d_device)
{
    LPD3DXFONT d3dxfont { };

    D3DXFONT_DESC desc;
    desc.Height = 50;
    desc.Width = 0;
    desc.Weight = FW_NORMAL;
    desc.MipLevels = 0;
    desc.Italic = FALSE;
    desc.CharSet = SHIFTJIS_CHARSET;
    desc.OutputPrecision = OUT_TT_ONLY_PRECIS;
    desc.Quality = DEFAULT_QUALITY;
    desc.PitchAndFamily = FIXED_PITCH | FF_DONTCARE;
    lstrcpy(desc.FaceName, constants::FONT_NAME.c_str());

    HRESULT hresult { D3DXCreateFontIndirect(d3d_device.get(), &desc, &d3dxfont) };

    if (FAILED(hresult))
    {
        THROW_WITH_TRACE("Failed to create a font.");
    }
    d3dxfont_.reset(d3dxfont, custom_deleter { });
}

void text_d3dxfont::operator()(const std::string& message, const int& x, const int& y,
    const DWORD& color)
{
    RECT rect { x, y, 0 , 0};

    d3dxfont_->DrawText(
        NULL,
        message.c_str(),
        -1,
        &rect,
        DT_CALCRECT,
        NULL);

    d3dxfont_->DrawText(
        NULL,
        message.c_str(),
        -1,
        &rect,
        DT_LEFT | DT_BOTTOM,
        color);
}

}
