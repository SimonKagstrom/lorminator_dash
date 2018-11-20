#include <io.hh>
#include <game.hh>
#include <resource-store.hh>

#include <optional>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

static std::optional<std::string> resolveFilename(const std::vector<std::string> dirs, const std::string &filename)
{
    for (auto &dir : dirs)
    {
        auto cur = dir + "/" + filename;

        std::ifstream ifs(cur);
        if (ifs.is_open())
        {
            return cur;
        }
    }

    return std::optional<std::string>();
}


int main(int argc, const char *argv[])
{
    auto io = IIo::getInstance();
    auto game = IGame::create();
    auto resourceStore = IResourceStore::getInstance();

    const std::vector<std::string> dirsToSearch =
    {
        "resources",
        "../resources",
        "."
    };

    const std::unordered_map<Image, std::string> imagesToAdd =
    {
        {Image::PLAYER, "sprites/player.png"},
        {Image::BOULDER, "sprites/boulder.png"},
        {Image::GHOST, "sprites/ghost.png"},
        {Image::TILES, "tiles.bmp"},
    };

    for (auto [image, filename] : imagesToAdd)
    {
        auto resolved = resolveFilename(dirsToSearch, filename);

        if (!resolved)
        {
            printf("Can't find %s\n", filename.c_str());
            exit(1);
        }

        resourceStore->addImage(image, *resolved);
   }

    io->setup(1024, 768);

    auto rv = game->setLevel("30 21 "
            "...o....d....................." // b1 Fall
            "... ....d....................."
            "... ....d....................."
            "... .. ......................."
            ".#. t.o  ....................#" // b2 Fall
            ".##d..o .....................#" // The player takes the diamond and then gets hit by the boulder
            ".............................#"
            ".........p...................."
            ".............................."
            ".....................dd......."
            ".............................."
            "....d........................."
            "......................d......."
            "..................ooo........."
            ".............................."
            "........................##...."
            ".............................."
            ".......................###...."
            ".............................."
            ".............................."
            "........t.....................");
    if (!rv)
    {
        printf("Invalid level\n");
        return 1;
    }
    game->play();

    return 0;
}
