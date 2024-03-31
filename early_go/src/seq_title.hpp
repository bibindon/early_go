#pragma once

#include "seq_interface.hpp"

namespace early_go
{
class seq_title : public seq_interface
{
    virtual void operator()() override;
};
}
