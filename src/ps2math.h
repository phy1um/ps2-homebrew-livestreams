
#ifndef PS2_MATH_H
#define PS2_MATH_H

float p2m_vec2_length(float *v2);
float p2m_vec2_dot(float *a, float *b);
void p2m_vec2_rotate(float *v2, float theta);
void p2m_vec2_scale(float *v, float s);

float p2m_vec3_length(float *v3);
float p2m_vec3_dot(float *a, float *b);
void p2m_vec3_scale(float *v, float s);

void p2m_m3_copy(float *a, float *b);
void p2m_m3_apply(float *m3, float *v);
void p2m_m3_add(float *a, float *b);
void p2m_m3_identity(float *m);
void p2m_m3_multiply(float *a, float *b, float *to);

#endif
