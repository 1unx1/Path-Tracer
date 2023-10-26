#ifndef DEFINE_H
#define DEFINE_H
#include <random>
const float INF = 1e100;

const int NEWTON_STEPS = 15;
const float NEWTON_EPSILON = 1e-4;
const float DELTA = 1e-16;

static std::mt19937 mersenneTwister;
static std::uniform_real_distribution<float> uniform;
#define RND1 (2.0 * uniform(mersenneTwister) - 1.0) // [-1, 1]
#define RND2 (uniform(mersenneTwister))             // [0, 1]
#endif