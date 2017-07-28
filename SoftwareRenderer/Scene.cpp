#include "Scene.h"
extern FrameBuffer fb;
extern Matrix4 modelviewMatrix;
extern Matrix4 projectionMatrix;
extern bool clip_w;

/* set the perspective projection matrix given the following values */
void setPerspectiveProjection(float eye_fov, float aspect_ratio, float n, float f) {
	projectionMatrix = Matrix4::Perspective(eye_fov * (M_PI/180.0f), aspect_ratio, n, f);
}


/* set the modelview matrix with the given parameters */
void setModelviewMatrix(float *eye_pos, float eye_theta, float eye_phi) {
	modelviewMatrix = Matrix4::RotateX(-eye_phi* (M_PI / 180.0f)) * Matrix4::RotateZ(-eye_theta* (M_PI / 180.0f))  * Matrix4::Translate(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
}

/* this implements the software rendering of the scene */ 
void Scene :: renderSceneSoftware(void) {
	//clear color in framebuffer
	fb.clearBuffers();

	//build a list of triangles to check clipping
	TriangleList* clip_head = NULL;
	TriangleList* clip_tail = NULL;

	//build MVP matrix
	Matrix4 mvp = projectionMatrix * modelviewMatrix;

	//apply MVP to all triangles
	for (TriangleList* t = original_head; t != NULL; t = t->next) {
		//make copy of original triangle
		Triangle* new_triangle = new Triangle();
		*new_triangle = *(t->t);

		//apply MVP
		new_triangle->applyMVP(mvp);
		//add to clip list
		addTriangle(&clip_head, &clip_tail, new_triangle);
	}

	//clip against each plane, making a new list each time
	TriangleList* next_head = NULL;
	TriangleList* next_tail = NULL;
	//clip against w = +x,-x,+y,-y,+z,-z
	//dim = 0 for x, 1 for y, 2 for z
	//pos is false for - and true for +
	for (int dim = 0; dim < 3; dim++) {
		for (int pos = 0; pos < 2; pos++) {
			for (TriangleList* t = clip_head; t != NULL; t = t->next) {
				Triangle* new_triangle = new Triangle();
				*new_triangle = *(t->t);

				Triangle* extra_triangle = NULL;
				if (new_triangle->clipAgainst(dim, pos, &extra_triangle)) {
					if (extra_triangle != NULL)
						addTriangle(&next_head, &next_tail, extra_triangle);
					addTriangle(&next_head, &next_tail, new_triangle);
				}
			}

			//swap lists
			destroyList(&clip_head, &clip_tail);
			clip_head = next_head;
			clip_tail = next_tail;
			next_head = NULL;
			next_tail = NULL;
		}
	}

	//render all clipped triangles and delete
	for (TriangleList* t = clip_head; t != NULL; t = t->next) {
		t->t->renderSoftware();
	}
	destroyList(&clip_head, &clip_tail);

	/*Vertex v0 = Vertex(-0.5, -0.5, 1, 1);
	Vertex v1 = Vertex(-0.4, 0.5, 1, 1);
	Vertex v2 = Vertex(0.5, 0, 1, 1);
	Triangle* t = new Triangle(&v0, &v1, &v2);
	t->setColor(0, 255, 0, 0);
	t->setColor(1, 0, 255, 0);
	t->setColor(2, 0, 0, 255);

	t->renderSoftware();*/
	
	return;
}