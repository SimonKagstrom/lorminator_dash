#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

class NotifierBase;

class ObserverCookie
{
public:
    ObserverCookie(uint32_t id, std::weak_ptr<NotifierBase> notifier) :
        m_id(id), m_parent(notifier)
    {
    }

    virtual ~ObserverCookie();

protected:
    uint32_t m_id;
    std::weak_ptr<NotifierBase> m_parent;
};

class NotifierBase : public std::enable_shared_from_this<NotifierBase>
{
public:
    friend class ObserverCookie;

    virtual ~NotifierBase()
    {
    }

protected:
    virtual void detach(uint32_t id) = 0;
};

template<typename FN>
class NotifierContainer : public NotifierBase
{
public:
    std::unique_ptr<ObserverCookie> listen(FN &cb)
    {
        auto id = (uint32_t)m_listeners.size();

        m_listeners[id] = cb;

        return std::make_unique<ObserverCookie>(id, shared_from_this());
    }

    void detach(uint32_t id) override
    {
        if (m_invoking)
        {
            // We can't remove while iterating, so put in a list for later removal
            m_entriesToRemove.push_back(id);
        }
        else
        {
            m_listeners.erase(id);
        }
    }

    bool m_invoking{false};
    std::unordered_map<uint32_t, FN> m_listeners;
    std::vector<uint32_t> m_entriesToRemove;
};

template<typename FN>
class NotifierBasex
{
public:
    std::unique_ptr<ObserverCookie> listen(FN cb)
    {
        return m_impl->listen(cb);
    }

protected:
    NotifierBasex() : m_impl(std::make_shared<NotifierContainer<FN>>())
    {
    }

    void doInvoke(std::function<void()> wrapped)
    {
        m_impl->m_invoking = true;
        wrapped();
        m_impl->m_invoking = false;
        for (auto it : m_impl->m_entriesToRemove)
        {
            m_impl->detach(it);
        }
        m_impl->m_entriesToRemove.clear();
    }

    std::shared_ptr<NotifierContainer<FN>> m_impl;
};

class Notifier0 : public NotifierBasex<std::function<void()>>
{
public:
    void operator()()
    {
        invoke();
    }

    void invoke()
    {
        for (auto &kv : m_impl->m_listeners)
        {
            kv.second();
        }
    }
};

template<typename A0>
class Notifier1 : public NotifierBasex<std::function<void(A0)>>
{
public:
    void operator()(A0 arg0)
    {
        invoke(arg0);
    }

    void invoke(A0 arg0)
    {
        NotifierBasex<std::function<void(A0)>>::doInvoke([this, arg0]()
        {
            for (auto &kv : NotifierBasex<std::function<void(A0)>>::m_impl->m_listeners)
            {
                kv.second(arg0);
            }
        });
    }
};

template<typename A0, typename A1>
class Notifier2 : public NotifierBasex<std::function<void(A0, A1)>>
{
public:
    void operator()(A0 arg0, A1 arg1)
    {
        invoke(arg0, arg1);
    }

    void invoke(A0 arg0, A1 arg1)
    {
        NotifierBasex<std::function<void(A0, A1)>>::doInvoke([this, arg0, arg1]()
        {
            for (auto &kv : NotifierBasex<std::function<void(A0, A1)>>::m_impl->m_listeners)
            {
                kv.second(arg0, arg1);
            }
        });
    }
};

template<typename A0, typename A1, typename A2>
class Notifier3 : public NotifierBasex<std::function<void(A0, A1, A2)>>
{
public:
    void operator()(A0 arg0, A1 arg1, A2 arg2)
    {
        invoke(arg0, arg1, arg2);
    }

    void invoke(A0 arg0, A1 arg1, A2 arg2)
    {
        NotifierBasex<std::function<void(A0, A1, A2)>>::doInvoke([this, arg0, arg1, arg2]()
        {
            for (auto &kv : NotifierBasex<std::function<void(A0, A1, A2)>>::m_impl->m_listeners)
            {
                kv.second(arg0, arg1, arg2);
            }
        });
    }
};
