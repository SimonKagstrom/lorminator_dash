#pragma once

#include <string>

class IGame
{
public:
    virtual bool setLevel(const std::string &levelData) = 0;

    virtual bool play() = 0;
};
