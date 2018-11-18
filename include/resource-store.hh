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

    virtual void *getImageFrame(const ImageEntry &entry) = 0;


    static extents getFrameExtents();
    static std::shared_ptr<IResourceStore> getInstance();
};
