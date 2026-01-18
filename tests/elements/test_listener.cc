#include <doctest/doctest.h>
#include "elements/listener.hh"

TEST_CASE("Listener: default construction")
{
    nelement::Listener listener;

    CHECK(listener.get_position() == glm::vec3(0.0f, 0.0f, 0.0f));
}

TEST_CASE("Listener: self_update changes position")
{
    nelement::Listener listener;

    glm::vec3 newPos(5.0f, 3.0f, -2.0f);
    listener.self_update(newPos);

    CHECK(listener.get_position() == newPos);
}

TEST_CASE("Listener: self_update multiple times")
{
    nelement::Listener listener;

    listener.self_update(glm::vec3(1.0f, 0.0f, 0.0f));
    CHECK(listener.get_position() == glm::vec3(1.0f, 0.0f, 0.0f));

    listener.self_update(glm::vec3(0.0f, 2.0f, 0.0f));
    CHECK(listener.get_position() == glm::vec3(0.0f, 2.0f, 0.0f));

    listener.self_update(glm::vec3(0.0f, 0.0f, 3.0f));
    CHECK(listener.get_position() == glm::vec3(0.0f, 0.0f, 3.0f));
}

TEST_CASE("Listener: init creates audio context")
{
    nelement::Listener listener;

    // Before init, update should not crash (no context)
    listener.self_update(glm::vec3(1.0f, 2.0f, 3.0f));
    CHECK(listener.get_position() == glm::vec3(1.0f, 2.0f, 3.0f));

    // After init, audio context should be available
    listener.init();
    listener.self_update(glm::vec3(4.0f, 5.0f, 6.0f));
    CHECK(listener.get_position() == glm::vec3(4.0f, 5.0f, 6.0f));
}

TEST_CASE("Listener: extreme positions")
{
    nelement::Listener listener;

    // Very large position
    listener.self_update(glm::vec3(10000.0f, 10000.0f, 10000.0f));
    CHECK(listener.get_position() == glm::vec3(10000.0f, 10000.0f, 10000.0f));

    // Negative position
    listener.self_update(glm::vec3(-500.0f, -500.0f, -500.0f));
    CHECK(listener.get_position() == glm::vec3(-500.0f, -500.0f, -500.0f));

    // Zero position
    listener.self_update(glm::vec3(0.0f, 0.0f, 0.0f));
    CHECK(listener.get_position() == glm::vec3(0.0f, 0.0f, 0.0f));
}
