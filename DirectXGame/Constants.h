#pragma once
#include "VectorMath.h"

struct DirectionalLight
{
    float4 dirEye; //NOTE: Direction towards the light
    float4 color;
};

struct PointLight
{
    float4 posEye;
    float4 color;
};

// Create Constant Buffer for our light vertex shader
struct LightVSConstants
{
    float4x4 modelViewProj;
    float4 color;
};

// Create Constant Buffer for our Blinn-Phong pixel shader
struct BlinnPhongPSConstants
{
    DirectionalLight dirLight;
    PointLight pointLights[2];
    float4 objectColor;
    bool useColor;
    float3 padding;
};

// Create Constant Buffer for our Blinn-Phong vertex shader
struct BlinnPhongVSConstants
{
    float4x4 modelViewProj;
    float4x4 modelView;
    float3x3 normalMatrix;
};