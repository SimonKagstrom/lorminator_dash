#include <resource-store.hh>
#include <io.hh>

#include <SDL.h>
#include <SDL_image.h>


class ResourceStore : public IResourceStore
{
public:
    ResourceStore()
    {
    }

    ~ResourceStore()
    {
        for (auto &[key, value] : m_framesByImageEntry)
        {
            SDL_FreeSurface(value);
        }
    }
    
    void addImage(Image image, const std::string &filename) override
    {
        auto img = IMG_Load(filename.c_str());
        if (!img)
        {
            throw std::invalid_argument("Can't load " + filename + " as an image");
        }

        auto size = determineFrameExtents(img);

        if (m_frameExtents != (extents){0,0} && size != m_frameExtents)
        {
            throw std::invalid_argument("Extents mismatch for " + filename);
        }
        m_frameExtents = size;

        auto fmt = img->format;

        unsigned frameCount = (img->w * img->h) / (size.width * size.height);
        m_frameCountByImage[image] = frameCount;

        for (unsigned frame = 0; frame < frameCount; frame++)
        {
            auto surface = SDL_CreateRGBSurface(0, size.width, size.height,
                fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

            if (!surface)
            {
                throw std::invalid_argument("Can't create new surface for frame");
            }

            int x = (frame * size.width) % img->w;
            int y = (frame * size.height) % img->h;
            SDL_Rect srcRect = {x, y, (int)size.width, (int)size.height};
            SDL_Rect dstRect = {0, 0, (int)size.width, (int)size.height};

            SDL_BlitSurface(img, &srcRect, surface, &dstRect);

            m_framesByImageEntry[(ImageEntry){image, frame}] = surface;
        }
    }

    virtual unsigned getImageFrameCount(Image image) const override
    {
        auto it = m_frameCountByImage.find(image);

        if (it == m_frameCountByImage.end())
        {
            throw std::invalid_argument("No frame count for image");
        }

        return it->second;
    }

    virtual void *getImageFrame(const ImageEntry &entry) override
    {
        auto it = m_framesByImageEntry.find(entry);

        if (it == m_framesByImageEntry.end())
        {
            return nullptr;
        }

        return (void *)it->second;
    }

    virtual extents getFrameExtents() const override
    {
        if (ResourceStore::m_frameExtents == (extents){0,0})
        {
            throw std::invalid_argument("frame extens not set, yet");
        }

        return m_frameExtents;
    }

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

    std::vector<std::string> m_dirs;
    std::unordered_map<ImageEntry, SDL_Surface *> m_framesByImageEntry;
    std::unordered_map<Image, unsigned> m_frameCountByImage;
    extents m_frameExtents{0,0};
};

std::shared_ptr<IResourceStore> IResourceStore::getInstance()
{
    static std::weak_ptr<IResourceStore> g_instance;

    if (auto p = g_instance.lock())
    {
        return p;
    }

    // Create a new one
    auto p = std::shared_ptr<IResourceStore>(new ResourceStore());
    g_instance = p;

    return p;
}
