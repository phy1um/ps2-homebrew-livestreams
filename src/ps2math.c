#include <math.h>

#include <p2g/log.h>
#include <p2g/ps2math.h>

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

void p2m_m4_identity(float *m) {
  for(int i = 1; i < 16; i++) m[i] = 0;
  m[0] = 1;
  m[5] = 1;
  m[10] = 1;
  m[15] = 1;
}

void p2m_m4_apply(float *m, float *v) {
  float nx = m[0]*v[0]  + m[1]*v[1]  + m[2]*v[2]  + m[3]*v[3];
  float ny = m[4]*v[0]  + m[5]*v[1]  + m[6]*v[2]  + m[7]*v[3];
  float nz = m[8]*v[0]  + m[9]*v[1]  + m[10]*v[2] + m[11]*v[3];
  float nw = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
  v[0] = nx;
  v[1] = ny;
  v[2] = nz;
  v[3] = nw;
}

void p2m_m4_copy(float *a, float *b) {
  for (int i = 0; i < 16; i++) {
    a[i] = b[i];
  }
}


void p2m_m4_multiply(float *a, float *b, float *to) {
  float a00 = a[0];
  float a10 = a[1];
  float a20 = a[2];
  float a30 = a[3];
  float a01 = a[4];
  float a11 = a[5];
  float a21 = a[6];
  float a31 = a[7];
  float a02 = a[8];
  float a12 = a[9];
  float a22 = a[10];
  float a32 = a[11];  
  float a03 = a[12];
  float a13 = a[13];
  float a23 = a[14];
  float a33 = a[15];

  float b00 = b[0];
  float b10 = b[1];
  float b20 = b[2];
  float b30 = b[3];
  float b01 = b[4];
  float b11 = b[5];
  float b21 = b[6];
  float b31 = b[7];
  float b02 = b[8];
  float b12 = b[9];
  float b22 = b[10];
  float b32 = b[11];  
  float b03 = b[12];
  float b13 = b[13];
  float b23 = b[14];
  float b33 = b[15];

  to[0] = a00*b00 + a10*b01 + a20*b02 + a30*b03;
  to[1] = a00*b10 + a10*b11 + a20*b12 + a30*b13;
  to[2] = a00*b20 + a10*b21 + a20*b22 + a30*b23;
  to[3] = a00*b30 + a10*b31 + a20*b32 + a30*b33;

  to[4] = a01*b00 + a11*b01 + a21*b02 + a31*b03;
  to[5] = a01*b10 + a11*b11 + a21*b12 + a31*b13;
  to[6] = a01*b20 + a11*b21 + a21*b22 + a31*b23;
  to[7] = a01*b30 + a11*b31 + a21*b32 + a31*b33;

  to[8] = a02*b00 + a12*b01 + a22*b02 + a32*b03;
  to[9] = a02*b10 + a12*b11 + a22*b12 + a32*b13;
  to[10]= a02*b20 + a12*b21 + a22*b22 + a32*b23;
  to[11]= a02*b30 + a12*b31 + a22*b32 + a32*b33;

  to[12]= a03*b00 + a13*b01 + a23*b02 + a33*b03;
  to[13]= a03*b10 + a13*b11 + a23*b12 + a33*b13;
  to[14]= a03*b20 + a13*b21 + a23*b22 + a33*b23;
  to[15]= a03*b30 + a13*b31 + a23*b32 + a33*b33;
}
