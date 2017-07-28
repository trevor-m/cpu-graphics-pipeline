#ifndef SCENE_H
#define SCENE_H


#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "Matrix4.h"
#include "Triangle.h"
#include "mouse.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* creates a linked list of triangles, which will define our scene */
typedef struct TriangleList_t {
	Triangle *t;
	struct TriangleList_t *next;
} TriangleList;


/* implements the scene class */
class Scene {
	private:
		/* the scene class is basically a linked list of triangles */
		TriangleList *original_head;
		TriangleList *original_tail;

		Shader defaultShader;
		TileShader tileShader;
		LightShader lightShader;

	public:
		Scene() : lightShader(0, 0, 6.0, 0.000007f, 0.0014f, 1.0, 0.2, 0.2, 0.2, 0.8, 0.8, 0.8, 1.0, 1.0, 1.0) {
			original_head = NULL;		original_tail = NULL;
		};

		/* destructor */
		~Scene(){
			destroyList(&original_head, &original_tail);
		};

		void destroyList(TriangleList **head_ptr, TriangleList **tail_ptr) {
			TriangleList *ptr, *ptr2;
			
			if (*head_ptr) {
				for (ptr = *head_ptr; ptr;) {
					ptr2 = ptr->next;

					/* delete my triangle */
					delete ptr->t;

					/* free the node */
					free(ptr);	
					ptr = ptr2;
				}
			}

			*head_ptr = NULL;
			*tail_ptr = NULL;

			return;
		};

		TriangleList *newNode(void) {
			TriangleList *node = (TriangleList*)malloc(sizeof(TriangleList));
			node->next = NULL;
			node->t = NULL;

			return node;
		};

		void addTriangle(TriangleList **head_ptr, TriangleList **tail_ptr, Triangle *tri) {
			if ((*head_ptr) == NULL) {
				(*head_ptr) = newNode();
				(*head_ptr)->t = tri;
				(*tail_ptr) = (*head_ptr);
			}
			else {
				(*tail_ptr)->next = newNode();
				(*tail_ptr) = (*tail_ptr)->next;
				(*tail_ptr)->t = tri;
			}
			return;
		};

		
		void addWall(float x1, float y1, float x2, float y2, int wall_height, Texture *tex) {
			Triangle *t1, *t2;
			Vertex v0(x1, y1, 0);
			Vertex v1(x2, y2, 0);
			Vertex v2(x2, y2, wall_height);
			Vertex v3(x1, y1, wall_height);

			t1 = new Triangle(&v0, &v1, &v2, (Shader*)&lightShader);
			t2 = new Triangle(&v0, &v3, &v2, (Shader*)&lightShader);
			
			/* set the texture coordinates for the wall */
			t1->setCoords(0, 0, 0);
			t1->setCoords(1, 1, 0);
			t1->setCoords(2, 1, 1);

			t2->setCoords(0, 0, 0);
			t2->setCoords(1, 0, 1);
			t2->setCoords(2, 1, 1);

			t1->setTexture(tex);
			t2->setTexture(tex);

			/* add these triangles to the list */
			addTriangle(&original_head, &original_tail, t1);
			addTriangle(&original_head, &original_tail, t2);
		};

		void addFloor(float x1, float y1, float x2, float y2, Texture *tex, int scene_width, int scene_height) {
			Triangle *t1, *t2;
			Vertex v0(x1, y2, 0);
			Vertex v1(x2, y2, 0);
			Vertex v2(x2, y1, 0);
			Vertex v3(x1, y1, 0);

			t1 = new Triangle(&v0, &v1, &v2, (Shader*)&lightShader);
			t2 = new Triangle(&v2, &v3, &v0, (Shader*)&lightShader);
			
			/* set the texture coordinates for the wall */
			t1->setCoords(0, 0, 0);
			t1->setCoords(1, scene_width/4.0, 0);
			t1->setCoords(2, scene_width/4.0, scene_height/4.0);

			t2->setCoords(0, scene_width/4.0, scene_height/4.0);
			t2->setCoords(1, 0, scene_height/4.0);
			t2->setCoords(2, 0, 0);

			t1->setTexture(tex);
			t2->setTexture(tex);

			/* add these triangles to the list */
			addTriangle(&original_head, &original_tail, t1);
			addTriangle(&original_head, &original_tail, t2);
		};

		void renderSceneOpenGL(void) {
			TriangleList *ptr;

			/* now clear all the stuff */
			glClearColor(100/(float)255, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			/* now render the triangles in the list */
			for (ptr = original_head; ptr;) {
				ptr->t->renderOpenGL();
				ptr = ptr->next;
			}
			
			glDisable(GL_DEPTH_TEST);

			return;
		};	

		void renderSceneSoftware(void);
};

void loadScene(char *name);
void setDefaults(void);
void sceneCleanup(void);


void setPerspectiveProjection(float eye_fov, float aspect_ratio, float near, float far);
void setModelviewMatrix(float *eye_pos, float eye_theta, float eye_phi);

#endif		/* SCENE_H */