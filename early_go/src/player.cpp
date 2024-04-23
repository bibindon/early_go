#include "light.hpp"
#include "player.hpp"

using std::shared_ptr;
using std::string;

namespace early_go
{
void player::update(const std::shared_ptr<stage>& _stage)
{
    character2::update(_stage);
}
}

