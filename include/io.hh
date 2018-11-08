#pragma once

#include <memory>

#include <point.hh>

class IEntityStore;
class ILevel;

class IIo
{
public:
    virtual ~IIo()
    {
    }

    virtual void setup(uint32_t windowWidth, uint32_t windowHeight) = 0;

    virtual void display(const point &center, std::shared_ptr<ILevel> level, std::shared_ptr<IEntityStore> store) = 0;
    virtual uint32_t msSince(uint32_t last) = 0;
    virtual void delay(uint32_t ms) = 0;



    static std::shared_ptr<IIo> getInstance();
};
