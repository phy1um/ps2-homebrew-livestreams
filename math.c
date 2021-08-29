#include <math3d.h>
#include <math.h>


#define SWAP(i,j) do { t = a[i]; a[i] = a[j]; a[j] = t; } while(0)
void matrix_tsp(MATRIX a)
{
  float t;
  SWAP(1,4);
  SWAP(2,8);
  SWAP(3,12);
  SWAP(6,9);
  SWAP(7,13);
  SWAP(11,14);
}

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
  MATRIX t;
  matrix_unit(t);

  for (int i = 0; i < 3; i++) {
    out[i] = x[i];
    out[4 + i] = y[i];
    out[8 + i] = z[i];
    t[12 + i] =  c[i];
  }

  matrix_multiply(out, out, t);
  matrix_tsp(out);
}

void matrix_zero(MATRIX out)
{
  for (int i = 0; i < 16; i++) {
    out[i] = 0;
  }
}

void matrix_proj(MATRIX out, float fov, float ar, float near, float far)
{
  matrix_zero(out);
  float angle = 1 / (tanf(fov/2));
  out[0] = ar * angle;
  out[5] = angle;
  out[10] = far / (far - near);
  out[11] = 1;
  out[14] = (-far * near) / (far - near);
  matrix_tsp(out);
}

void matrix_viewport(MATRIX out, float w, float h)
{
  matrix_unit(out);
  out[0] = 320;
  out[5] = 224;
  out[3] = 320;
  out[7] = 224;
  matrix_tsp(out);
}

void vector_rotate_y(VECTOR out, float r)
{
  out[0] = (out[0] * cos(r)) + (out[2] * sin(r));
  out[2] = (-1.f * out[0] * sin(r)) + (out[2] * cos(r));
}
