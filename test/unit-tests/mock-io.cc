#include "mock-io.hh"

std::shared_ptr<MockIo> g_mockIo;
std::shared_ptr<IIo> IIo::getInstance()
{
    return g_mockIo;
}
