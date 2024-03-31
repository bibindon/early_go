#include "seq_title.hpp"
#include "inline_macro.hpp"
#include "sprite.hpp"

using std::make_shared;
using std::shared_ptr;

namespace early_go
{
seq_title::seq_title(const shared_ptr<IDirect3DDevice9>& d3d_device)
    : d3d_device_ { d3d_device }
{
    sprite_ = make_shared<sprite>(d3d_device_, "image/sprite_test1.png");
}
void seq_title::update()
{
    log_liner { } << "hogehoeg";
}

void seq_title::render()
{
    sprite_->update(D3DXVECTOR3 { });
}

}
