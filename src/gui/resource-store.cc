#include <resource-store.hh>
#include <io.hh>

#include <optional>
#include <fstream>
#include <SDL.h>


class ResourceStore : public IResourceStore
{
public:
    ResourceStore(const std::vector<std::string> &dirsToSearch) :
        m_dirs(dirsToSearch)
    {
    }
    
    void addImage(Image image, const std::string &filename) override
    {
        auto resolved = resolveFilename(filename);

        if (!resolved)
        {
            throw std::invalid_argument("Can't locate " + filename);
        }

        auto img = SDL_LoadBMP(resolved->c_str());
        if (!img)
        {
            throw std::invalid_argument("Can't load " + filename + " as an image");
        }

        auto size = determineFrameExtents(img);
        if (size != m_frameExtents)
        {
            throw std::invalid_argument("Extents mismatch for " + filename);
        }

        auto renderer = (SDL_Renderer *)IIo::getInstance()->getRenderer();
    }

    virtual void *getImageFrame(const ImageEntry &entry) override
    {
        return nullptr;
    }

    static extents m_frameExtents;

private:
    extents determineFrameExtents(const SDL_Surface *image)
    {
        const std::vector<unsigned> sizes = {256, 128, 96, 64, 32};

        for (auto sz : sizes)
        {
            if (image->w % sz == 0 && image->h % sz == 0)
            {
                return (extents){sz, sz};
            }
        }

        std::invalid_argument("Extents can't be determined");
        return (extents){0,0};
    }

    std::optional<std::string> resolveFilename(const std::string &filename) const
    {
        for (auto &dir : m_dirs)
        {
            auto cur = dir + "/" + filename;

            std::ifstream ifs(cur);
            if (ifs.is_open())
            {
                return cur;
            }
        }

        return nullptr;
    }

    std::vector<std::string> m_dirs;
};
extents ResourceStore::m_frameExtents;

extents IResourceStore::getFrameExtents()
{
    if (ResourceStore::m_frameExtents == (extents){0,0})
    {
        throw std::invalid_argument("frame extens not set, yet");
    }

    return ResourceStore::m_frameExtents;
}

std::unique_ptr<IResourceStore> IResourceStore::create(const std::vector<std::string> &dirsToSearch)
{
    return std::make_unique<ResourceStore>(dirsToSearch);
}
