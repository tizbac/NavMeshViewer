#version 120

#extension GL_EXT_geometry_shader4 : enable

varying out vec3 dist;
void main(void)
{
vec2 WIN_SCALE = vec2(640.0,480.0);
  vec2 p0 = WIN_SCALE * gl_PositionIn[0].xy/gl_PositionIn[0].w;
  vec2 p1 = WIN_SCALE * gl_PositionIn[1].xy/gl_PositionIn[1].w;
  vec2 p2 = WIN_SCALE * gl_PositionIn[2].xy/gl_PositionIn[2].w;
  
  vec2 v0 = p2-p1;
  vec2 v1 = p2-p0;
  vec2 v2 = p1-p0;
  float area = abs(v1.x*v2.y - v1.y * v2.x);

  dist = vec3(area/length(v0),0,0);
  gl_Position = gl_PositionIn[0];
  EmitVertex();
	
  dist = vec3(0,area/length(v1),0);
  gl_Position = gl_PositionIn[1];
  EmitVertex();

  dist = vec3(0,0,area/length(v2));
  gl_Position = gl_PositionIn[2];
  EmitVertex();

  EndPrimitive();
}
