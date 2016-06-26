#version 450
//-----------------------------------------------------------------------------
// Program Type: Fragment shader
// Language: glsl
// Created by Ogre RT Shader Generator. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                         FORWARD DECLARATIONS
//-----------------------------------------------------------------------------
void FFP_Add(in vec3, in vec3, out vec3);
void FFP_Assign(in vec4, out vec4);
void FFP_Construct(in float, in float, in float, in float, out vec4);
void SGX_FetchNormal(in sampler2D, in vec2, out vec4);
void SGX_Light_Point_DiffuseSpecular(in vec4, in vec3, in vec3, in vec4, in vec3, in vec3, in float, in vec3, in vec3, out vec3, out vec3);

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	sampler2D	gNormalMapSampler0;
uniform	vec4	derived_ambient_light_colour;
uniform	vec4	surface_diffuse_colour;
uniform	vec4	surface_specular_colour;
uniform	vec4	surface_emissive_colour;
uniform	vec4	derived_scene_colour;
uniform	float	surface_shininess;
uniform	vec4	light_attenuation0;
uniform	vec4	derived_light_diffuse1;
uniform	vec4	derived_light_specular2;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Pixel Program Entry point
//-----------------------------------------------------------------------------
in	vec2	oTexcoord2_0;
in	vec3	oTexcoord3_1;
in	vec3	oTexcoord3_2;
out vec4 fragColour;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	vec4	lLocalParam_2;
	vec4	lNormalMapDiffuse;
	vec4	lNormalMapSpecular;

	FFP_Construct(1.0, 1.0, 1.0, 1.0, lLocalParam_0);

	FFP_Construct(0.0, 0.0, 0.0, 0.0, lLocalParam_1);

	FFP_Assign(lLocalParam_0, fragColour);

	SGX_FetchNormal(gNormalMapSampler0, oTexcoord2_0, lLocalParam_2);

	FFP_Assign(derived_scene_colour, lNormalMapDiffuse);

	FFP_Assign(lLocalParam_1, lNormalMapSpecular);

	SGX_Light_Point_DiffuseSpecular(lLocalParam_2, oTexcoord3_1, oTexcoord3_2.xyz, light_attenuation0, derived_light_diffuse1.xyz, derived_light_specular2.xyz, surface_shininess, lNormalMapDiffuse.xyz, lNormalMapSpecular.xyz, lNormalMapDiffuse.xyz, lNormalMapSpecular.xyz);

	FFP_Assign(lNormalMapDiffuse, lLocalParam_0);

	FFP_Assign(lLocalParam_0, fragColour);

	FFP_Assign(lNormalMapSpecular, lLocalParam_1);

	FFP_Add(fragColour.xyz, lLocalParam_1.xyz, fragColour.xyz);

}

