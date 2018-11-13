#pragma once

#include <io.hh>
#include <point.hh>

#include <trompeloeil.hpp>

#include <memory>

class MockIo : public IIo
{
public:
    MAKE_MOCK2(setup, void(uint32_t windowWidth, uint32_t windowHeight));

    MAKE_MOCK3(display, void(const std::shared_ptr<IEntity> center, std::shared_ptr<ILevel> level, const std::unordered_map<uint32_t, std::shared_ptr<IAnimator>> &animators));
    MAKE_MOCK1(msSince, uint32_t(uint32_t last));
    MAKE_MOCK1(delay, void(uint32_t ms));
};

extern std::shared_ptr<MockIo> g_mockIo;
