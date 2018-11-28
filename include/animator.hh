#pragma once

#include <memory>

#include <image.hh>
#include <entity.hh>

struct point;
struct extents;
class ILevel;

class IAnimator
{
public:
    virtual ~IAnimator()
    {
    }

    virtual void animate(unsigned round) = 0;

    virtual point getPixelPosition() const = 0;

    virtual ImageEntry getFrame() const = 0;


    static std::unique_ptr<IAnimator> fromEntity(std::shared_ptr<IEntity> entity, const extents &size, int nFrames);
    static ImageEntry imageEntryFromType(EntityType type);
};
