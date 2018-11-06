#pragma once

#include <memory>

enum InputTypes
{
    UP = 1,
    DOWN = 2,
    LEFT = 4,
    RIGHT = 8,
    OPERATE = 16,
    BOMB = 32,
};

class IEntity;

class IInput
{
public:
    virtual ~IInput()
    {
    }

    virtual uint32_t getInput() = 0;

    static std::shared_ptr<IInput> fromEntity(std::shared_ptr<IEntity> entity);
};
