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
        m_frame.frame = cur.frame;
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
        FrameHandler(const point &to, unsigned frameIn) :
            dst(to),
            frame(frameIn)
        {
        }

        const point dst;
        const unsigned frame;
    };

    virtual unsigned selectFrame(unsigned round) = 0;

    void handleMovement(const point &from, const point &to)
    {
        int dx = to.x - from.x;
        int dy = to.y - from.y;

        m_frameHandlers.clear();
        m_pixelPosition = from * m_width;

        if (std::abs(dx) > 1 || std::abs(dy) > 1 || // Long movement
            (std::abs(dx) && std::abs(dy)))         // Not Manhattan-style movement
        {
            // Just move in a single frame
            m_frameHandlers.push_back(FrameHandler(to * m_width, 0));
            return;
        }

        Direction dir{Direction::NONE};
        if (from.x < to.x && from.y == to.y)
        {
            dir = Direction::RIGHT;
        }
        else if (from.x > to.x && from.y == to.y)
        {
            dir = Direction::LEFT;
        }
        else if (from.x == to.x && from.y > to.y)
        {
            dir = Direction::UP;
        }
        else if (from.x == to.x && from.y < to.y)
        {
            dir = Direction::DOWN;
        }

        int pixelsPerFrame = m_width / m_nRounds;
        for (unsigned i = 1; i <= m_nRounds; i++)
        {
            point diff;

            diff = (diff + dir) * (i * pixelsPerFrame);
            m_frameHandlers.push_back(FrameHandler(m_pixelPosition + diff, selectFrame(i)));
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

std::unique_ptr<IAnimator> IAnimator::fromEntity(std::shared_ptr<IEntity> entity, const extents &size, int nRounds)
{
    auto resourceStore = IResourceStore::getInstance();

    switch (entity->getType())
    {
    case EntityType::BOULDER:
        return std::make_unique<DefaultAnimator>(Image::BOULDER, entity, size.width, resourceStore->getImageFrameCount(Image::BOULDER), nRounds);
    case EntityType::DIAMOND:
        return std::make_unique<Gem>(entity, size.width, nRounds);
    default:
        break;
    }

    return std::make_unique<DefaultAnimator>(Image::PLAYER, entity, size.width, resourceStore->getImageFrameCount(Image::PLAYER), nRounds);
}
