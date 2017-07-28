#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdlib.h>
#include <math.h>
#include "Matrix4.h"
#include "Vertex.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include <vector>


struct VertAttributes {
	Vertex v;
	float coords[2];
	float modelPos[3];
};

template <typename T>
void swap(T& a, T& b) {
	T temp = a;
	a = b;
	b = temp;
}

struct Edge {
	int x1, y1, x2, y2;
	int index1, index2; //indexes for vertex attributes

	void Swap() {
		swap(x1, x2);
		swap(y1, y2);
		swap(index1, index2);
	}
};

struct ScanLine {
	//x coords to draw between
	int x1, x2;

	//interpolants
	float z1, z2;
	float texCoords1[2];
	float texCoords2[2];
	float inverseW1, inverseW2;
	float modelPos1[3];
	float modelPos2[3];

	void Swap() {
		swap(x1, x2);
		swap(z1, z2);
		swap(texCoords1[0], texCoords2[0]);
		swap(texCoords1[1], texCoords2[1]);
		swap(inverseW1, inverseW2);
		swap(modelPos1[0], modelPos2[0]);
		swap(modelPos1[1], modelPos2[1]);
		swap(modelPos1[2], modelPos2[2]);
	}
};

/* implements the triangle class */
class Triangle {
	private:
		Vertex v[3];		/* the original vertices of the triangle */

		/* per-vertex values */
		float c[3][3];		/* color at each vertex */
		float coords[3][2];
		Vertex modelPos[3]; /* position in model space of each vertex for lighting calculations */

		/* normal of entire triangle */
		Vertex normal; 

		/* point to the texture that is bound, if any */
		Texture *tex;

		/* shader to draw this triangle with */
		Shader* shader;

		/* rasterization */
		void rasterize();
		//draws one half of a triangle between the two edges scanline by scanline
		void drawBetweenEdges(Edge& e1, Edge& e2, bool flip);
		//draw a scanline from line.x1 to line.x2
		void drawScanLine(int y, ScanLine& line, bool flip);

		//used to interpolate vertex attributes when clipping
		void interpolate(int prev, int curr, float alpha, float* outputCoords, float* outputPos);
	public:
		bool isfloor = false;
		int floornum;
		/* constructors */
		Triangle() {
			v[0].set(0, 0, 0);
			v[1].set(0, 0, 0);
			v[2].set(0, 0, 0);

			c[0][0] = 0.0;	c[0][1] = 0.0;	c[0][2] = 0.0;
			c[1][0] = 0.0;	c[1][1] = 0.0;	c[1][2] = 0.0;
			c[2][0] = 0.0;	c[2][1] = 0.0;	c[2][2] = 0.0;

			coords[0][0] = 0.0;		coords[0][1] = 0.0;	
			coords[1][0] = 0.0;		coords[1][1] = 0.0;	
			coords[2][0] = 0.0;		coords[2][1] = 0.0;

			tex = NULL;
		};

		Triangle(Vertex *v0, Vertex *v1, Vertex *v2, Shader* shader) {
			this->shader = shader;
			v[0] = (*v0);
			v[1] = (*v1);
			v[2] = (*v2);

			//save model space position
			for (int i = 0; i < 3; i++) {
				modelPos[i] = v[i];
			}

			//calculate normal
			Vertex v0v1 = v[1] - v[0];
			Vertex v0v2 = v[2] - v[0];
			normal = v0v1.cross(v0v2).normalize();
			if (normal.z < 0)
				normal.invert();

			c[0][0] = 0.0;	c[0][1] = 0.0;	c[0][2] = 0.0;
			c[1][0] = 0.0;	c[1][1] = 0.0;	c[1][2] = 0.0;
			c[2][0] = 0.0;	c[2][1] = 0.0;	c[2][2] = 0.0;
			
			coords[0][0] = 0.0;		coords[0][1] = 0.0;		coords[0][2] = 0.0;
			coords[1][0] = 0.0;		coords[1][1] = 0.0;		coords[1][2] = 0.0;
			coords[2][0] = 0.0;		coords[2][1] = 0.0;		coords[2][2] = 0.0;

			tex = NULL;
		};


		/* sets the color of vertex v of the triangle to (r,g,b) */
		void setColor(int v, int r, int g, int b) {
			if ((r < 0) || (r > 255) ||
				(g < 0) || (g > 255) ||
				(b < 0) || (b > 255)) {
				fprintf(stderr,"ERROR! Invalid color set in triangle\n");
				fflush(stderr);
				exit(-1);
			}

			c[v][0] = ((float)r/(float)255);
			c[v][1] = ((float)g/(float)255);	
			c[v][2] = ((float)b/(float)255);
	
			return;
		};

		void setCoords(int v, int s, int t) {
			coords[v][0] = s;
			coords[v][1] = t;

			return;
		};

		void setTexture(Texture *t) {
			tex = t;
			return;
		}

		/* openGL rendering of the triangle */
		void renderOpenGL(void);

		/* software rendering */
		void renderSoftware();
		void applyMVP(Matrix4& mvp);
		//clip v[dim] against w (pos or negative), allocates newly created triangle in new_triangle if any
		//returns true if triangle still exists after clipping
		//puts a new triangle in extra_triangle if it is created
		bool clipAgainst(int dim, bool pos, Triangle** extra_triangle);

		/* for debugging */
		void print(void) {
			fprintf(stderr,"Triangle:\n");
			v[0].print();
			v[1].print();
			v[2].print();
			return;
		};
};

#endif		/* TRIANGLE_H */