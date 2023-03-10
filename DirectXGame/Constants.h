#pragma once
#include "VectorMath.h"

struct DirectionalLight
{
    Vector4 dirEye; //NOTE: Direction towards the light
    Vector4 color;
};

struct PointLight
{
    Vector4 posEye;
    Vector4 color;
};

// Create Constant Buffer for our light vertex shader
struct LightVSConstants
{
    Matrix4 modelViewProj;
    Vector4 color;
};

// Create Constant Buffer for our Blinn-Phong pixel shader
struct BlinnPhongPSConstants
{
    DirectionalLight dirLight;
    PointLight pointLights[2];

};

// Create Constant Buffer for our Blinn-Phong vertex shader
struct BlinnPhongVSConstants
{
    Matrix4 modelViewProj;
    Matrix4 modelView;
    Matrix3 normalMatrix;
    Vector4 color;
};