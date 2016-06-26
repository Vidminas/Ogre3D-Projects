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
void SGX_TriplanarTexturing(in vec4, in vec3, in vec4, in sampler2D, in sampler2D, in sampler2D, in vec3, out vec4);

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	sampler2D	tp_sampler_from_x0;
uniform	sampler2D	tp_sampler_from_y1;
uniform	sampler2D	tp_sampler_from_z2;
uniform	vec3	gTPParams0;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Pixel Program Entry point
//-----------------------------------------------------------------------------
in	vec4	oColor_0;
in	vec3	oTexcoord3_0;
in	vec4	oTexcoord4_1;
out vec4 fragColour;
void main(void) {
	vec4	lLocalParam_0;

	FFP_Construct(0.0, 0.0, 0.0, 0.0, lLocalParam_0);

	FFP_Assign(oColor_0, fragColour);

	SGX_TriplanarTexturing(oColor_0, oTexcoord3_0, oTexcoord4_1, tp_sampler_from_x0, tp_sampler_from_y1, tp_sampler_from_z2, gTPParams0, fragColour);

	FFP_Add(fragColour.xyz, lLocalParam_0.xyz, fragColour.xyz);

}

