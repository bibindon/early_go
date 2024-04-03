#include "sequence_switcher.hpp"
#include "seq_title.hpp"
#include "seq_novel.hpp"

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
    seq_type _seq_type_result = sequence_->update(_early, _suo, _camera);
    if (seq_type_ != _seq_type_result)
    {
        seq_type_ = _seq_type_result;
        if (_seq_type_result == seq_type::EXIT)
        {
            PostQuitMessage(0);
        }
        else if (_seq_type_result == seq_type::NOVEL)
        {
            sequence_.reset(new_crt seq_novel { d3d_device_, _early });
        }
        else if (_seq_type_result == seq_type::BATTLE)
        {
//            sequence_.reset(new_crt seq_battle { d3d_device_ });
        }
    }
}
void sequence_switcher::render(
    const shared_ptr<character>& _early,
    const shared_ptr<character>& _suo,
    const shared_ptr<camera>& _camera)
{
    sequence_->render(_early, _suo, _camera);
}
}

