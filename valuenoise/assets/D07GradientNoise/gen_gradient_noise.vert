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

void main()
{
    gl_Position = vec4(aVertexPosition.xy, 0.0, 1.0);
    vTextureCoordinates = aVertexTextureCoordinates;
}
