//
// Created by ashcon on 3/14/21.
//

#ifndef NAG52_SIMULATOR_HELPER_H_
#define NAG52_SIMULATOR_HELPER_H_


#ifdef SIM_MODE
#include <cstdio>
#define LOG_MSG(x) printf(x)
#else
#define LOG_MSG(x) Serial.printf(x)
#endif


/**
 * Clamps input value between 2 bounds
 * @param x  Input value
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return
 */
int clamp(int x, int min, int max) {
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    } else {
        return x;
    }
}

/**
 * Converts a number with a known upper and lower bound to a new range.
 *
 * Example:
 *      convert_range(50, 0, 100, 0, 50); // 25
 *      convert_range(10, 0, 10, 10, 100) // 100
 *
 * NOTE: If v is outside the bounds o1 or o2, it is clamped to be be o1 or o2.
 *
 * @param v  Number to convert.
 * @param o1 Old range min bound
 * @param o2 Old range max bound
 * @param n1 New range min bound
 * @param n2 New range max bound
 * @return Converted value
 */
int convert_range(int v, int o1, int o2, int n1, int n2) {
    return (((clamp(v, o1, o2) - o1) * (n2 - n1)) / (o2 - o1)) + n1;
}

#endif //NAG52_SIMULATOR_HELPER_H
