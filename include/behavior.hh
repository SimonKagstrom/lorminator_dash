#pragma once

#include <memory>

class IEntity;
class ILevel;

class IBehavior
{
public:
    virtual ~IBehavior()
    {
    }

    virtual void run(unsigned ms) = 0;

    static std::unique_ptr<IBehavior> fromEntity(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity);
    static std::unique_ptr<IBehavior> fromLevel(std::shared_ptr<ILevel> level);
};
