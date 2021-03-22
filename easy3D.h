#ifndef __EASY3D_H__
#define __EASY3D_H__

#include <pspge.h>
#include "graphics.h"

#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI/180.0)
#define RGB(r, g, b) ((r)|((g)<<8)|((b)<<16))

PspGeContext __atribute__((aligned(16))) geContext;

void GU_Start();
void GU_CameraLookAt(float camX, float camY, float camZ, float lookX, float lookY, float lookZ);
void GU_End();
void GU_DrawModel(float x, float y, float z, float a, float b, float c, Image *Tex, int Model);
void GU_DrawLine(float x1, float y1, float z1, Color color1, float x2, float y2, float z2, Color color2);
void GU_DrawPyramide(float x, float y, float z, float a, float b, float c, float Length, float hauteur, Image *tex, Color color);
void GU_DrawPlaneX(float x, float y, float z, float a, float b, float c, float Length, Image *tex, Color color);

#endif

