#ifndef MOUSE_H
#define MOUSE_H

void reshape(int w, int h);
void computeModelViewMatrix(void);
void computeProjectionMatrix(void);
void initMouse(void);

void debugDrawEyeCoordinateSystem(void);


/* to move the user around the scene */
void moveForward(void);
void moveLeft(void);
void moveRight(void);
void moveBack(void);

#endif		/* MOUSE_H */