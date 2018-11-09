#pragma once

#include <input.hh>
#include <trompeloeil.hpp>

class MockInput : public IInput
{
public:
    MAKE_MOCK0(getInput, uint32_t());
};

extern std::shared_ptr<MockInput> g_mockInput;
