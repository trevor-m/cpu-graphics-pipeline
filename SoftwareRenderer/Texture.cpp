#include <atlimage.h>

#include "globals.h"

#include "Texture.h"

extern bool bilerp_flag;

void Texture::getInterpolatedColor(float u, float v, u08 *color) {
	u = fmod(u, 1.0);
	v = fmod(v, 1.0);
	u *= width - 1;
	v *= height - 1;
	int x = u;
	int y = v ;
	if (x >= width)
		x = width - 1;
	else if (x < 0)
		x = 0;
	if (y >= height)
		y = height - 1;
	else if (y < 0)
		y = 0;
	if (bilerp_flag) {
		float dx = u - x;
		float dy = v - y;

		//dont exceed width-1, height-1
		int xplusone = x + 1;
		int yplusone = y + 1;
		if (xplusone > width - 1) {
			xplusone = width - 1;
			dx = 0;
		}
		if (yplusone > height - 1) {
			yplusone = height - 1;
			dy = 0;
		}

		//look up surrounding colors
		float c00[3], c01[3], c10[3], c11[3];
		getFloatColor(x, y, c00);
		getFloatColor(x, yplusone, c01);
		getFloatColor(xplusone, y, c10);
		getFloatColor(xplusone, yplusone, c11);

		for (int i = 0; i < 3; i++)
			color[i] = (1 - dy)* ((1 - dx)*c00[i] + dx*c10[i]) + dy * ((1 - dx) *c01[i] + dx*c11[i]);
	}
	else {
		
		u08 *ptr = data + (((y * width) + x) * 3);
		color[0] = *(ptr);
		color[1] = *(ptr + 1);
		color[2] = *(ptr + 2);
	}
}

/* this function copies the color buffer to the given CImage */
void Texture :: copyTextureData(CImage *image) {
	int i, j;
	int pitch = image->GetPitch();
	u08 *ptr_src = (u08 *)image->GetBits();
	u08 *ptr_dst = data + (height - 1) * width * 3 ;


	/* note that CImage is upside down, so we start reading the framebuffer
	   from the top of the screen */

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			*(ptr_dst++) = *(ptr_src + 2);
			*(ptr_dst++) = *(ptr_src + 1);
			*(ptr_dst++) = *(ptr_src);

			ptr_src += 3;
		}

		ptr_dst += -(width * 3) * 2;
		ptr_src += pitch - (width * 3); 
	}

	return;
}


Texture :: Texture(char *name) {
	CImage image;

	image.Load(name);

	if (image.IsNull()) {
		fprintf(stderr,"ERROR! Could not read texture %s\n", name);
		fflush(stderr);
		exit(-1);
	}

	/* now copy the image into the data array */
	width = image.GetWidth();
	height = image.GetHeight();

	/* make room for the data array */
	data = (u08 *)malloc(sizeof(u08) * width * height * 3);

	/* now copy the contents from one to the other */
	copyTextureData(&image);

	/* finally cleanup after myself */
	image.Destroy();

	/* if opengl i should bind it and all that */
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (bilerp_flag) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);


	return;
};

/* switches between nearest neighbor and bilinear interpolation */
void Texture :: switchTextureFiltering(bool flag) {

	bilerp_flag = flag;

	glBindTexture(GL_TEXTURE_2D, tex);
	if (bilerp_flag) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	return;
}