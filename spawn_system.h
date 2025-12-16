#pragma once
#include "components.h"

struct SpawnSystem{
    SpawnSystem()
    :m_spawn_interval(2.5f)
    ,m_spawn_size(1)
    ,m_spawn_timeout(2.5f)
    ,m_max_enemies(100)
    {}
    void update(Entities& objects, const Level& level, float deltatime);

    float  m_spawn_interval;
    int    m_spawn_size;
    float  m_spawn_timeout;
    int    m_max_enemies;

};