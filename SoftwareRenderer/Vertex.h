#ifndef VERTEX_H
#define VERTEX_H

#include <stdio.h>
#include <cmath>

/* represents a vertex in homogenous coordinates */
class Vertex {
	private:
	public:
		float x, y, z, w;

		Vertex() {
			set(0, 0, 0);
		};

		Vertex(float _x, float _y, float _z) {
			set(_x, _y, _z);
		};

		Vertex(float _x, float _y, float _z, float _w) {
			x = _x;		y = _y;		z = _z;		w = _w;
		};

		void set(float _x, float _y, float _z) {
			x = _x;		y = _y;		z = _z;		w = 1;
		};

		/* for debugging */
		void print(void) {
			fprintf(stderr,"[%f %f %f %f]\n", x, y, z, w);
		}

		Vertex cross(Vertex& other) {
			// x    y    z
			//v[x] v[y] v[z]
			//o[x] o[y] o[z]
			Vertex result;
			result.x = y * other.z - z * other.y;
			result.y = -x * other.z + z * other.x;
			result.z = x * other.y - y * other.x;
			return result;
		}

		Vertex reflect(Vertex& normal) {
			return *this - normal * 2 * this->dot3(normal);
		}

		float dot3(Vertex& other) {
			return x * other.x + y * other.y + z * other.z;
		}
		
		Vertex operator-() {
			return Vertex(-x, -y, -z);
		}

		Vertex operator-(Vertex& other) {
			return Vertex(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		Vertex operator+(Vertex& other) {
			return Vertex(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		Vertex operator*(float scalar) {
			return Vertex(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		Vertex normalize() {
			float magnitude = length();
			return Vertex(x / magnitude, y / magnitude, z / magnitude);
		}

		//dot product between clipping plane
		/*float dotProduct(int dim, bool pos) {
			if (pos) {
				if (dim == 0)
					return x + w;
				else if (dim == 1)
					return y + w;
				else if (dim == 2)
					return z + w;
			}
			else {
				if (dim == 0)
					return -x + w;
				else if (dim == 1)
					return -y + w;
				else if (dim == 2)
					return z;
			}
			return 0;
		}*/

		float get(int dim) {
			if (dim == 0)
				return x;
			if (dim == 1)
				return y;
			if (dim == 2)
				return z;
			return w;
		}

		float length() {
			return sqrt(x*x + y*y + z*z);
		}

		float distance3(Vertex& other) {
			return sqrt(pow(other.x/other.w - x/w, 2) + pow(other.y/other.w - y/w, 2) + pow(other.z/other.w - z/w, 2));
		}

		void invert() {
			x = -x;
			y = -y;
			z = -z;
			w = -w;
		}
};

#endif		/* VERTEX_H */