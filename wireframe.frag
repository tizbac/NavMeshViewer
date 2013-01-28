#version 120
//#extension GL_EXT_gpu_shader4 : enable

varying vec3 dist;
const vec4 WIRE_COL = vec4(0,0.0,0.0,1);
varying vec3 normal, lightDir, eyeVec; 

void main(void)
{ 

	float d = min(dist[0],min(dist[1],dist[2]));
 	float I = exp2(-2*d*d);
 	gl_FragColor = I*WIRE_COL + (1.0 - I)*gl_FrontMaterial.diffuse;
}
