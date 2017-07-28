#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "globals.h"

#include "Scene.h"

/* store the eye position */
float eye_theta;			/* the theta angle of the eye (the longitude).  it is positive in a counterclockwise direction when looking down -z */
float eye_phi;				/* the phi angle of the eye (the latitude).  i define 0 as the north pole. */
float eye_fov;				/* field of view of the eye */
float eye_pos[3];			/* the position of the eye, computed from eye_theta, eye_phi, eye_distance and eye_origin */

float sight_vector[3];		/* vector in the direction of the eye */
float up_vector[3];
float right_vector[3];


float speed = 0.3;

static float drag_x, drag_y;
static float translate_x, translate_y;
static float zoom_y;

static int left_button = GLUT_UP, middle_button = GLUT_UP, right_button = GLUT_UP;

#define FLOAT_EPSILON 0.0000001


extern bool opengl_test;



void computeUpSightAndRightVectors(void);

bool floatEqual(float a, float b) {
	double diff = a - b;

	/* return true if they are within DOUBLE_EPSILON of each other */
	return ((-FLOAT_EPSILON < diff) && (diff < FLOAT_EPSILON));
}

void printVector(char *phrase, float *v) {
	fprintf(stderr,"%s: %2.10f %2.10f %2.10f\n", phrase, v[0], v[1], v[2]);
}

void reshape(int w, int h) {
	/* set the viewport */
	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(eye_fov,				/* field of view in degree */
				   (double)w/(double)h, /* aspect ratio */
					0.1, 700);

	return;
}

/* this function establishes the modelview matrix using either openGL calls or
   software calls which you will fill in */
void computeProjectionMatrix(void) {
	if (opengl_test) {
		/* openGL mode */
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(eye_fov,		/* field of view in degree */
					   (double)window_width/(double)window_height, /* aspect ratio */
						0.1,		/* near plane */
						700);		/* far plane */
	}
	else {
		/* software rendering mode */
		setPerspectiveProjection(eye_fov, /* field of view in degree */
								(double)window_width/(double)window_height, /* aspect ratio */
								 0.1, 700);
	}

	return;
}

/* this function establishes the modelview matrix using either openGL calls or
   software calls which you will fill in */
void computeModelViewMatrix(void) {
	if (opengl_test) {
		/* openGL mode */
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		/* now rotate by phi */
 		glRotatef(-eye_phi, 1, 0, 0);

		/* now rotate by theta */
		glRotatef(-eye_theta, 0, 0, 1);
		
		/* translate to get the eye origin in the center of the coordinate system */
		glTranslatef(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
	}
	else {
		/* software rendering mode */
		setModelviewMatrix(eye_pos, eye_theta, eye_phi);
	}

	return;
}




float drag_constant = 0.75;


void beginDrag(int x, int y) {
    drag_x = x;
    drag_y = y;
}

void dragView(int x, int y) {
    eye_theta = eye_theta + drag_constant * (drag_x - x);
    eye_phi = eye_phi + drag_constant * (drag_y - y);


	eye_phi = (eye_phi < 0) ? 0 : eye_phi;
	eye_phi = (eye_phi > 180) ? 180 : eye_phi;

	drag_x = x;
	drag_y = y;
	
	computeUpSightAndRightVectors();
	return;
}



float zoom_constant = 0.1;// 0.2;

void beginZoom(int x, int y) {
	zoom_y = y;
}



/* these rotations all assume the right-hand rule, so if the vector is pointing towards the
   viewer the positive rotation is counterclockwise */
void rotateAboutX(float *vector, float angle) {
	float new_vector[3];

	/* note that the ith component of the vector stays the same */
	new_vector[1] = cos(angle * MY_PI/180.0) * vector[1] - sin(angle * MY_PI/180.0) * vector[2];
	new_vector[2] = sin(angle * MY_PI/180.0) * vector[1] + cos(angle * MY_PI/180.0) * vector[2];

	/* copy it back to up_vector */
	vector[1] = new_vector[1];
	vector[2] = new_vector[2];
}


void rotateAboutY(float *vector, float angle) {
	float new_vector[3];

	/* note that the jth component of the vector stays the same */
	new_vector[0] = cos(angle * MY_PI/180.0) * vector[0] + sin(angle * MY_PI/180.0) * vector[2];
	new_vector[2] = -sin(angle * MY_PI/180.0) * vector[0] + cos(angle * MY_PI/180.0) * vector[2];

	/* copy it back to up_vector */
	vector[0] = new_vector[0];
	vector[2] = new_vector[2];
}


void rotateAboutZ(float *vector, float angle) {
	float new_vector[3];

	/* note that the kth component of the vector stays the same */
	new_vector[0] = cos(angle * MY_PI/180.0) * vector[0] - sin(angle * MY_PI/180.0) * vector[1];
	new_vector[1] = sin(angle * MY_PI/180.0) * vector[0] + cos(angle * MY_PI/180.0) * vector[1];

	/* copy it back to up_vector */
	vector[0] = new_vector[0];
	vector[1] = new_vector[1];
}


float translate_constant = 0.003;

void beginTranslate(int x, int y) {
	translate_x = x;
	translate_y = y;
}

float dotProduct(float *v1, float *v2) {
	return ((v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]));
}

void scaleVector(float *new_vector, float *vector, float scale) {
	new_vector[0] = vector[0] * scale;
	new_vector[1] = vector[1] * scale;
	new_vector[2] = vector[2] * scale;
}

void addVector(float *res_vector, float *vector1, float *vector2) {
	res_vector[0] = vector1[0] + vector2[0];
	res_vector[1] = vector1[1] + vector2[1];
	res_vector[2] = vector1[2] + vector2[2];
	return;
}

void crossProduct(float *res_vector, float *vector1, float *vector2) {
	res_vector[0] = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
	res_vector[1] = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
	res_vector[2] = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
}


/* this function computes some useful vectors to determine where the
   viewer is facing.  this is used in the move functions, since you
   move in the direction you are facing, for example */
void computeUpSightAndRightVectors(void) {
	/* initialize the up vector */
	up_vector[0] = 0;	up_vector[1] = 1;	up_vector[2] = 0;

	rotateAboutX(up_vector, eye_phi);
	rotateAboutZ(up_vector, eye_theta);

	/* at this point i have a valid up vector */

	/* now compute the line of sight vector */
	sight_vector[0] = 0;	sight_vector[1] = 0;	sight_vector[2] = -1;
	
	rotateAboutX(sight_vector, eye_phi);
	rotateAboutZ(sight_vector, eye_theta);

	/* for debugging, make sure that these two are normal two each other */
	if (!floatEqual(dotProduct(up_vector, sight_vector), 0)) {
		fprintf(stderr,"ERROR! The dot product between the up_vector and sight_vector should be 0!\n");
		fflush(stderr);
		exit(-1);
	}

	/* now take the cross product between sight vector and up vector to get the right vector */
	crossProduct(right_vector, sight_vector, up_vector);
	return;
}

void mouseInput(int button, int state, int x, int y) {
    switch(button) {
		case GLUT_LEFT_BUTTON:
			left_button = state;
			if (state == GLUT_DOWN) {
				beginDrag(x, y);
			}
			else {
				dragView(x, y);
				glutPostRedisplay();
			}
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
	}

	return;
}

void mouseMotion(int x, int y) {
	dragView(x, y);
    glutPostRedisplay();
}


/* function to initialize the eye parameters and the mouse */
void initMouse(void) {
	/* first initialize the eye parameters */
	eye_theta = 45;
	eye_phi = 0;

	eye_fov = 45;

	/* set the eye origin to (0,0,0) */
	eye_pos[0] = 0;
	eye_pos[1] = 0;
	eye_pos[2] = 5;

	computeUpSightAndRightVectors();

	/* set the glut callback functions */
    glutMouseFunc(mouseInput);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);

	return;
}

/* functions to move the viewpoint around */
void moveForward(void) {
	/* update the x and y components of the eye position 
	   moving in the direction of the sight vector */
	eye_pos[0] += sight_vector[0] * speed;
	eye_pos[1] += sight_vector[1] * speed;
}

void moveLeft(void) {
	/* update the x and y components of the eye position 
	   moving in the opposite direction of the right vector */
	eye_pos[0] += -right_vector[0] * speed;
	eye_pos[1] += -right_vector[1] * speed;
}

void moveRight(void) {
	/* update the x and y components of the eye position 
	   moving in the direction of the right vector */
	eye_pos[0] += right_vector[0] * speed;
	eye_pos[1] += right_vector[1] * speed;
}

void moveBack(void) {
/* update the x and y components of the eye position 
	   moving in the opposite direction of the sight vector */
	eye_pos[0] -= sight_vector[0] * speed;
	eye_pos[1] -= sight_vector[1] * speed;
}
