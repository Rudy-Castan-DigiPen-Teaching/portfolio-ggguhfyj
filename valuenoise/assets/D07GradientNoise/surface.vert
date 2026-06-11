#version 300 es
precision highp float;

/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 vTextureCoordinates;

uniform sampler2D uTex2d;
uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform float uTileScale;

void main()
{
    vTextureCoordinates = aVertexTextureCoordinates * uTileScale;
    float height = texture(uTex2d, vTextureCoordinates).r - 0.5;
    vec3 displacedPosition = vec3(aVertexPosition.xy, height);
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(displacedPosition, 1.0);
}
