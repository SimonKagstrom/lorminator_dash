#include <observer.hh>

ObserverCookie::~ObserverCookie()
{
    auto p = m_parent.lock();

    if (p)
    {
        p->detach(m_id);
    }
}
