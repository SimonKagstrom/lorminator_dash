#include <behavior.hh>
#include <point.hh>
#include <entity.hh>
#include <level.hh>

#include "itrait.hh"

#include <memory>
#include <vector>
#include <exception>

#include <input.hh>

#include "player-traits.hh"

namespace player
{
class Walk : public PlayerTraitBase
{
public:
    Walk(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) : PlayerTraitBase(level, entity)
    {
    }

    bool run(unsigned ms) override
    {
        auto keys = m_input->getInput();

        if (keys == 0 || (keys & InputTypes::OPERATE))
        {
            // Not walking
            return false;
        }

        auto store = IEntityStore::getInstance();

        auto dir = keysToDir(keys);
        auto dst = m_entity->getPosition() + dir;

        m_entity->setDirection(dir);

        auto tileAtDst = m_level->tileAt(dst);
        auto entAtDst = store->getEntityByPoint(dst);

        if (!tileAtDst)
        {
            // out of bounds
            return false;
        }

        if (!ILevel::tileIsPassable(*tileAtDst))
        {
            return false;
        }

        if (entAtDst)
        {
            auto type = entAtDst->getType();

            if (type == EntityType::DIAMOND)
            {
                // Remove and increase diamond count
                entAtDst->remove();
                m_props->set("diamonds", m_props->asInt("diamonds") + 1);
            }
            else if (type == EntityType::BOULDER)
            {
                if (dir == Direction::UP || dir == Direction::DOWN)
                {
                    // Can't push vertically
                    return false;
                }

                auto afterBoulder = dst + dir;
                auto tileAfterBoulder = m_level->tileAt(afterBoulder);

                if (!tileAfterBoulder)
                {
                    // Out of bounds
                    return false;
                }
                auto entityAfterBoulder = store->getEntityByPoint(afterBoulder);

                if (*tileAfterBoulder == TileType::EMPTY && !entityAfterBoulder)
                {
                    // push!
                    entAtDst->setPosition(afterBoulder);
                }
                else
                {
                    return false;
                }
            }
        }

        // Move the player
        if (*tileAtDst == TileType::DIRT)
        {
            m_level->setTile(dst, TileType::EMPTY);
        }
        m_entity->setPosition(dst);

        return true;
    }
};
}
