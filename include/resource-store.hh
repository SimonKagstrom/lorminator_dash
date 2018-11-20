#pragma once

#include <memory>
#include <vector>
#include <string>

#include "image.hh"
#include "point.hh"

class IResourceStore
{
public:
    virtual ~IResourceStore()
    {
    }

    virtual void addImage(Image image, const std::string &filename) = 0;

    virtual unsigned getImageFrameCount(Image image) const = 0;

    virtual void *getImageFrame(const ImageEntry &entry) = 0;

    virtual extents getFrameExtents() const = 0;


    static std::shared_ptr<IResourceStore> getInstance();
};
