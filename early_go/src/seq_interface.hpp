#pragma once

namespace early_go
{
class seq_interface
{
public:
    virtual void operator()() = 0;
    virtual void render() = 0;
    virtual ~seq_interface();
};
}
