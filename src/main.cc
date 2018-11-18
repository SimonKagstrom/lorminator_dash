#include <io.hh>
#include <game.hh>

#include <optional>
#include <fstream>
#include <vector>
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

    return nullptr;
}


int main(int argc, const char *argv[])
{
    auto io = IIo::getInstance();
    auto game = IGame::create();

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
