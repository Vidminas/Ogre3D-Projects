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
void FFP_Modulate(in vec4, in vec4, out vec4);
void FFP_SampleTexture(in sampler2D, in vec2, out vec4);
void SGX_FetchNormal(in sampler2D, in vec2, out vec4);

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	sampler2D	gNormalMapSampler1;
uniform	vec4	derived_ambient_light_colour;
uniform	vec4	surface_diffuse_colour;
uniform	vec4	surface_specular_colour;
uniform	vec4	surface_emissive_colour;
uniform	vec4	derived_scene_colour;
uniform	float	surface_shininess;
uniform	sampler2D	gTextureSampler0;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Pixel Program Entry point
//-----------------------------------------------------------------------------
in	vec2	oTexcoord2_0;
in	vec3	oTexcoord3_1;
out vec4 fragColour;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	vec4	lLocalParam_2;
	vec4	lNormalMapDiffuse;
	vec4	lNormalMapSpecular;
	vec4	texel_0;
	vec4	source1;
	vec4	source2;

	FFP_Construct(1.0, 1.0, 1.0, 1.0, lLocalParam_0);

	FFP_Construct(0.0, 0.0, 0.0, 0.0, lLocalParam_1);

	FFP_Assign(lLocalParam_0, fragColour);

	SGX_FetchNormal(gNormalMapSampler1, oTexcoord2_0, lLocalParam_2);

	FFP_Assign(derived_scene_colour, lNormalMapDiffuse);

	FFP_Assign(lLocalParam_1, lNormalMapSpecular);

	FFP_Assign(lNormalMapDiffuse, lLocalParam_0);

	FFP_Assign(lLocalParam_0, fragColour);

	FFP_Assign(lNormalMapSpecular, lLocalParam_1);

	FFP_SampleTexture(gTextureSampler0, oTexcoord2_0, texel_0);

	FFP_Assign(texel_0, source1);

	FFP_Assign(lLocalParam_0, source2);

	FFP_Modulate(source1, source2, fragColour);

	FFP_Add(fragColour.xyz, lLocalParam_1.xyz, fragColour.xyz);

}

