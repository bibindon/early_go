#pragma once

namespace early_go
{
class seq_interface
{
public:
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ~seq_interface();
};
}
