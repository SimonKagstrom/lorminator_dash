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
class Operate : public PlayerTraitBase
{
public:
    Operate(std::shared_ptr<ILevel> level, std::shared_ptr<IEntity> entity) : PlayerTraitBase(level, entity)
    {
    }

    bool run(unsigned ms) override
    {
        auto keys = m_input->getInput();

        if ((keys & InputTypes::OPERATE) == 0)
        {
            // Not operating
            return false;
        }
        if ((keys & (InputTypes::UP | InputTypes::DOWN | InputTypes::LEFT | InputTypes::RIGHT)) == 0)
        {
            // Not operating in a direction
            return false;
        }


        auto dir = keysToDir(keys);
        auto dst = m_entity->getPosition() + dir;

        auto tileAtDst = m_level->tileAt(dst);
        auto entAtDst = IEntityStore::getInstance()->getEntityByPoint(dst); 

        if (!tileAtDst)
        {
            return false;
        }
        if (*tileAtDst == TileType::DIRT)
        {
            m_level->setTile(dst, TileType::EMPTY);
        }
        if (entAtDst)
        {
            if (entAtDst->getType() == EntityType::DIAMOND)
            {
                entAtDst->remove();
                m_props->set("diamonds", m_props->asInt("diamonds") + 1);
            }
        }

        return true;
    }
};
}
