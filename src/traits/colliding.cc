#include <behavior.hh>
#include "itrait.hh"

#include <memory>
#include <vector>
#include <functional>

class CollisionTrait : public ITrait
{
public:
    CollisionTrait()
    {
        auto store = IEntityStore::getInstance();

        m_cookie = store->onCollision([this](std::shared_ptr<IEntity> one, std::shared_ptr<IEntity> other){onCollision(one, other);});
    }

    bool run(unsigned ms) override
    {
        // Handled in the callback
        return false;
    }

    void onCollision(std::shared_ptr<IEntity> one, std::shared_ptr<IEntity> other)
    {
        ifOne(one, other, EntityType::FIREBALL,
            [](std::shared_ptr<IEntity> which)
        {
            which->remove();
        });

        // More to come
    }

private:
    void ifOne(std::shared_ptr<IEntity> one, std::shared_ptr<IEntity> other, EntityType type,
        std::function<void(std::shared_ptr<IEntity>)> onTrue) const
    {
        if (one->getType() == type)
        {
            onTrue(other);
        }
        else if (other->getType() == type)
        {
            onTrue(one);
        }
    }

    void ifPair(std::shared_ptr<IEntity> one, std::shared_ptr<IEntity> other, EntityType type, EntityType type2,
        std::function<void(std::shared_ptr<IEntity>)> onTrue) const
    {
        if (one->getType() == type && other->getType() == type2)
        {
            onTrue(other);
        }
        else if (other->getType() == type && one->getType() == type2)
        {
            onTrue(one);
        }
    }

    std::unique_ptr<ObserverCookie> m_cookie;
};
