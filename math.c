#include <math3d.h>
#include <math.h>

void vector_cross(VECTOR out, VECTOR a, VECTOR b)
{
  out[0] = a[1] * b[2] - a[2] * b[1];
  out[1] = a[2] * b[0] - a[0] * b[2];
  out[2] = a[0] * b[1] - a[1] * b[0];
}

void vector_sub(VECTOR out, VECTOR a, VECTOR b)
{
  out[0] = a[0] - b[0];
  out[1] = a[1] - b[1];
  out[2] = a[2] - b[2];
  out[3] = a[3] - b[3];
}

void matrix_lookat(MATRIX out, VECTOR eye, VECTOR c, VECTOR up) 
{
  VECTOR z, x, y;
  vector_sub(z, eye, c);
  vector_normalize(z, z);

  vector_cross(x, up, z);
  vector_normalize(x, x);

  vector_cross(y, z, x);
  vector_normalize(y, y);

  matrix_unit(out);

  for (int i = 0; i < 3; i++) {
    out[i] = x[i];
    out[4 + i] = y[i];
    out[8 + i] = z[i];
    // out[4 * (i + 1) - 1] = -1 * c[i];
  }

  MATRIX t;
  matrix_unit(t);
  t[12] = -1 * c[0];
  t[13] = -1 * c[1];
  t[14] = -1 * c[2];

  matrix_multiply(out, out, t);
}
