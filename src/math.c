#include <math.h>
#include <math3d.h>

#define SWAP(i, j)                                                             \
  do {                                                                         \
    t = a[i];                                                                  \
    a[i] = a[j];                                                               \
    a[j] = t;                                                                  \
  } while (0)
void matrix_tsp(MATRIX a) {
  float t;
  SWAP(1, 4);
  SWAP(2, 8);
  SWAP(3, 12);
  SWAP(6, 9);
  SWAP(7, 13);
  SWAP(11, 14);
}

void vector_cross(VECTOR out, VECTOR a, VECTOR b) {
  out[0] = a[1] * b[2] - a[2] * b[1];
  out[1] = a[2] * b[0] - a[0] * b[2];
  out[2] = a[0] * b[1] - a[1] * b[0];
}

void vector_sub(VECTOR out, VECTOR a, VECTOR b) {
  out[0] = a[0] - b[0];
  out[1] = a[1] - b[1];
  out[2] = a[2] - b[2];
  // out[3] = a[3] - b[3];
}

void matrix_lookat(MATRIX out, VECTOR eye, VECTOR c, VECTOR up) {
  VECTOR z, x, y, nd;
  vector_sub(z, c, eye);
  vector_normalize(z, z);

  vector_cross(x, up, z);
  vector_normalize(x, x);

  vector_cross(y, z, x);
  vector_normalize(y, y);

  matrix_unit(out);

  nd[0] = -1 * (x[0] * eye[0] + x[1] * eye[1] + x[2] * eye[2]);
  nd[1] = -1 * (y[0] * eye[0] + y[1] * eye[1] + y[2] * eye[2]);
  nd[2] = -1 * (z[0] * eye[0] + z[1] * eye[1] + z[2] * eye[2]);

  for (int i = 0; i < 3; i++) {
    out[4 * i] = x[i];
    out[4 * i + 1] = y[i];
    out[4 * i + 2] = z[i];
    out[4 * i + 3] = nd[i];
  }

  matrix_tsp(out);
}

void matrix_zero(MATRIX out) {
  for (int i = 0; i < 16; i++) {
    out[i] = 0;
  }
}

void matrix_proj(MATRIX out, float fov, float ar, float near, float far) {
  matrix_zero(out);
  float angle = 1.f / (tanf(fov * 0.5f));
  float fn = 1.f / (near - far);
  out[0] = angle / ar;
  out[5] = angle;
  out[10] = -1 * far * fn;
  out[14] = near * far * fn;
  out[11] = 1.0f;
}

void matrix_viewport(MATRIX out, float w, float h) {
  matrix_unit(out);
  out[0] = 320;
  out[5] = 224;
  out[3] = 320;
  out[7] = 224;
  matrix_tsp(out);
}

void vector_rotate_y(VECTOR out, float r) {
  out[0] = (out[0] * cos(r)) + (out[2] * sin(r));
  out[2] = (-1.f * out[0] * sin(r)) + (out[2] * cos(r));
}
