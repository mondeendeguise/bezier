#include "vector.h"

#include <stddef.h>

inline V2f v2f(float x, float y)
{
    V2f v = {0};
    v.c[0] = x;
    v.c[1] = y;
    return v;
}

inline V4f v4f(float x, float y, float z, float w)
{
    V4f v = {0};
    v.c[0] = x;
    v.c[1] = y;
    v.c[2] = z;
    v.c[3] = w;
    return v;
}

V2f v2f_add(V2f a, V2f b)
{
    for(size_t i = 0; i < 2; ++i) {
        a.c[i] += b.c[i];
    }
    return a;
}

V2f v2f_scale(V2f a, float x)
{
    for(size_t i = 0; i < 2; ++i) {
        a.c[i] *= x;
    }
    return a;
}

V2f v2f_multiply(V2f a, V2f b)
{
    for(size_t i = 0; i < 2; ++i) {
        a.c[i] *= b.c[i];
    }
    return a;
}

V2f v2f_lerp(V2f a, V2f b, float t)
{
    return v2f_add(v2f_scale(a, (1.0f - t)), v2f_scale(b, t));
}

V2f v2f_spline(V2f p0, V2f p1, V2f p2, V2f p3, float t)
{
    V2f A = v2f_lerp(p0, p1, t);
    V2f B = v2f_lerp(p1, p2, t);
    V2f C = v2f_lerp(p2, p3, t);
    V2f D = v2f_lerp(A, B, t);
    V2f E = v2f_lerp(B, C, t);
    V2f P = v2f_lerp(D, E, t);
    return P;
}
