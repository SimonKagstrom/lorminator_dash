#include <animator.hh>
#include <observer.hh>

#include <point.hh>
#include <entity.hh>

#include <list>

class Animator : public IAnimator
{
public:
    Animator(Image frame, std::shared_ptr<IEntity> entity, int width, int nFrames) :
        m_frame({frame, 0}),
        m_pixelPosition(entity->getPosition() * width),
        m_entity(entity),
        m_width(width),
        m_nFrames(nFrames)
    {
        m_movementCookie = m_entity->onMovement([this](std::shared_ptr<IEntity> entity, const point &from, const point &to)
        {
            handleMovement(from, to);
        });
    }

    virtual void animate(unsigned round)
    {
        if (m_frameHandlers.empty())
        {
            return;
        }
        auto &cur = m_frameHandlers.front();
        m_frameHandlers.pop_front();

        m_pixelPosition = cur.dst;
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
            m_frameHandlers.push_back(FrameHandler(to * m_width));
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

        int pixelsPerFrame = m_width / m_nFrames;
        for (unsigned i = 1; i <= m_nFrames; i++)
        {
            point diff;

            diff = (diff + dir) * (i * pixelsPerFrame);
            m_frameHandlers.push_back(FrameHandler(m_pixelPosition + diff));
        }
    }

    ImageEntry m_frame;
    point m_pixelPosition;
    const std::shared_ptr<IEntity> m_entity;
    const int m_width;
    const int m_nFrames;

    std::unique_ptr<ObserverCookie> m_movementCookie;
    std::list<FrameHandler> m_frameHandlers;
};

std::unique_ptr<IAnimator> IAnimator::fromEntity(std::shared_ptr<IEntity> entity, const extents &size, int nFrames)
{
    return std::make_unique<Animator>(Image::PLAYER, entity, size.width, nFrames);
}
