#include <behavior.hh>
#include <point.hh>
#include <entity.hh>
#include <level.hh>

#include "itrait.hh"

#include <memory>
#include <vector>

class ExplodeAfterTrait : public ITrait
{
public:
    ExplodeAfterTrait(int ms, std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) :
        m_timeLeft(ms),
        m_level(level),
        m_entity(entity)
    {
    }

    bool run(unsigned ms) override
    {
        m_timeLeft -= ms;

        // The time has expired, so explode!
        if (m_timeLeft <= 0)
        {
            auto where = m_entity->getPosition();

            m_entity->remove();
            m_level->explode(where);
        }

        return false;
    }

private:
    int m_timeLeft;
    std::shared_ptr<ILevel> m_level;
    std::shared_ptr<IEntity> m_entity;
};
