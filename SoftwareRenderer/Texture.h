#ifndef TEXTURE_H
#define TEXTURE_H

#include <windows.h>
#include <atlimage.h>

#include "globals.h"

#if OPENGL_TEST
/* this includes opengl calls in the code */

#include <stdio.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "gl_ext.h"

#endif


#include <math.h>


/* implements the texture mapping class so that the triangles
   can be textured in our software renderer */
class Texture {
	private:
		int width, height;	/* width and height for the texture */
		u08 *data;			/* contains the color data for the texture */

		/* for opengl only */
		GLuint	tex;		/* handle to opengl texture (only used by opengl */
		void copyTextureData(CImage *image); 

		void getFloatColor(int u, int v, float *color) {
			u08 *ptr = data + (((v * width) + u) * 3); 
			color[0] = (float) *(ptr);
			color[1] = (float) *(ptr + 1);
			color[2] = (float) *(ptr + 2);
		}

	public:
		/* constructor takes the name of the file to use as a texture */
		Texture(char *name);

		/* destructor */
		~Texture() {
			if (data)
				free(data);
		};


		//looks up color at u,v
		//uses bilinear interpolation if bilerp_flag is on
		//uses nearest-neighbor otherwise
		void getInterpolatedColor(float u, float v, u08 *color);

		
		/* switches between nearest neighbor and bilinear interpolation */
		void switchTextureFiltering(bool flag);


		/* for opengl only */
		void bind(void) {
			glBindTexture(GL_TEXTURE_2D, tex);
			glEnable(GL_TEXTURE_2D);
		};

		void release(void) {
			glDisable(GL_TEXTURE_2D);
		};

};

#endif		/* TEXTURE_H */