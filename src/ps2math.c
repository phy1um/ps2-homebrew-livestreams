#include <math.h>

#include "log.h"
#include "ps2math.h"

float p2m_vec2_length(float *v2) {
  float xs = v2[0] * v2[0];
  float ys = v2[1] * v2[1];
  return sqrtf(xs + ys);
}

float p2m_vec2_dot(float *a, float *b) { return (a[0] * b[0]) + (a[1] * b[1]); }

void p2m_vec2_rotate(float *v, float theta) {
  float x = v[0];
  float y = v[1];
  v[0] = x * cosf(theta) - y * sinf(theta);
  v[1] = x * sinf(theta) + y * cosf(theta);
}

void p2m_vec2_scale(float *v, float s) {
  v[0] *= s;
  v[1] *= s;
}

float p2m_vec3_length(float *v3) {
  float xs = v3[0] * v3[0];
  float ys = v3[1] * v3[1];
  float zs = v3[2] * v3[2];
  return sqrtf(xs + ys + zs);
}

float p2m_vec3_dot(float *a, float *b) {
  return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

void p2m_vec3_scale(float *v, float s) {
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
}

void p2m_m3_copy(float *a, float *b) {
  for (int i = 0; i < 9; i++) {
    a[i] = b[i];
  }
}

void p2m_m3_apply(float *m3, float *v) {
  float nx = m3[0] * v[0] + m3[1] * v[1] + m3[2] * v[2];
  float ny = m3[3] * v[0] + m3[4] * v[1] + m3[5] * v[2];
  float nz = m3[6] * v[0] + m3[7] * v[1] + m3[8] * v[2];
  v[0] = nx;
  v[1] = ny;
  v[2] = nz;
}

void p2m_m3_add(float *a, float *b) {
  for (int i = 0; i < 9; i++) {
    a[i] += b[i];
  }
}

void p2m_m3_identity(float *m) {
  m[0] = 1;
  m[1] = 0;
  m[2] = 0;
  m[3] = 0;
  m[4] = 1;
  m[5] = 0;
  m[6] = 0;
  m[7] = 0;
  m[8] = 1;
}

void p2m_m3_multiply(float *a, float *b, float *to) {
  float a00 = a[0];
  float a10 = a[1];
  float a20 = a[2];
  float a01 = a[3];
  float a11 = a[4];
  float a21 = a[5];
  float a02 = a[6];
  float a12 = a[7];
  float a22 = a[8];
  float b00 = b[0];
  float b10 = b[1];
  float b20 = b[2];
  float b01 = b[3];
  float b11 = b[4];
  float b21 = b[5];
  float b02 = b[6];
  float b12 = b[7];
  float b22 = b[8];

  to[0] = a00 * b00 + a10 * b01 + a20 * b02;
  to[3] = a01 * b00 + a11 * b01 + a21 * b02;
  to[6] = a02 * b00 + a12 * b01 + a22 * b02;
  to[1] = a00 * b10 + a10 * b11 + a20 * b12;
  to[4] = a01 * b10 + a11 * b11 + a21 * b12;
  to[7] = a02 * b10 + a12 * b11 + a22 * b12;
  to[2] = a00 * b20 + a10 * b21 + a20 * b22;
  to[5] = a01 * b20 + a11 * b21 + a21 * b22;
  to[8] = a02 * b20 + a12 * b21 + a22 * b22;
}
