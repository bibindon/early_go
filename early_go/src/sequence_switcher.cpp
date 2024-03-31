#include "sequence_switcher.hpp"
#include "seq_title.hpp"

namespace early_go
{
sequence_switcher::sequence_switcher()
{
    sequence_.reset(new_crt seq_title { });
}
void sequence_switcher::operator()()
{
    (*sequence_)();
}
}

