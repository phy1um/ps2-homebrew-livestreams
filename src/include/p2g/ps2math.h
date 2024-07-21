
#ifndef PS2_MATH_H
#define PS2_MATH_H

float p2m_vec2_length(const float *v2);
float p2m_vec2_dot(const float *a, const float *b);
void p2m_vec2_rotate(float *v2, float theta);
void p2m_vec2_scale(float *v, float s);

float p2m_vec3_length(const float *v3);
float p2m_vec3_dot(const float *a, const float *b);
void p2m_vec3_scale(float *v, float s);

float p2m_vec4_length(const float *v3);
float p2m_vec4_dot(const float *a, const float *b);
void p2m_vec4_scale(float *v, float s);

void p2m_m3_copy(float *a, const float *b);
void p2m_m3_apply(const float *m3, float *v);
void p2m_m3_add(float *a, const float *b);
void p2m_m3_identity(float *m);
void p2m_m3_multiply(const float *a, const float *b, float *to);

void p2m_m4_copy(float *a, const float *b);
void p2m_m4_apply(const float *m3, float *v);
void p2m_m4_add(float *a, const float *b);
void p2m_m4_identity(float *m);
void p2m_m4_multiply(const float *a, const float *b, float *to);

#endif
