#pragma once

#include <memory>
#include <string>

class IEntity;

class IEntityProperties
{
public:
    class IProperties
    {
    public:
        virtual ~IProperties()
        {
        }

        virtual int asInt(const std::string &key) const = 0;

        virtual void set(const std::string &key, int value) = 0;
    };

    virtual ~IEntityProperties()
    {
    }

    virtual std::shared_ptr<IProperties> fromEntity(std::shared_ptr<IEntity> entity) = 0;

    static std::shared_ptr<IEntityProperties> getInstance();
};
