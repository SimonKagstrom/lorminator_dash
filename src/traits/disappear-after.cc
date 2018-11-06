#include <behavior.hh>
#include <point.hh>
#include <entity.hh>

#include "itrait.hh"

#include <memory>
#include <vector>

class DisappearAfterTrait : public ITrait
{
public:
    DisappearAfterTrait(int ms, std::shared_ptr<IEntity> entity) :
        m_timeLeft(ms),
        m_entity(entity)
    {
    }

    bool run(unsigned ms) override
    {
        m_timeLeft -= ms;

        // The time has expired, so remove
        if (m_timeLeft <= 0)
        {
            m_entity->remove();
        }

        return false;
    }

private:
    int m_timeLeft;
    std::shared_ptr<IEntity> m_entity;
};
