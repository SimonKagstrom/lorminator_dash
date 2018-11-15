#pragma once

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
    GRAY       = 17,
    BLACK      = 18,
    DIRT       = 19,
    BOMB_CRATE = 20,
    MAGIC_WALL = 21,
    LEFT_TRANSPORT = 22,
    RIGHT_TRANSPORT= 23,
    DOOR           = 27,
    WALL           = 30,
    TELEPORTER     = 32,
    EXIT           = 34,
    TRANSPORTER    = 36,
};

struct ImageEntry
{
    Image image;
    unsigned frame;
};
