#include <lightning.hh>

class Lightning : public ILightning
{
public:
    Lightning(std::shared_ptr<ILevel> level) :
        m_size(level->getSize()),
        m_level(level),
        m_store(IEntityStore::getInstance())
    {
        m_tiles.resize(m_size.width * m_size.height);
        std::fill(m_tiles.begin(), m_tiles.end(), TileType::UNKNOWN);
    }

    void updateLightning(const std::set<point> &lighted) override
    {
        if (m_hideUnknown)
        {
            updateLightningHideUnknown(lighted);
        }
        else
        {
            updateLightningShowUnknown(lighted);
        }
    }

    void updateLightningShowUnknown(const std::set<point> &lighted)
    {
        m_lighted = lighted;

        m_visibleEntities.clear();
        m_shadowEntities.clear();
        for (auto ent : m_store->getEntities())
        {
            auto pos = ent->getPosition();
            if (lighted.find(pos) == lighted.end())
            {
                m_shadowEntities[pos] = ent->getType();
            }
            else
            {
                m_visibleEntities.push_back(ent->getId());
            }
        }

        // Update visible tiles and entities
        for (auto pt : lighted)
        {
            m_shadowEntities.erase(pt);

            auto tile = m_level->tileAt(pt);
            if (tile)
            {
                m_tiles[pt.y * m_size.width + pt.x] = *tile;
            }
        }
    }

    void updateLightningHideUnknown(const std::set<point> &lighted)
    {
        m_lighted = lighted;

        for (auto entId : m_visibleEntities)
        {
            auto ent = m_store->getEntityById(entId);
            if (!ent)
            {
                continue;
            }
         
            auto pos = ent->getPosition();

            if (lighted.find(pos) == lighted.end())
            {
                m_shadowEntities[pos] = ent->getType();
            }
        }

        m_visibleEntities.clear();

        // Update visible tiles and entities
        for (auto pt : lighted)
        {
            m_shadowEntities.erase(pt);

            auto tile = m_level->tileAt(pt);
            if (tile)
            {
                m_tiles[pt.y * m_size.width + pt.x] = *tile;
            }

            auto ent = m_store->getEntityByPoint(pt);
            if (ent)
            {
                // The entity is visible, add to the visible set
                m_visibleEntities.push_back(ent->getId());
            }
        }
    }

    const std::set<point> getLighted() const override
    {
        return m_lighted;
    }

    std::optional<TileType> tileAt(const point &where) const override
    {
        // Bounds check
        if (!m_level->tileAt(where))
        {
            return std::optional<TileType>();
        }
        if (m_hideUnknown)
        {
            return m_tiles[where.y * m_size.width + where.x];
        }

        return m_level->tileAt(where);
    }

    const std::vector<uint32_t> &getVisibleEntities() override
    {
        return m_visibleEntities;
    }

    std::vector<ShadowEntity> getShadowEntities()  override
    {
        std::vector<ShadowEntity> out;

        for (auto &[pt, type] : m_shadowEntities)
        {
            out.push_back({pt, type});
        }

        return out;
    }


    void setUnknownBehavior(const ILightning::UnknownBehavior &what)
    {
        if (what == ILightning::UnknownBehavior::HIDE)
        {
            m_hideUnknown = true;
        }
        else
        {
            m_hideUnknown = false;
        }
    }

private:
    const extents m_size;
    std::shared_ptr<ILevel> m_level;
    std::vector<TileType> m_tiles;
    std::vector<uint32_t> m_visibleEntities;
    std::unordered_map<point, EntityType> m_shadowEntities;
    std::set<point> m_lighted;

    std::shared_ptr<IEntityStore> m_store;
    bool m_hideUnknown{true};
};

std::unique_ptr<ILightning> ILightning::create(std::shared_ptr<ILevel> level)
{
    return std::unique_ptr<ILightning>(new Lightning(level));
}