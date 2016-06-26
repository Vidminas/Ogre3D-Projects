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
void FFP_Subtract(in vec3, in vec3, out vec3);
void FFP_Transform(in mat4, in vec4, out vec4);
void SGX_ConstructTBNMatrix(in vec3, in vec3, out mat3);
void SGX_TransformNormal(in mat3, in vec3, out vec3);
void SGX_TransformNormal(in mat4, in vec3, out vec3);
void SGX_TransformPosition(in mat4, in vec4, out vec3);

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	worldviewproj_matrix;
uniform	vec3	camera_position;
uniform	mat4	world_matrix;
uniform	mat4	inv_world_rotation_matrix0;
uniform	vec4	light_position_world_space0;
uniform	vec4	light_position_world_space1;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Vertex Program Entry point
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec3	normal;
in	vec3	tangent;
in	vec4	uv0;
out	vec2	oTexcoord2_0;
out	vec3	oTexcoord3_1;
out	vec3	oTexcoord3_2;
out	vec3	oTexcoord3_3;
out gl_PerVertex
{
vec4 gl_Position;
float gl_PointSize;
float gl_ClipDistance[];
};

void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	mat3	lMatTBN;
	vec3	lNormalMapTempDir;
	vec3	lLocalParam_4;

	FFP_Transform(worldviewproj_matrix, vertex, gl_Position);

	FFP_Construct(1.0, 1.0, 1.0, 1.0, lLocalParam_0);

	FFP_Construct(0.0, 0.0, 0.0, 0.0, lLocalParam_1);

	SGX_ConstructTBNMatrix(normal, tangent, lMatTBN);

	FFP_Assign(uv0.xy, oTexcoord2_0);

	SGX_TransformPosition(world_matrix, vertex, lLocalParam_4);

	FFP_Subtract(camera_position.xyz, lLocalParam_4.xyz, lNormalMapTempDir);

	SGX_TransformNormal(inv_world_rotation_matrix0, lNormalMapTempDir, lNormalMapTempDir);

	SGX_TransformNormal(lMatTBN, lNormalMapTempDir, oTexcoord3_1);

	FFP_Subtract(light_position_world_space0.xyz, lLocalParam_4, lNormalMapTempDir);

	SGX_TransformNormal(inv_world_rotation_matrix0, lNormalMapTempDir, lNormalMapTempDir);

	SGX_TransformNormal(lMatTBN, lNormalMapTempDir, oTexcoord3_2);

	FFP_Subtract(light_position_world_space1.xyz, lLocalParam_4, lNormalMapTempDir);

	SGX_TransformNormal(inv_world_rotation_matrix0, lNormalMapTempDir, lNormalMapTempDir);

	SGX_TransformNormal(lMatTBN, lNormalMapTempDir, oTexcoord3_3);

}

