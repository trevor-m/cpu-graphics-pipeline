#include <stdio.h>
#include "OpenGL.h"
#include "Scene.h"
#include "FrameBuffer.h"

Scene scene;
FrameBuffer fb(window_width, window_height);
Matrix4 modelviewMatrix;
Matrix4 projectionMatrix;

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr,"Usage: run <file>\n");
		fprintf(stderr,"argc: %d\n", argc);
		return -1;
	}

	setDefaults();

	initGLUT(argc, argv);
	
	sceneCleanup();

	return 1;
}