
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspge.h>
#include <pspgu.h>

#include "easy3D.h"

void GU_Start() {
	sceGeSaveContext(&geContext);
	guStart();
}

void SCE_GumCameraLookAt(float camPos_x, float camPos_y, float camPos_z, float lookAt_x, float lookAt_y, float lookAt_z, float upVec_x, float upVec_y, float upVec_z) {
	ScePspFVector3 camPos;
	ScePspFVector3 lookAt;
	ScePspFVector3 upVec;

	camPos.x = camPos_x;
	camPos.y = camPos_y;
	camPos.z = camPos_z;
	lookAt.x = lookAt_x;
	lookAt.y = lookAt_y;
	lookAt.z = lookAt_z;
	upVec.x = upVec_x;
	upVec.y = upVec_y;
	upVec.z = upVec_z;

	sceGumLookAt(&camPos, &lookAt, &upVec);
}

void GU_CameraLookAt(float camX, float camY, float camZ, float lookX, float lookY, float lookZ) {
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(50, 16/9, 3, 10000);
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	SCE_GumCameraLookAt(camX, camY, camZ, lookX, lookY, lookZ, 0, 1, 0);
}

void GU_End() {
	sceGuFinish();
	sceGuSync(0, 0);
	sceGeRestoreContext(&geContext);
}

void SCE_GumDrawArray(int prim, int vtype, int n) {
	int quads = 0;
	int colorIndex = -1;
	if (vtype & GU_TEXTURE_32BITF) quads += 2;
	if (vtype & GU_COLOR_8888) {
		quads++;
		colorIndex = quads;
	}
	if (vtype & GU_NORMAL_32BITF) quads += 3;
	if (vtype & GU_VERTEX_32BITF) quads += 3;

	void *vertices = memalign(16, n * quads*4);
		
	sceKernelDcacheWritebackInvalidateAll();
	sceGumDrawArray(prim, vtype, n, NULL, vertices);
	free(vertices);
}

void SCE_GumTranslate(float X, float Y, float Z) {
	ScePspFVector3 v;
	v.x = X;
	v.y = Y;
	v.z = Z;
	sceGumTranslate(&v);
}

void SCE_GumRotateXYZ(float X, float Y, float Z) {
	ScePspFVector3 v;
	v.x = X;
	v.y = Y;
	v.z = Z;
	sceGumRotateXYZ(&v);
}

void GU_DrawModel(float x, float y, float z, float a, float b, float c, Image *Tex, int Model) {
	if(Tex != "false") {
		sceGuEnable(GU_TEXTURE_2D);
		sceGuTexImage(0, Tex->textureWidth, Tex->textureHeight, Tex->textureWidth, Tex->data);
		sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
		sceGuTexEnvColor(RGB(255,255,255));
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);
		sceGuTexScale(1, 1);
		sceGuTexOffset(0, 0);
		sceGuAmbientColor(RGB(255,255,255));
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		SCE_GumTranslate(x, y, z);
		SCE_GumRotateXYZ(returnRadians(a),returnRadians(b),returnRadians(c));
		SCE_GumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF+GU_COLOR_8888+GU_VERTEX_32BITF+GU_TRANSFORM_3D, Model);
	} else {
		sceGuDisable(GU_TEXTURE_2D);  
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		SCE_GumTranslate(x, y, z);
		SCE_GumRotateXYZ(returnRadians(a),returnRadians(b),returnRadians(c));
		SCE_GumDrawArray(GU_TRIANGLES, GU_COLOR_8888+GU_VERTEX_32BITF+GU_TRANSFORM_3D, Model);
	}
}

void GU_DrawLine(float x1, float y1, float z1, Color color1, float x2, float y2, float z2, Color color2) {
	int model[] = { 
		{color1, x1, y1, z1}, 
		{color2, x2, y2, z2} 
	};
	sceGuDisable(GU_TEXTURE_2D);  
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	SCE_GumTranslate(0, 0, 0);
	SCE_GumRotateXYZ(0, 0, 0);
	SCE_GumDrawArray(GU_LINES, GU_COLOR_8888+GU_VERTEX_32BITF+GU_TRANSFORM_3D, model);
}

float returnRadians(float number) {
	return number*RADIANS_PER_DEGREE;
}

void GU_DrawPyramide(float x, float y, float z, float a, float b, float c, float Length, float hauteur, Image *tex, Color color) {
	if(tex != "false") {
		int model = {
   			{1, 1, color, 0, 0, 0},
   			{1, 0, color, 1*Length, 0, 0},
   			{0, 0, color, 0.5*Length,  1*hauteur, 0.5*Length},

   			{1, 1, color, 1*Length, 0, 0},
   			{1, 0, color, 1*Length, 0,1*Length},
   			{0, 0, color, 0.5*Length, 1*hauteur, 0.5*Length},


   			{1, 1, color, 1*Length, 0, 1*Length},
  			{1, 0, color, 0, 0, 1*Length},
   			{0, 0, color, 0.5*Length, 1*hauteur, 0.5*Length},


   			{1, 1, color, 0, 0, 1*Length},
   			{1, 0, color, 0, 0, 0},
   			{0, 0, color, 0.5*Length, 1*hauteur, 0.5*Length}
		};
		sceGuEnable(GU_TEXTURE_2D);
		sceGuTexImage(0, tex->textureWidth, tex->textureHeight, tex->textureWidth, tex->data);
		sceGuTexFunc(Gu.TFX_MODULATE, GU_TCC_RGBA);
		sceGuTexEnvColor(RGB(255,255,255));
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);
		sceGuTexScale(1, 1);
		sceGuTexOffset(0, 0);
		sceGuAmbientColor(RGB(255,255,255));
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		SCE_GumTranslate(x, y, z);
		SCE_GumRotateXYZ(returnRadians(a),returnRadians(b),returnRadians(c));
		SCE_GumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF+GU_COLOR_8888+GU_VERTEX_32BITF+GU_TRANSFORM_3D, model);
	} else {
		int model = {
			{color,  0,  0, 0},
			{color,  1*Length,  0, 0},
			{color,  0.5*Length,  1*hauteur,  0.5*Length},

			{color,  1*Length,  0, 0},
			{color,  1*Length,  0,1*Length},
			{color,  0.5*Length,  1*hauteur, 0.5*Length},

			{color,  1*Length,  0, 1*Length},
			{color,  0,  0, 1*Length},
			{color,  0.5*Length,  1*hauteur,  0.5*Length},

			{color,  0,  0, 1*Length},
			{color,  0,  0, 0},
			{color, 0.5*Length,  1*hauteur,  0.5*Length}
		};
		sceGuDisable(GU_TEXTURE_2D);  
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		SCE_GumTranslate(x, y, z);
		SCE_GumRotateXYZ(returnRadians(a),returnRadians(b),returnRadians(c));
		SCE_GumDrawArray(GU_TRIANGLES, GU_COLOR_8888+GU_VERTEX_32BITF+GU_TRANSFORM_3D, model);
	}
}

void GU_DrawPlaneX(float x, float y, float z, float a, float b, float c, float Length, Image *tex, Color color) {
	if(tex != "false") {
		int model = {
			{0,0,color,1*Length,1*Length,0},
			{1,1,color,-1*Length,-1*Length,0},
			{1,0,color,-1*Length,1*Length,0},

			{0,0,color,1*Length,1*Length,0},
			{1,1,color,-1*Length,-1*Length,0},
			{0,1,color,1*Length,-1*Length,0},


			{0,0,color,1*Length,1*Length,0},
			{1,0,color,-1*Length,1*Length,0},
			{1,1,color,-1*Length,-1*Length,0},

			{0,0,color,1*Length,1*Length,0},
			{0,1,color,1*Length,-1*Length,0},
			{1,1,color,-1*Length,-1*Length,0}
		};
		sceGuEnable(GU_TEXTURE_2D);
		sceGuTexImage(0, tex->textureWidth, tex->textureHeight, tex->textureWidth, tex->data);
		sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
		sceGuTexEnvColor(RGB(255,255,255));
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);
		sceGuTexScale(1, 1);
		sceGuTexOffset(0, 0);
		sceGuAmbientColor(RGB(255,255,255));
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		SCE_GumTranslate(x, y+(Length), z);
		SCE_GumRotateXYZ(returnRadians(a),returnRadians(b),returnRadians(c));
		SCE_GumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF+GU_COLOR_8888+GU_VERTEX_32BITF+GU_TRANSFORM_3D, model);
	} else {
		int model = {
			{color,1*Length,1*Length,0},
			{color,-1*Length,-1*Length,0},
			{color,-1*Length,1*Length,0},

			{color,1*Length,1*Length,0},
			{color,-1*Length,-1*Length,0},
			{color,1*Length,-1*Length,0},


			{color,1*Length,1*Length,0},
			{color,-1*Length,1*Length,0},
			{color,-1*Length,-1*Length,0},

			{color,1*Length,1*Length,0},
			{color,1*Length,-1*Length,0},
			{color,-1*Length,-1*Length,0}

		};
		sceGuDisable(GU_TEXTURE_2D);  
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		SCE_GumTranslate(x, y+(Length), z);
		SCE_GumRotateXYZ(returnRadians(a),returnRadians(b),returnRadians(c));
		SCE_GumDrawArray(GU_TRIANGLES, GU_COLOR_8888+GU_VERTEX_32BITF+GU_TRANSFORM_3D, model);
	}
}

