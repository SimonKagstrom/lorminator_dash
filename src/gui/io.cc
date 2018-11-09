#include <input.hh>
#include <io.hh>

#include <level.hh>
#include <entity.hh>

#include <point.hh>

#include <SDL.h>

class SDL2Io : public IInput, public IIo
{
public:
    SDL2Io()
    {
        SDL_Init(SDL_INIT_VIDEO);
    }

    ~SDL2Io()
    {
        if (m_sprites)
        {
//            SDL_FreeTexture(m_sprites);
        }
        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
        }
        if (m_window)
        {
            SDL_DestroyWindow(m_window);
        }

        SDL_Quit();
    }

    // From IInput
    uint32_t getInput() override
    {
        return m_currentInput;
    }

    void display(const point &center, std::shared_ptr<ILevel> level, std::shared_ptr<IEntityStore> store) override
    {
        for (int y = 0; y < level->getSize().width; y++)
        {
            for (int x = 0; x < level->getSize().height; x++)
            {
                auto cur = (point){x,y};
                auto scaled = cur * m_spriteSize.width;

                auto tile = level->tileAt(cur);

                if (!tile)
                {
                    continue;
                }

                auto off = getSpriteFromTile(*tile);
                auto rect = getRectFromOffset(off);

                SDL_Rect dst = {scaled.x, scaled.y, (int)m_spriteSize.width, (int)m_spriteSize.height};

                SDL_RenderCopy(m_renderer, m_tiles, &rect, &dst);
            }
        }

        auto entities = store->getEntities();
        for (auto &it : entities)
        {
            auto scaled = it->getPosition() * m_spriteSize.width;

            auto off = getSpriteFromEntity(it->getType());
            auto rect = getRectFromOffset(off);

            SDL_Rect dst = {scaled.x, scaled.y, (int)m_spriteSize.width, (int)m_spriteSize.height};

            SDL_RenderCopy(m_renderer, m_sprites, &rect, &dst);
        }

        SDL_RenderPresent(m_renderer);
    }

    void setup(uint32_t windowWidth, uint32_t windowHeight) override
    {
        m_window = SDL_CreateWindow("Dash", 100, 100, windowWidth, windowHeight,
                        SDL_WINDOW_SHOWN);
        if (!m_window)
        {
            printf("Can't open SDL window???\n");
            exit(1);
        }

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!m_renderer)
        {
            printf("Can't open SDL renderer???\n");
            exit(1);
        }


        auto sprites = SDL_LoadBMP("../resources/sprites.bmp");
        if (!sprites)
        {
            printf("Looking for sprites.bmp in ../resources, failed...\n");
            exit(1);
        }
        m_sprites = SDL_CreateTextureFromSurface(m_renderer, sprites);

        auto tiles = SDL_LoadBMP("../resources/tiles.bmp");
        if (!tiles)
        {
            printf("Looking for sprites.bmp in ../resources, failed...\n");
            exit(1);
        }
        m_tiles = SDL_CreateTextureFromSurface(m_renderer,tiles);

        m_spriteSize = (extents){(unsigned)sprites->w, (unsigned)sprites->w};
    }

    uint32_t msSince(uint32_t last) override
    {
        return SDL_GetTicks() - last;
    }

    void delay(uint32_t ms) override
    {
        SDL_Event ev;

        SDL_Delay(ms);
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT ||
                (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
            {
                exit(1);
            }

            updateKeys(ev);
        }
    }

private:
    void updateKeys(const SDL_Event &ev)
    {
        if (ev.type == SDL_KEYDOWN)
        {
            m_currentInput |= ev.key.keysym.sym == SDLK_UP ? InputTypes::UP : 0;
            m_currentInput |= ev.key.keysym.sym == SDLK_DOWN ? InputTypes::DOWN : 0;
            m_currentInput |= ev.key.keysym.sym == SDLK_LEFT ? InputTypes::LEFT : 0;
            m_currentInput |= ev.key.keysym.sym == SDLK_RIGHT ? InputTypes::RIGHT : 0;
            m_currentInput |= ev.key.keysym.sym == SDLK_SPACE ? InputTypes::OPERATE : 0;
            m_currentInput |= ev.key.keysym.sym == SDLK_b ? InputTypes::BOMB : 0;
        }
        else if (ev.type == SDL_KEYUP)
        {
            m_currentInput &= ev.key.keysym.sym == SDLK_UP ? ~InputTypes::UP : ~0;
            m_currentInput &= ev.key.keysym.sym == SDLK_DOWN ? ~InputTypes::DOWN : ~0;
            m_currentInput &= ev.key.keysym.sym == SDLK_LEFT ? ~InputTypes::LEFT : ~0;
            m_currentInput &= ev.key.keysym.sym == SDLK_RIGHT ? ~InputTypes::RIGHT : ~0;
            m_currentInput &= ev.key.keysym.sym == SDLK_SPACE ? ~InputTypes::OPERATE : ~0;
            m_currentInput &= ev.key.keysym.sym == SDLK_b ? ~InputTypes::BOMB : ~0;
        }
    }

    int getSpriteFromTile(TileType tile) const
    {
        const std::unordered_map<TileType, int> transform =
        {
            {TileType::EMPTY, 1},
            {TileType::DIRT, 2},
            {TileType::MAGIC_WALL, 4},
            {TileType::LEFT_TRANSPORT, 5},
            {TileType::RIGHT_TRANSPORT, 6},
            {TileType::STONE_WALL, 13},
            {TileType::WEAK_STONE_WALL, 14},
            {TileType::TELEPORTER, 15},
            {TileType::CONVEYOR, 21},
            {TileType::EXIT, 15},
        };

        auto it = transform.find(tile);

        return it->second;
    }

    int getSpriteFromEntity(EntityType tile) const
    {
        const std::unordered_map<EntityType, int> transform =
        {
            {EntityType::BOULDER, 4},
            {EntityType::BLOCK, 15},
            {EntityType::GHOST, 9},
            {EntityType::PLAYER, 0},
            {EntityType::DIAMOND, 5},
            {EntityType::BOMB, 8},
            {EntityType::IRON_KEY, 12},
            {EntityType::GOLD_KEY, 13},
            {EntityType::RED_KEY, 14},
            {EntityType::FIREBALL, 10},

        };

        auto it = transform.find(tile);

        return it->second;
    }

    SDL_Rect getRectFromOffset(int offset) const
    {
        SDL_Rect out {0, offset * (int)m_spriteSize.height, (int)m_spriteSize.width, (int)m_spriteSize.height};

        return out;
    }

    uint32_t m_currentInput{0};

    SDL_Window *m_window{nullptr};
    SDL_Renderer *m_renderer{nullptr};
    SDL_Texture *m_sprites{nullptr};
    SDL_Texture *m_tiles{nullptr};

    struct extents m_spriteSize;
};

std::shared_ptr<IIo> IIo::getInstance()
{
    static std::weak_ptr<IIo> g_instance;

    if (auto p = g_instance.lock())
    {
        return p;
    }

    // Create a new one
    auto p = std::shared_ptr<IIo>(new SDL2Io());
    g_instance = p;

    return p;
}

std::shared_ptr<IInput> IInput::fromEntity(std::shared_ptr<IEntity> entity)
{
    if (entity->getType() != EntityType::PLAYER)
    {
        return nullptr;
    }

    auto io = std::dynamic_pointer_cast<SDL2Io>(IIo::getInstance());

    return io;
}
