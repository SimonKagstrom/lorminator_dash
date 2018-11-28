#include <level-animator.hh>
#include <lightning.hh>

class LevelAnimator : public ILevelAnimator
{
public:
    LevelAnimator(std::shared_ptr<ILightning> lightning) : 
        m_lightning(lightning)
    {
    }

    virtual void animate(unsigned round) override
    {
    }

    virtual ImageEntry getImageEntryAt(const point &where) const override
    {
        auto tileType = TileType::UNKNOWN;

        auto tile = m_lightning->tileAt(where);
        if (tile)
        {
            tileType = *tile;
        }

        return {Image::TILES, tileToFrame(tileType)};
    }

private:
    unsigned tileToFrame(TileType tile) const
    {
        const std::unordered_map<TileType, unsigned> transform =
        {
            {TileType::UNKNOWN, 0},
            {TileType::EMPTY, 1},
            {TileType::DIRT, 2},
            {TileType::MAGIC_WALL, 4},
            {TileType::LEFT_TRANSPORT, 5},
            {TileType::RIGHT_TRANSPORT, 6},
            {TileType::STONE_WALL, 13},
            {TileType::WEAK_STONE_WALL, 14},
            {TileType::TELEPORTER, 15},
            {TileType::CONVEYOR, 21},
            {TileType::EXIT, 15},
        };

        auto it = transform.find(tile);
    
        return it->second;
    }

    std::shared_ptr<ILightning> m_lightning;
};

std::unique_ptr<ILevelAnimator> ILevelAnimator::fromLightning(std::shared_ptr<ILightning> lightning)
{
    return std::make_unique<LevelAnimator>(lightning);
}
