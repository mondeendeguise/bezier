#ifndef VECTOR_H_
#define VECTOR_H_

typedef struct {
    float c[2];
} V2f;

typedef struct {
    float c[4];
} V4f;

V2f v2f(float x, float y);
V4f v4f(float x, float y, float z, float w);

V2f v2f_add(V2f a, V2f b);
V2f v2f_scale(V2f a, float x);
V2f v2f_multiply(V2f a, V2f b);

V2f v2f_lerp(V2f a, V2f b, float t);

V2f v2f_spline(V2f p0, V2f p1, V2f p2, V2f p3, float t);

#endif // VECTOR_H_
