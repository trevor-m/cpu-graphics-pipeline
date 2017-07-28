#include "Triangle.h"
//#include "Matrix4.h"
extern FrameBuffer fb;
extern bool show_stencil;
extern float eye_pos[3];

/* for openGL rendering of the triangle */
void Triangle :: renderOpenGL(void) {
	/* first bind the texture */
	if (tex) {
		tex->bind();
	}

	/* now draw the triangle */
	glBegin(GL_TRIANGLES);
		glTexCoord2fv(coords[0]);
		glColor3fv(c[0]);
		glVertex3f(v[0].x, v[0].y, v[0].z);

		glTexCoord2fv(coords[1]);
		glColor3fv(c[1]);
		glVertex3f(v[1].x, v[1].y, v[1].z);

		glTexCoord2fv(coords[2]);
		glColor3fv(c[2]);
		glVertex3f(v[2].x, v[2].y, v[2].z);
	glEnd();

	/* release the texture */
	if (tex) {
		tex->release();
	}

	return;
}

bool shadowTest(Vertex& line) {
	return (line.x == 0) ? (line.y > 0) : (line.y > 0);
}

bool insideLine(float e, Vertex& line) {
	return (e == 0) ? shadowTest(line) : (e > 0);
}

void Triangle::applyMVP(Matrix4& mvp) {
	for (int i = 0; i < 3; i++) {
		//apply mvp
		v[i] = mvp * v[i];
	}
}

void Triangle::interpolate(int prev, int curr, float alpha, float* outputCoords, float* outputPos) {
	for (int i = 0; i < 2; i++)
		outputCoords[i] = ((1 - alpha) * coords[prev][i] + alpha * coords[curr][i]);
	for (int i = 0; i < 3; i++)
		outputPos[i] = ((1 - alpha) * modelPos[prev].get(i) + alpha * modelPos[curr].get(i));
}

void Triangle::drawScanLine(int y, ScanLine& line, bool flip) {
	if (line.x2 < line.x1)
		line.Swap();
	int xdiff = line.x2 - line.x1;
	if (xdiff == 0)
		return;

	float alpha = 0.0f;
	float alphaInc = 1.0f / (float)xdiff;

	// draw each pixel in the span
	for (int x = line.x1; x < line.x2; x++) {
		//SetPixel(x, y, span.Color1 + (colordiff * alpha));
		float z = line.z1 + (line.z2 - line.z1) * alpha;
		if (z < *fb.getDepthPtr(x, y)) {
			//write depth buffer
			*fb.getDepthPtr(x, y) = z;
			//increment stencil buffer
			*(fb.getStencilPtr(x, y)) = *fb.getStencilPtr(x, y) + 1;

			//show stencil buffer
			if (show_stencil) {
				fb.getColorPtr(x, y)[0] = *fb.getStencilPtr(x, y) * 10;
				fb.getColorPtr(x, y)[1] = *fb.getStencilPtr(x, y) * 20;
				fb.getColorPtr(x, y)[2] = *fb.getStencilPtr(x, y) * 40;
			}
			//or render scene?
			else {
				//get perspective correct texture coords
				float correct = line.inverseW1 + (line.inverseW2 - line.inverseW1) * alpha;
				float u = (line.texCoords1[0] + (line.texCoords2[0] - line.texCoords1[0]) * alpha) / correct;
				float v = (line.texCoords1[1] + (line.texCoords2[1] - line.texCoords1[1]) * alpha) / correct;

				//interpolate modelPos
				Vertex modelPosition;
				modelPosition.x = (line.modelPos1[0] + (line.modelPos2[0] - line.modelPos1[0]) * alpha) / correct;
				modelPosition.y = (line.modelPos1[1] + (line.modelPos2[1] - line.modelPos1[1]) * alpha) / correct;
				modelPosition.z = (line.modelPos1[2] + (line.modelPos2[2] - line.modelPos1[2]) * alpha) / correct;

				Vertex viewPosition = Vertex(eye_pos[0], eye_pos[1], eye_pos[2]);
				Vertex norm = normal;
				if(flip)
					norm.invert();
				//run shader
				shader->Shade(fb.getColorPtr(x, y), u, v, tex, norm, viewPosition, modelPosition);
				//texture lookup into color buffer
				//tex->getInterpolatedColor(u / correct, v / correct, fb.getColorPtr(x, y));
			}
		}
		alpha += alphaInc;
	}
}

void Triangle::drawBetweenEdges(Edge& e1, Edge& e2, bool flip) {
	//swap if in reversed order
	if (e2.y1 > e2.y2) {
		e2.Swap();
	}
	if (e1.y1 > e1.y2) {
		e1.Swap();
	}
	float e1DeltaY = e1.y2 - e1.y1;
	if (e1DeltaY == 0.0f)
		return;

	float e2DeltaY = e2.y2 - e2.y1;
	if (e2DeltaY == 0.0f)
		return;

	float alpha1 = (float)(e2.y1 - e1.y1) / e1DeltaY;
	float alphaInc1 = 1.0f / e1DeltaY;
	float alpha2 = 0.0f;
	float alphaInc2 = 1.0f / e2DeltaY;
	
	for (int y = e2.y1; y < e2.y2; y++) {
		//create scan line
		ScanLine scanLine;
		scanLine.x1 = e1.x1 + floor((e1.x2 - e1.x1) * alpha1);
		scanLine.x2 = e2.x1 + floor((e2.x2 - e2.x1) * alpha2);
		scanLine.z1 = v[e1.index1].z + (v[e1.index2].z - v[e1.index1].z) * alpha1;
		scanLine.z2 = v[e2.index1].z + (v[e2.index2].z - v[e2.index1].z) * alpha2;
		scanLine.inverseW1 = 1/v[e1.index1].w + (1/v[e1.index2].w - 1/v[e1.index1].w) * alpha1;
		scanLine.inverseW2 = 1/v[e2.index1].w + (1/v[e2.index2].w - 1/v[e2.index1].w) * alpha2;
		for (int i = 0; i < 2; i++) {
			scanLine.texCoords1[i] = coords[e1.index1][i] / v[e1.index1].w + (coords[e1.index2][i] / v[e1.index2].w - coords[e1.index1][i] / v[e1.index1].w) * alpha1;
			scanLine.texCoords2[i] = coords[e2.index1][i] / v[e2.index1].w + (coords[e2.index2][i] / v[e2.index2].w - coords[e2.index1][i] / v[e2.index1].w) * alpha2;
		}
		for (int i = 0; i < 3; i++) {
			scanLine.modelPos1[i] = modelPos[e1.index1].get(i) / v[e1.index1].w + (modelPos[e1.index2].get(i) / v[e1.index2].w - modelPos[e1.index1].get(i) / v[e1.index1].w) * alpha1;
			scanLine.modelPos2[i] = modelPos[e2.index1].get(i) / v[e2.index1].w + (modelPos[e2.index2].get(i) / v[e2.index2].w - modelPos[e2.index1].get(i) / v[e2.index1].w) * alpha2;
		}
		//draw
		drawScanLine(y, scanLine, flip);

		//next line
		alpha1 += alphaInc1;
		alpha2 += alphaInc2;
	}
}

void Triangle::rasterize() {
	//get all edges
	Edge edges[3];
	for (int i = 0; i < 3; i++) {
		edges[i].x1 = v[i].x;
		edges[i].y1 = v[i].y;
		edges[i].index1 = i;
		edges[i].x2 = v[(i+1) % 3].x;
		edges[i].y2 = v[(i+1) % 3].y;
		edges[i].index2 = (i + 1) % 3;
	}

	//find long and short edges
	int maxLength = 0;
	int longest = 0;
	for (int i = 0; i < 3; i++) {
		int length = abs(edges[i].y2 - edges[i].y1);
		if (length > maxLength) {
			maxLength = length;
			longest = i;
		}
	}
	int shorter1 = (longest + 1) % 3;
	int shorter2 = (longest + 2) % 3;

	//trinagle is front facing?
	Vertex v0v1 = v[1] - v[0];
	Vertex v0v2 = v[2] - v[0];
	Vertex normal = v0v1.cross(v0v2);
	bool flip = (normal.z <= 0);

	//fill in two halves of triangle
	drawBetweenEdges(edges[longest], edges[shorter1], flip);
	drawBetweenEdges(edges[longest], edges[shorter2], flip);
}

bool Triangle::clipAgainst(int dim, bool pos, Triangle** extra_triangle) {
	std::vector<VertAttributes> finalVertices;

	//go through all vertex->vertex lines
	//2->0, 0->1, 1->2
	int prev = 2;
	int prevDot;
	if(pos)
		prevDot = (v[prev].get(dim) <= v[prev].w) ? 1 : -1;
	else
		prevDot = (v[prev].get(dim) > -v[prev].w) ? 1 : -1;

	for (int curr = 0; curr < 3; curr++) {
		int currDot;
		if(pos)
			currDot = (v[curr].get(dim) <= v[curr].w) ? 1 : -1;
		else
			currDot = (v[curr].get(dim) > -v[curr].w) ? 1 : -1;
		
		if (currDot * prevDot < 0) {
			//clip against w = +-dim
			float intersectionFactor;
			if (pos)
				intersectionFactor = (v[prev].w - v[prev].get(dim)) / ((v[prev].w - v[prev].get(dim)) - (v[curr].w - v[curr].get(dim)));
			else
				intersectionFactor = (v[prev].w + v[prev].get(dim)) / ((v[prev].w + v[prev].get(dim)) - (v[curr].w + v[curr].get(dim)));

			Vertex newPoint = v[prev] + (v[curr] - v[prev]) * intersectionFactor;

			//compute values at newPoint and add to list
			VertAttributes va;
			interpolate(prev, curr, intersectionFactor, va.coords, va.modelPos);
			va.v = newPoint;
			finalVertices.push_back(va);
		}
		if (currDot > 0) {
			//copy values from v[curr]
			VertAttributes va;
			va.coords[0] = coords[curr][0];
			va.coords[1] = coords[curr][1];
			va.v = v[curr];
			va.modelPos[0] = modelPos[curr].x;
			va.modelPos[1] = modelPos[curr].y;
			va.modelPos[2] = modelPos[curr].z;
			finalVertices.push_back(va);
		}
		//go to next vertex->vertex line
		prevDot = currDot;
		prev = curr;
	}
	
	//build new geomtry with list of vertices
	if (finalVertices.size() == 3) {
		for (int i = 0; i < 3; i++) {
			v[i] = finalVertices[i].v;
			//depth[i] = finalVertices[i].depth;
			coords[i][0] = finalVertices[i].coords[0];
			coords[i][1] = finalVertices[i].coords[1];
			modelPos[i].x = finalVertices[i].modelPos[0];
			modelPos[i].y = finalVertices[i].modelPos[1];
			modelPos[i].z = finalVertices[i].modelPos[2];
		}
		return true;
	}
	else if (finalVertices.size() == 4) {
		for (int i = 0; i < 3; i++) {
			v[i] = finalVertices[i].v;
			//depth[i] = finalVertices[i].depth;
			coords[i][0] = finalVertices[i].coords[0];
			coords[i][1] = finalVertices[i].coords[1];
			modelPos[i].x = finalVertices[i].modelPos[0];
			modelPos[i].y = finalVertices[i].modelPos[1];
			modelPos[i].z = finalVertices[i].modelPos[2];
		}

		//copy into new triangle
		*extra_triangle = new Triangle();
		(*extra_triangle)->shader = this->shader;
		(*extra_triangle)->normal = this->normal; //normal doesn't change
		(*extra_triangle)->tex = this->tex;

		for (int i = 0; i < 3; i++) {
			(*extra_triangle)->v[i] = finalVertices[(i+2)%4].v;
			//(*extra_triangle)->depth[i] = finalVertices[(i + 2) % 4].depth;
			(*extra_triangle)->coords[i][0] = finalVertices[(i + 2) % 4].coords[0];
			(*extra_triangle)->coords[i][1] = finalVertices[(i + 2) % 4].coords[1];
			(*extra_triangle)->modelPos[i].x = finalVertices[(i + 2) % 4].modelPos[0];
			(*extra_triangle)->modelPos[i].y = finalVertices[(i + 2) % 4].modelPos[1];
			(*extra_triangle)->modelPos[i].z = finalVertices[(i + 2) % 4].modelPos[2];
		}
		return true;
	}
	//clipped completely
	return false;
}

void Triangle::renderSoftware() {
	for (int i = 0; i < 3; i++) {
		//perspective divide
		v[i].x = v[i].x / v[i].w;
		v[i].y = v[i].y / v[i].w;
		v[i].z = v[i].z / v[i].w;

		//convert from NDC to screen coordinates
		//Viewport transform
		v[i].x = (v[i].x + 1.0) * window_width / 2.0;
		v[i].y = (v[i].y + 1.0) * window_height / 2.0;

		//set z to 1 if doing BB rasterization(for cross products)
		//v[i].z = 1;
	}

	rasterize();
	//BOUNDING BOX RASTERIZATION
	/*int min_x = floor(min(min(v[0].x, v[1].x), v[2].x));
	int max_x = ceil(max(max(v[0].x, v[1].x), v[2].x));
	int min_y = floor(min(min(v[0].y, v[1].y), v[2].y));
	int max_y = ceil(max(max(v[0].y, v[1].y), v[2].y));
	Vertex line0 = v[0].cross(v[1]);
	Vertex line1 = v[1].cross(v[2]);
	Vertex line2 = v[2].cross(v[0]);

	//invert lines if vertices are not in CCW order
	Vertex v0v1 = v[1] - v[0];
	Vertex v0v2 = v[2] - v[0];
	Vertex normal = v0v1.cross(v0v2);
	bool ccw = (normal.z > 0);
	if (!ccw) {
		line0.invert();
		line1.invert();
		line2.invert();
	}

	//set up rasterization
	Vertex point = Vertex(min_x, min_y, 1);
	float e0 = point.dot3(line0);
	float e1 = point.dot3(line1);
	float e2 = point.dot3(line2);*/

	//set up interpolation
	//float area = normal.length() / 2;
	//Vertex edge0 = v[2] - v[1];
	//Vertex edge1 = v[0] - v[2];

	//bounding box rasterization
	/*for (int y = min_y; y < max_y; y++) {
		for (int x = min_x; x < max_x; x++) {
			//inside all 3 lines
			if (insideLine(e0, line0) && insideLine(e1, line1) && insideLine(e2, line2) &&
				!(x < 0 || x >= window_width || y < 0 || y >= window_height)) {
				//interpolate
				Vertex p = Vertex(x, y, 1);
				Vertex vp1 = p - v[1];
				Vertex vp2 = p - v[2];
				float b0 = (edge0.cross(vp1).length() / 2) / area;
				float b1 = (edge1.cross(vp2).length() / 2) / area;

				//correct perspective
				float correctFactor = 1 / (b0 * inverseW[0] + b1 * inverseW[1] + (1 - b0 - b1)*inverseW[2]);

				//color
				float color[3];
				for(int i = 0; i < 3; i++) {
					color[i] = b0 * c[0][i]/v[0].w + b1 * c[1][i]/v[1].w + (1-b0-b1)*c[2][i]/v[2].w;
					color[i] *= correctFactor;
				}

				//texture coords
				float texCoords[2];
				for (int i = 0; i < 2; i++) {
					texCoords[i] = b0 * coords[0][i]/v[0].w + b1 * coords[1][i]/v[1].w + (1 - b0 - b1)*coords[2][i]/v[2].w;
					texCoords[i] *= correctFactor;
				}

				//depth
				float z = b0 * depth[0] + b1 * depth[1] + (1 - b0 - b1) * depth[2];

				//depth test
				if (z < *fb.getDepthPtr(x, y)) {
					//texture lookup + write into color buffer
					tex->getInterpolatedColor(texCoords[0], texCoords[1], fb.getColorPtr(x, y));

					//write depth buffer
					*fb.getDepthPtr(x, y) = z;

					//when displaying color
					//fb.getColorPtr(x, y)[0] = color[0] * 255;
					//fb.getColorPtr(x, y)[1] = color[1] * 255;
					//fb.getColorPtr(x, y)[2] = color[2] * 255;
				}
				//display depth buffer
				//for(int i = 0; i < 3; i++)
					//fb.getColorPtr(x, y)[i] = (*fb.getDepthPtr(x, y) * 255);
			}

			e0 += line0.x;
			e1 += line1.x;
			e2 += line2.x;
		}
		e0 += -(max_x - min_x)* line0.x + line0.y;
		e1 += -(max_x - min_x)* line1.x + line1.y;
		e2 += -(max_x - min_x)* line2.x + line2.y;
	}
	*/
}