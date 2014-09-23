#version 150


uniform mat4 MVP;

in vec3 inVert;
in vec2 inUV;


void main(void)
{
  gl_Position = MVP*vec4(inVert,1.0);
}
