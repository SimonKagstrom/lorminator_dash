#pragma once

#include <resource-store.hh>

#include <trompeloeil.hpp>

#include <memory>

class MockResourceStore : public IResourceStore
{
    public:

    MAKE_MOCK2(addImage, void(Image image, const std::string &filename));

    MAKE_CONST_MOCK1(getImageFrameCount, unsigned(Image image));

    MAKE_CONST_MOCK0(getFrameExtents, extents());

    MAKE_MOCK1(getImageFrame, void*(const ImageEntry &entry));
};

extern std::shared_ptr<MockResourceStore> g_mockResourceStore;
