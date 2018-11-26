#pragma once

#include <utility>

enum class Image
{
    PLAYER   =  0,
    BOULDER  =  4,
    GEM      =  5,
    BOMB     =  8,
    GHOST    =  9,
    FIREBALL = 10,
    KEY      = 12,
    BLOCK    = 15,
    LEVER    = 16,

    // Tiles
    TILES,

    // Special
    GRAY,
};

struct ImageEntry
{
    Image image;
    unsigned frame;

    bool operator==(const ImageEntry &other) const
    {
        return image == other.image && frame == other.frame;
    }
};

namespace std
{
template <>
struct hash<ImageEntry>
{
    std::size_t operator()(const ImageEntry& k) const
    {
        return std::hash<unsigned>()((unsigned)k.image) ^ std::hash<unsigned>()(k.frame);
    }
};
}
