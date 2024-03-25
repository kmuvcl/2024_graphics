#version 120                  // GLSL 1.20

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_normal;

uniform mat4 u_PVM;

// for phong shading
// TODO define uniform variables

varying	  vec3 v_color;

vec3 directional_light()
{
	vec3 color = vec3(0.0);

	// TODO compute phong reflection model
	
		
	return color;
}

void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);

  v_color = directional_light();
}