#include <entity-properties.hh>
#include <entity.hh>

#include <unordered_map>

class Properties : public IEntityProperties::IProperties
{
public:
    Properties()
    {
    }

    int asInt(const std::string &key) const override
    {
        auto it = m_props.find(key);

        if (it != m_props.end())
        {
            return it->second;            
        }

        return 0;
    }

    void set(const std::string &key, int value) override
    {
        m_props[key] = value;
    }

private:
    std::unordered_map<std::string, int> m_props;
};


class EntityProperties : public IEntityProperties
{
public:
    std::shared_ptr<IProperties> fromEntity(std::shared_ptr<IEntity> entity) override
    {
        auto id = entity->getId();
        auto it = m_entityProperties.find(id);

        if (it != m_entityProperties.end())
        {
            return it->second;
        }

        m_entityProperties[id] = std::make_shared<Properties>();

        return m_entityProperties[id];
    }

private:
    std::unordered_map<uint32_t, std::shared_ptr<Properties>> m_entityProperties;
};

std::shared_ptr<IEntityProperties> IEntityProperties::getInstance()
{
    static std::weak_ptr<IEntityProperties> g_instance;

    if (auto p = g_instance.lock())
    {
        return p;
    }

    // Create a new one
    auto p = std::shared_ptr<IEntityProperties>(new EntityProperties());
    g_instance = p;

    return p;
}