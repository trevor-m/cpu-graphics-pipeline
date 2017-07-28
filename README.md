# CPU Implementation of Real-time Graphics Pipeline

Controls:
* Mouse to look around
* WASD to move
* Space to toggle between OpenGL and CPU implementation
* i to view stencil buffer
* b to toggle between bilinear texture interpolation and nearest

Features:
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
  * LightShader applies a light source at (0,0,7), which has ambient, diffuse, and specular components which are attentuated by distance. There is some glitchiness with specular lighting around the edges of triangles, I think this is caused by the way I am calculating normals and interpolate the model positions of vertices.


Trevor