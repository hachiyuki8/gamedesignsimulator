#pragma once

#include "GL.hpp"
#include "Load.hpp"

//Shader program that draws transformed, vertices tinted with vertex colors:
struct ColorTextureProgram {
	ColorTextureProgram();
	~ColorTextureProgram();

	GLuint program = 0;
	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	//Uniform (per-invocation variable) locations:
	GLuint PROJECTION_mat4 = -1U;
};

extern Load< ColorTextureProgram > color_texture_program;
