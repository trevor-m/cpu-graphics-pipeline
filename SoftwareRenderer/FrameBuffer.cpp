#include <atlimage.h>
#include "FrameBuffer.h"

void FrameBuffer::clearBuffers() {
	memset(color_buffer, 0, sizeof(u08) * 3 * window_height * window_width);
	memset(stencil_buffer, 0, sizeof(u08) * window_height * window_width);
	for (int x = 0; x < window_width; x++) {
		for (int y = 0; y < window_height; y++) {
			*getDepthPtr(x, y) = DEPTH_MAX;
		}
	}
}

/* this function copies the color buffer to the given CImage */
void FrameBuffer :: copyColorBuffer(CImage *image) {
	int i, j;
	int pitch = image->GetPitch();
	u08 *ptr_dst = (u08 *)image->GetBits();
	u08 *ptr_src = getColorPtr(0, height - 1);
	/* note that CImage is upside down, so we start reading the framebuffer
	   from the top of the screen */

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			*(ptr_dst++) = *(ptr_src + 2);
			*(ptr_dst++) = *(ptr_src + 1);
			*(ptr_dst++) = *(ptr_src);

			ptr_src += 3;
		}

		ptr_src += -(width * 3) * 2;
		ptr_dst += pitch - (width * 3); 
	}
	return;
}

/* this function dumps the color framebuffer to the specified file.  I will
   use the CImage class to do this */
void FrameBuffer :: dumpColorBufferToFile(char *name) {
	CImage image;

	image.Create(width, height, 24);


	/* now copy color buffer to the CImage bit array */
	copyColorBuffer(&image);

	/* save the image to the file */
	image.Save(name);

	/* clean up the CImage */
	image.Destroy();

	return;
}


#include <windows.h>
#include "gl.h"

/* this function dumps the data array to the screen using glDrawPixels() */
void FrameBuffer :: dumpToScreen(void) {
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);

	return;
}
