#version 450
//-----------------------------------------------------------------------------
// Program Type: Vertex shader
// Language: glsl
// Created by Ogre RT Shader Generator. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                         FORWARD DECLARATIONS
//-----------------------------------------------------------------------------
void FFP_Assign(in vec3, out vec3);
void FFP_Assign(in vec4, out vec4);
void FFP_Construct(in float, in float, in float, in float, out vec4);
void FFP_Transform(in mat4, in vec4, out vec4);
void SGX_TransformNormal(in mat4, in vec3, out vec3);

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	worldviewproj_matrix;
uniform	mat4	inverse_transpose_worldview_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Vertex Program Entry point
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec3	normal;
in	vec4	colour;
out	vec3	oTexcoord3_0;
out	vec4	oTexcoord4_1;
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

	SGX_TransformNormal(inverse_transpose_worldview_matrix, normal, oTexcoord3_0);

	FFP_Assign(normal, oTexcoord3_0);

	FFP_Assign(vertex, oTexcoord4_1);

}

