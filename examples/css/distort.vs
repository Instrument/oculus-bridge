precision mediump float;

attribute vec4 a_position;

uniform mat4 u_projectionMatrix;
uniform float lensShift;
//uniform float scale

void main() {

	vec2 scaleIn = vec2(1.2, 1.225);
	vec2 lensCenter = vec2(lensShift / 2.0, 0.0);

	vec2 uv = vec2(a_position.x, a_position.y);//range from [0,1] to [-1,1]
	vec2 theta = (uv - lensCenter) * scaleIn;

	vec4 hmdWarpParam = vec4(1.0, 0.22, 0.24, 0.0);

	
	float rSq = theta.x * theta.x + theta.y * theta.y;
	vec2 rvector = theta/(hmdWarpParam.x + hmdWarpParam.y*rSq + hmdWarpParam.z*rSq*rSq + hmdWarpParam.w*rSq*rSq*rSq);

	gl_Position = u_projectionMatrix * vec4(rvector.x, rvector.y, a_position.z, a_position.w);
}