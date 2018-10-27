#include <catch.hpp>
#include <trompeloeil.hpp>

#include <observer.hh>

TEST_CASE("The notifier can be created")
{
    auto n = std::make_shared<Notifier0>();

    SECTION("no listeners attached - nothing funny happens")
    {
        // Nothing listens
        n->invoke();
    }

    SECTION("a listener can be attached")
    {
        int a = 0;
        auto cookie = n->listen([&a]{a++;});

        REQUIRE(a == 0U);
        n->invoke();
        REQUIRE(a == 1U);
        n->invoke();
        REQUIRE(a == 2U);

        cookie.reset();
        n->invoke();
        REQUIRE(a == 2U);
    }

    SECTION("multiple listeners can listen")
    {
        int a = 0;
        int b = 0;

        auto ca = n->listen([&a]{a++;});

        n->invoke();

        auto cb = n->listen([&b]{b++;});

        n->invoke();

        REQUIRE(a == 2U);
        REQUIRE(b == 1U);

        ca.reset();

        n->invoke();

        REQUIRE(a == 2U);
        REQUIRE(b == 2U);

        cb.reset();
    }

    SECTION("the notifier can be removed with listeners present")
    {
        int a = 0;
        auto cookie = n->listen([&a]{a++;});

        REQUIRE(a == 0U);
        n->invoke();
        REQUIRE(a == 1U);
        n.reset();

        cookie.reset();

        REQUIRE(a == 1U);
    }
}


TEST_CASE("argument notifiers")
{
    auto n = std::make_shared<Notifier1<int>>();

    SECTION("a listener can be attached")
    {
        int a = 0;
        auto cookie = n->listen([&a](int arg){ REQUIRE(arg == 35); a++;});

        REQUIRE(a == 0U);
        n->invoke(35);
        REQUIRE(a == 1U);
        n->invoke(35);
        REQUIRE(a == 2U);

        cookie.reset();
        n->invoke(35);
        REQUIRE(a == 2U);
    }
}
