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

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: Pixel Program Entry point
//-----------------------------------------------------------------------------
in	vec4	oColor_0;
out vec4 fragColour;
void main(void) {
	vec4	lLocalParam_0;

	FFP_Construct(0.0, 0.0, 0.0, 0.0, lLocalParam_0);

	FFP_Assign(oColor_0, fragColour);

	FFP_Add(fragColour.xyz, lLocalParam_0.xyz, fragColour.xyz);

}

