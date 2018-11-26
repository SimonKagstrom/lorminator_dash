#include <input.hh>
#include <io.hh>

#include <level.hh>
#include <entity.hh>
#include <animator.hh>

#include <point.hh>
#include <resource-store.hh>
#include <lightning.hh>

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
        for (auto [key, texture] : m_imageEntryToTexture)
        {
            SDL_DestroyTexture(texture);
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

    void display(const std::shared_ptr<IEntity> centerIn, std::shared_ptr<ILevel> level,
        const std::shared_ptr<ILightning> lightning,
        const std::unordered_map<uint32_t, std::shared_ptr<IAnimator>> &animators) override
    {
        int windowWidth, windowHeight;

        auto resourceStore = IResourceStore::getInstance();
        auto frameSize = resourceStore->getFrameExtents();

        SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

        auto it = animators.find(centerIn->getId());
        point pt = m_lastCenter;
        if (it != animators.end())
        {
            const auto centerAnimator = it->second;
            pt = m_lastCenter = centerAnimator->getPixelPosition();
        }

        auto center = (point){pt.x - windowWidth / 2, pt.y - windowHeight / 2};

        if (center.x < 0)
        {
            center.x = 0;
        }
        if (center.y < 0)
        {
            center.y = 0;
        }

        auto levelSize = level->getSize();

        if (center.x + windowWidth >= levelSize.width * frameSize.width)
        {
            center.x = levelSize.width  * frameSize.width - windowWidth;
        }
        if (center.y + windowHeight >= levelSize.height * frameSize.width)
        {
            center.y = levelSize.height * frameSize.width - windowHeight;
        }

        SDL_RenderClear(m_renderer);
        for (int y = 0; y < levelSize.height; y++)
        {
            for (int x = 0; x < levelSize.width; x++)
            {
                auto cur = (point){x,y};
                auto tile = lightning->tileAt(cur);

                if (!tile)
                {
                    continue;
                }

                auto scaled = cur * frameSize.width;

                scaled = scaled - center;
                if (scaled.x < -(int)frameSize.width || scaled.y < -(int)frameSize.height)
                {
                    continue;
                }

                auto off = getFrameFromTile(*tile);
                auto texture = getTextureFromImageEntry({Image::TILES, off});

                SDL_Rect dst = {scaled.x, scaled.y, (int)frameSize.width, (int)frameSize.height};
                SDL_RenderCopy(m_renderer, texture, nullptr, &dst);
            }
        }

        for (auto &it : animators)
        {
            auto cur = it.second->getPixelPosition() - center;

            if (cur.x < -(int)frameSize.width || cur.y < -(int)frameSize.height)
            {
                continue;
            }

            auto imageEntry = it.second->getFrame();
            auto texture = getTextureFromImageEntry(imageEntry);

            SDL_Rect dst = {cur.x, cur.y, (int)frameSize.width, (int)frameSize.height};

            SDL_RenderCopy(m_renderer, texture, nullptr, &dst);
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

        auto gray = getTextureFromImageEntry({Image::GRAY, 0});
        SDL_SetTextureBlendMode(gray, SDL_BLENDMODE_MOD);
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
    SDL_Texture *getTextureFromImageEntry(const ImageEntry &entry)
    {
        auto it = m_imageEntryToTexture.find(entry);

        if (it != m_imageEntryToTexture.end())
        {
            return it->second;
        }

        auto resourceStore = IResourceStore::getInstance();

        auto surface = (SDL_Surface*)resourceStore->getImageFrame(entry);
        auto texture = SDL_CreateTextureFromSurface(m_renderer, surface);

        if (!surface)
        {
            throw std::invalid_argument("No surface?");
        }
        if (!texture)
        {
            throw std::invalid_argument("No texture?");
        }

        m_imageEntryToTexture[entry] = texture;

        return texture;
    }

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

    unsigned getFrameFromTile(TileType tile) const
    {
        const std::unordered_map<TileType, unsigned> transform =
        {
            {TileType::UNKNOWN, 0},
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

    uint32_t m_currentInput{0};

    SDL_Window *m_window{nullptr};
    SDL_Renderer *m_renderer{nullptr};

    std::unordered_map<ImageEntry, SDL_Texture *> m_imageEntryToTexture;
    point m_lastCenter{0,0};
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
