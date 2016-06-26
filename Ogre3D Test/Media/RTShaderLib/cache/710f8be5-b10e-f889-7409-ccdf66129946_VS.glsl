#version 450
//-----------------------------------------------------------------------------
// Program Type: Vertex shader
// Language: glsl
// Created by Ogre RT Shader Generator. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                         FORWARD DECLARATIONS
//-----------------------------------------------------------------------------
void FFP_Assign(in vec2, out vec2);
void FFP_Construct(in float, in float, in float, in float, out vec4);
void FFP_Transform(in mat4, in vec4, out vec4);
void FFP_TransformTexCoord(in mat4, in vec2, out vec2);

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	worldviewproj_matrix;
uniform	mat4	texture_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Vertex Program Entry point
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec4	uv0;
out	vec2	oTexcoord2_0;
out	vec2	oTexcoord2_1;
out gl_PerVertex
{
vec4 gl_Position;
float gl_PointSize;
float gl_ClipDistance[];
};

void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;

	FFP_Transform(worldviewproj_matrix, vertex, gl_Position);

	FFP_Construct(1.0, 1.0, 1.0, 1.0, lLocalParam_0);

	FFP_Construct(0.0, 0.0, 0.0, 0.0, lLocalParam_1);

	FFP_TransformTexCoord(texture_matrix, uv0.xy, oTexcoord2_0);

	FFP_Assign(uv0.xy, oTexcoord2_1);

}

