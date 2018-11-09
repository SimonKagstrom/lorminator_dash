#include <io.hh>
#include <game.hh>

int main(int argc, const char *argv[])
{
    auto io = IIo::getInstance();
    auto game = IGame::create();

    io->setup(1024, 768);

    auto rv = game->setLevel("9 9 "
            "...o....d" // b1 Fall
            "... ....d"
            "... ....d"
            "... .. .."
            ".#. t.o  " // b2 Fall
            ".##dp.o ." // The player takes the diamond and then gets hit by the boulder
            "........."
            "........."
            "........t");
    if (!rv)
    {
        printf("Invalid level\n");
        return 1;
    }
    game->play();

    return 0;
}
