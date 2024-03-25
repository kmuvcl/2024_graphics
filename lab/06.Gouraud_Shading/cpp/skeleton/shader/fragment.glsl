#version 120                  // GLSL 1.20

varying	  vec3 v_color;

void main()
{
	gl_FragColor = vec4(v_color, 1.0f);
}