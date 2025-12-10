#pragma once
#include "components.h"

struct SpawnSystem{
    SpawnSystem()
    :m_spawn_interval(5.f)
    ,m_spawn_size(4)
    ,m_spawn_timeout(2.5f)
    ,m_max_enemies(100)
    {}
    void update(Entities& objects, const Level& level, float deltatime);

    float  m_spawn_interval;
    size_t m_spawn_size;
    float  m_spawn_timeout;
    size_t m_max_enemies;

};