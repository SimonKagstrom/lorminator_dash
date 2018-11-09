#pragma once

#include <string>
#include <memory>

class IGame
{
public:
    virtual bool setLevel(const std::string &levelData) = 0;

    virtual bool play() = 0;


    static std::shared_ptr<IGame> create();
};
