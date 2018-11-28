#pragma once

#include <memory>
#include <unordered_map>

#include <point.hh>

class IEntityStore;
class ILevel;
class IEntity;
class IAnimator;
class ILevelAnimator;
class ILightning;

class IIo
{
public:
    virtual ~IIo()
    {
    }

    virtual void setup(uint32_t windowWidth, uint32_t windowHeight) = 0;

    virtual void display(const std::shared_ptr<IEntity> center, std::shared_ptr<ILevel> level,
        const std::shared_ptr<ILightning> lightning, std::shared_ptr<ILevelAnimator> levelAnimator,
        const std::unordered_map<uint32_t, std::shared_ptr<IAnimator>> &animators) = 0;
    virtual uint32_t msSince(uint32_t last) = 0;
    virtual void delay(uint32_t ms) = 0;



    static std::shared_ptr<IIo> getInstance();
};
