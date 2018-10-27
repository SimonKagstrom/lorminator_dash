#pragma once

#include <memory>
#include <functional>
#include <unordered_map>

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

// How I'd like it to work
template<typename FN>
class NotifierContainer : public NotifierBase
{
    std::unique_ptr<ObserverCookie> listen(FN &cb)
    {
        auto id = (uint32_t)m_listeners.size();

        m_listeners[id] = cb;

        return std::make_unique<ObserverCookie>(id, shared_from_this());
    }

protected:
    void detach(uint32_t id) override
    {
        m_listeners.erase(id);
    }

    std::unordered_map<uint32_t, FN> m_listeners;
};

class Notifier0 : public NotifierBase
{
public:
    void invoke()
    {
        for (auto &kv : m_listeners)
        {
            kv.second();
        }
    }

    std::unique_ptr<ObserverCookie> listen(std::function<void()> cb)
    {
        auto id = (uint32_t)m_listeners.size();

        m_listeners[id] = cb;

        return std::make_unique<ObserverCookie>(id, shared_from_this());
    }

protected:
    void detach(uint32_t id) override
    {
        m_listeners.erase(id);
    }

    std::unordered_map<uint32_t, std::function<void()>> m_listeners;
};

template<typename A0>
class Notifier1 : public NotifierBase
{
public:
    void invoke(A0 arg0)
    {
        for (auto &kv : m_listeners)
        {
            kv.second(arg0);
        }
    }

    std::unique_ptr<ObserverCookie> listen(std::function<void(A0)> cb)
    {
        auto id = (uint32_t)m_listeners.size();

        m_listeners[id] = cb;

        return std::make_unique<ObserverCookie>(id, shared_from_this());
    }

protected:
    void detach(uint32_t id) override
    {
        m_listeners.erase(id);
    }

    std::unordered_map<uint32_t, std::function<void(A0)>> m_listeners;
};

template<typename A0, typename A1>
class Notifier2 : public NotifierBase
{
public:
    void invoke(A0 arg0, A1 arg1)
    {
        for (auto &kv : m_listeners)
        {
            kv.second(arg0, arg1);
        }
    }

    std::unique_ptr<ObserverCookie> listen(std::function<void(A0, A1)> cb)
    {
        auto id = (uint32_t)m_listeners.size();

        m_listeners[id] = cb;

        return std::make_unique<ObserverCookie>(id, shared_from_this());
    }

protected:
    void detach(uint32_t id) override
    {
        m_listeners.erase(id);
    }

    std::unordered_map<uint32_t, std::function<void(A0, A1)>> m_listeners;
};
