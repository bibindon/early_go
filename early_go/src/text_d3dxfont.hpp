#ifndef TEXT_D3DXFONT_HPP
#define TEXT_D3DXFONT_HPP
#include "stdafx.hpp"

namespace early_go
{
class text_d3dxfont
{
public:
    text_d3dxfont(const std::shared_ptr<IDirect3DDevice9>& d3d_device, const INT height_);
    void set_param(const std::string& message, const int& x, const int& y, const DWORD& color);
    void operator()();
private:
    std::shared_ptr<ID3DXFont> d3dxfont_ { };
    std::string message_ { };
    int x_ { 0 };
    int y_ { 0 };
    DWORD color_ { 0 };
};
} // namespace early_go 
#endif
