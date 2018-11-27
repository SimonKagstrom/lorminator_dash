#include <animator.hh>
#include <observer.hh>

#include <point.hh>
#include <entity.hh>

#include <resource-store.hh>

#include <list>

class Animator : public IAnimator
{
public:
    Animator(Image frame, std::shared_ptr<IEntity> entity, int width, unsigned nFrames, int nRounds) :
        m_frame({frame, 0}),
        m_pixelPosition(entity->getPosition() * width),
        m_entity(entity),
        m_width(width),
        m_nFrames(nFrames),
        m_nRounds(nRounds)
    {
        m_movementCookie = m_entity->onMovement([this](std::shared_ptr<IEntity> entity, const point &from, const point &to)
        {
            handleMovement(from, to);
        });
    }

    virtual void animate(unsigned round) override
    {
        if (m_frameHandlers.empty())
        {
            return;
        }
        auto &cur = m_frameHandlers.front();
        m_frameHandlers.pop_front();

        m_pixelPosition = cur.dst;
        m_frame.frame = selectFrame(round);
    }

    virtual point getPixelPosition() const override
    {
        return m_pixelPosition;
    }

    virtual ImageEntry getFrame() const override
    {
        return m_frame;
    }

protected:
    struct FrameHandler
    {
        FrameHandler(const point &to) :
            dst(to)
        {
        }

        const point dst;
    };

    virtual unsigned selectFrame(unsigned round) = 0;

    void handleMovement(const point &from, const point &to)
    {
        const int pixelsPerFrame = m_width / m_nRounds;
        int dx = to.x - from.x;
        int dy = to.y - from.y;

        point diff = (point){dx,dy} * pixelsPerFrame;

        m_frameHandlers.clear();
        m_pixelPosition = from * m_width;

        if (std::abs(dx) > 1 || std::abs(dy) > 1 || // Long movement
            (std::abs(dx) && std::abs(dy)))         // Not Manhattan-style movement
        {
            // Just move in a single frame
            m_frameHandlers.push_back(FrameHandler(to * m_width));
            return;
        }

        for (unsigned i = 1; i <= m_nRounds; i++)
        {
            m_frameHandlers.push_back(FrameHandler(m_pixelPosition + (diff * i)));
        }
    }

    ImageEntry m_frame;
    point m_pixelPosition;
    const std::shared_ptr<IEntity> m_entity;
    const int m_width;
    const unsigned m_nFrames;
    const int m_nRounds;

    std::unique_ptr<ObserverCookie> m_movementCookie;
    std::list<FrameHandler> m_frameHandlers;
};

class DefaultAnimator : public Animator
{
public:
    DefaultAnimator(Image frame, std::shared_ptr<IEntity> entity, int width, unsigned nFrames, int nRounds) :
        Animator(frame, entity, width, nFrames, nRounds)
    {
    }

    virtual unsigned selectFrame(unsigned round) override
    {
        return round % m_nFrames;
    }
};

class PlayerAnimator : public Animator
{
public:
    PlayerAnimator(std::shared_ptr<IEntity> entity, int width, int nRounds) :
        Animator(Image::PLAYER, entity, width, 3, nRounds)
    {
    }

protected:
    unsigned selectFrame(unsigned round) override
    {
        const std::vector<unsigned> offsetByDirection =
        {
            9, // Up
            6, // Down
            3, // Left
            0, // Right
            0  // None
        };

        return offsetByDirection[(unsigned)m_entity->getDirection()] + round % 3;
    }
};

class Gem : public Animator
{
public:
    Gem(std::shared_ptr<IEntity> entity, int width, int nRounds) :
        Animator(Image::GEM, entity, width, 1, nRounds),
        m_gemFrame(random() % IResourceStore::getInstance()->getImageFrameCount(Image::GEM))
    {
        m_frame.frame = m_gemFrame;
    }

    unsigned selectFrame(unsigned round) override
    {
        return m_gemFrame;
    }

private:
    const unsigned m_gemFrame;
};

class FireballAnimator : public DefaultAnimator
{
public:
    FireballAnimator(std::shared_ptr<IEntity> entity, int width, int nRounds) :
        DefaultAnimator(Image::FIREBALL, entity, width, IResourceStore::getInstance()->getImageFrameCount(Image::FIREBALL), nRounds)
    {
    }

    virtual void animate(unsigned round) override
    {
        DefaultAnimator::animate(round);

        m_frame.frame = selectFrame(round);
    }
};

std::unique_ptr<IAnimator> IAnimator::fromEntity(std::shared_ptr<IEntity> entity, const extents &size, int nRounds)
{
    auto resourceStore = IResourceStore::getInstance();

    switch (entity->getType())
    {
    case EntityType::BOULDER:
        return std::make_unique<DefaultAnimator>(Image::BOULDER, entity, size.width, resourceStore->getImageFrameCount(Image::BOULDER), nRounds);
    case EntityType::FIREBALL:
        return std::make_unique<FireballAnimator>(entity, size.width, nRounds);
    case EntityType::DIAMOND:
        return std::make_unique<Gem>(entity, size.width, nRounds);
    case EntityType::PLAYER:
        return std::make_unique<PlayerAnimator>(entity, size.width, nRounds);
    default:
        break;
    }

    return std::make_unique<DefaultAnimator>(Image::PLAYER, entity, size.width, resourceStore->getImageFrameCount(Image::PLAYER), nRounds);
}

ImageEntry IAnimator::imageEntryFromType(EntityType type)
{
    switch(type)
    {
    case EntityType::BOULDER:
        return {Image::BOULDER, 0};
    case EntityType::FIREBALL:
        return {Image::FIREBALL, 0};
    case EntityType::DIAMOND:
        return {Image::GEM, 0};
    case EntityType::BOMB:
        return {Image::BOMB, 0};
    default:
        break;
    }

    return {Image::PLAYER, 0};
}
