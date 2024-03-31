#include "sequence_switcher.hpp"
#include "seq_title.hpp"

namespace early_go
{
sequence_switcher::sequence_switcher(const std::shared_ptr<IDirect3DDevice9>& d3d_device)
    : d3d_device_ { d3d_device }
{
    sequence_.reset(new_crt seq_title { d3d_device_ });
}
void sequence_switcher::operator()()
{
    (*sequence_)();
}
void sequence_switcher::render()
{
    sequence_->render();
}
}

