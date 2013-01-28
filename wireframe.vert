#version 120
uniform vec3 lightPosition;
varying vec3 normal, lightDir, eyeVec;
void main() {
normal = gl_NormalMatrix * gl_Normal;
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

	lightDir = vec3(lightPosition - vVertex);
	eyeVec = -vVertex;

	gl_Position = ftransform();
	
}
