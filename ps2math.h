#ifndef PS2_MATH_H
#define PS2_MATH_H

#include <math3d.h>

void matrix_lookat(MATRIX out, VECTOR eye, VECTOR c, VECTOR up);

void matrix_viewport(MATRIX out, int w, int h);
void matrix_proj(MATRIX out, float fov, float ar, float near, float far);

void matrix_tsp(MATRIX t);

#endif
