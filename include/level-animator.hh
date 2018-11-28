#pragma once

#include <image.hh>

#include <memory>

struct point;
class ILightning;


class ILevelAnimator
{
public:
    virtual ~ILevelAnimator()
    {
    }

    virtual void animate(unsigned round) = 0;

    virtual ImageEntry getImageEntryAt(const point &where) const = 0;


    static std::unique_ptr<ILevelAnimator> fromLightning(std::shared_ptr<ILightning> lightning);
};
