#include "sequence_switcher.hpp"
#include "seq_title.hpp"

using std::shared_ptr;

namespace early_go
{
sequence_switcher::sequence_switcher(const std::shared_ptr<IDirect3DDevice9>& d3d_device)
    : d3d_device_ { d3d_device }
{
    sequence_.reset(new_crt seq_title { d3d_device_ });
}
void sequence_switcher::update(
        const std::shared_ptr<character>& _early,
        const std::shared_ptr<character>& _suo,
        const std::shared_ptr<camera>& _camera)
{
    sequence_->update(_early, _suo, _camera);
}
void sequence_switcher::render(
    const shared_ptr<character>& _early,
    const shared_ptr<character>& _suo,
    const shared_ptr<camera>& _camera)
{
    sequence_->render(_early, _suo, _camera);
}
}

