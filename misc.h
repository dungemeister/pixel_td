#pragma once
#include <stdlib.h>

static float RandomFloat01() { return (float)rand() / (float)RAND_MAX; }
static float RandomFloat(float min, float max) { return RandomFloat01() * (max - min) + min; }

static int   RandomInt  (int min, int max) { return min + rand() % (max - min + 1); }