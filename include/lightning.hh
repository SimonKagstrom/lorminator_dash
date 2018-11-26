#pragma once

#include <point.hh>
#include <entity.hh>
#include <level.hh>

#include <optional>
#include <memory>
#include <vector>

class ILightning
{
public:
    struct ShadowEntity
    {
        point pt;
        EntityType type;
    };

    virtual ~ILightning()
    {
    }

    virtual void updateLightning(const std::set<point> &lighted) = 0;

    virtual std::optional<TileType> tileAt(const point &where) const = 0;

    /// Return the entity ID:s which are visible
    virtual const std::vector<uint32_t> &getVisibleEntities() = 0;
    virtual std::vector<ShadowEntity> getShadowEntities() = 0;

    static std::unique_ptr<ILightning> create(std::shared_ptr<ILevel> level);
};
