#ifndef TEXT_D3DXFONT_HPP
#define TEXT_D3DXFONT_HPP
#include "stdafx.hpp"

namespace early_go
{
class text_d3dxfont
{
public:
    text_d3dxfont(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    void operator()(const std::string& message, const int& x, const int& y, const DWORD& color);
private:
    std::shared_ptr<ID3DXFont> d3dxfont_;
};
} // namespace early_go 
#endif
