# CPU Implementation of Real-time Graphics Pipeline
Implemented all of the main components of the OpenGL pipeline in software using C++ to create a 3D maze game.

<img width="382" alt="cpurenderer" src="https://user-images.githubusercontent.com/12981474/31321731-e2083156-ac3e-11e7-85f5-da442a9685a0.png">

## Features
* Vertex shader - Applies MVP transform
* Clipping
* Edge-walking rasterization/interpolation
* Bilinear texture interpolation
* Depth/z buffer
* Stencil Buffer - Press i to view a representation of the stencil buffer, which counts the number of times a pixel is drawn to
* Lighting - This is accomplished through the LightShader which is used to render the walls
* Fragment Shaders - Shader.h contains a few shaders using classes. Shaders should be assigned when creating triangles. 
  * Shader performs a basic texture lookup
  * TileShader creates a checkered pattern
  * LightShader applies a light source at (0,0,7), which has ambient, diffuse, and specular components which are attentuated by distance. There are some glitches with specular lighting around the edges of triangles, I think this is caused by the way I am calculating normals and interpolate the model positions of vertices.

## Controls:
* Mouse to look around
* WASD to move
* Space to toggle between OpenGL and CPU implementation
* i to view stencil buffer
* b to toggle between bilinear texture interpolation and nearest
