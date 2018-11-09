#pragma once

#include <io.hh>
#include <point.hh>

#include <trompeloeil.hpp>

class ILevel;
class IEntityStore;

class MockIo : public IIo
{
public:
    MAKE_MOCK2(setup, void(uint32_t windowWidth, uint32_t windowHeight));

    MAKE_MOCK3(display, void(const point &center, std::shared_ptr<ILevel> level, std::shared_ptr<IEntityStore> store));
    MAKE_MOCK1(msSince, uint32_t(uint32_t last));
    MAKE_MOCK1(delay, void(uint32_t ms));
};

extern std::shared_ptr<MockIo> g_mockIo;
