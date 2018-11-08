#include <input.hh>
#include <io.hh>

#include <level.hh>
#include <entity.hh>

#include <SDL.h>

class SDL2Io : public IInput, public IIo
{
public:
    // From IInput
    uint32_t getInput() override
    {
        return m_currentInput;
    }

    void display(const point &center, std::shared_ptr<ILevel> level, std::shared_ptr<IEntityStore> store) override
    {
    }

    void setup(uint32_t windowWidth, uint32_t windowHeight) override
    {
    }

    uint32_t msSince(uint32_t last) override
    {
        return SDL_GetTicks() - last;
    }

    void delay(uint32_t ms) override
    {
        SDL_Event ev;

        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
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

    uint32_t m_currentInput{0};
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
