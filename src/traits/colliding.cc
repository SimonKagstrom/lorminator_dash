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
        if (one->getType() == EntityType::FIREBALL)
        {
            other->remove();
        }
        else if (other->getType() == EntityType::FIREBALL)
        {
            one->remove();
        }

        // More to come
    }

private:
    std::unique_ptr<ObserverCookie> m_cookie;
};
