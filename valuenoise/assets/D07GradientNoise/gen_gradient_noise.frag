#version 300 es
precision highp float;

/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

in vec2 vTextureCoordinates;

layout(location = 0) out float fNoise;

uniform float uZ;
uniform int uPattern;

const int PERIOD = 64;
const float PERIOD_F = 64.0;
const float PI = 3.1415926535897932384626433832795;

int permutation(int index)
{
    const int values[128] = int[128](
        17, 33, 14, 52, 24, 36, 46, 48, 26, 7, 45, 57, 59, 2, 42, 61, 9, 3, 12, 63, 37, 53, 17, 8, 4, 35, 38, 22, 6, 18, 68, 55,
        31, 11, 21, 5, 27, 25, 35, 28, 32, 45, 41, 10, 71, 58, 62, 11, 13, 29, 34, 50, 8, 1, 29, 10, 16, 4, 15, 56, 18, 39, 56, 61,
        17, 33, 14, 52, 24, 36, 46, 48, 26, 7, 45, 57, 59, 2, 42, 61, 9, 3, 12, 63, 37, 53, 17, 8, 4, 35, 38, 22, 6, 18, 68, 55,
        31, 11, 21, 5, 27, 25, 35, 28, 32, 45, 41, 10, 71, 58, 62, 11, 13, 29, 34, 50, 8, 1, 29, 10, 16, 4, 15, 56, 18, 39, 56, 61);
    return values[index & 127];
}

int positive_mod(int value, int modulus)
{
    int result = value % modulus;
    return result < 0 ? result + modulus : result;
}

float fade(float t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float gradient_dot(int hash, vec3 offset)
{
    const vec3 gradients[12] = vec3[12](
        vec3(1.0, 1.0, 0.0), vec3(-1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0), vec3(-1.0, -1.0, 0.0),
        vec3(1.0, 0.0, 1.0), vec3(-1.0, 0.0, 1.0), vec3(1.0, 0.0, -1.0), vec3(-1.0, 0.0, -1.0),
        vec3(0.0, 1.0, 1.0), vec3(0.0, -1.0, 1.0), vec3(0.0, 1.0, -1.0), vec3(0.0, -1.0, -1.0));
    return dot(normalize(gradients[hash % 12]), offset);
}

int hash_lattice(ivec3 cell)
{
    const int x = positive_mod(cell.x, PERIOD);
    const int y = positive_mod(cell.y, PERIOD);
    const int z = positive_mod(cell.z, PERIOD);
    return permutation(permutation(permutation(x) + y) + z);
}

float gradient_noise(vec3 point)
{
    ivec3 base = ivec3(floor(point));
    vec3 local = point - vec3(base);
    vec3 faded = vec3(fade(local.x), fade(local.y), fade(local.z));

    float x00 = mix(gradient_dot(hash_lattice(base + ivec3(0, 0, 0)), local - vec3(0.0, 0.0, 0.0)),
                    gradient_dot(hash_lattice(base + ivec3(1, 0, 0)), local - vec3(1.0, 0.0, 0.0)), faded.x);
    float x10 = mix(gradient_dot(hash_lattice(base + ivec3(0, 1, 0)), local - vec3(0.0, 1.0, 0.0)),
                    gradient_dot(hash_lattice(base + ivec3(1, 1, 0)), local - vec3(1.0, 1.0, 0.0)), faded.x);
    float x01 = mix(gradient_dot(hash_lattice(base + ivec3(0, 0, 1)), local - vec3(0.0, 0.0, 1.0)),
                    gradient_dot(hash_lattice(base + ivec3(1, 0, 1)), local - vec3(1.0, 0.0, 1.0)), faded.x);
    float x11 = mix(gradient_dot(hash_lattice(base + ivec3(0, 1, 1)), local - vec3(0.0, 1.0, 1.0)),
                    gradient_dot(hash_lattice(base + ivec3(1, 1, 1)), local - vec3(1.0, 1.0, 1.0)), faded.x);

    float y0 = mix(x00, x10, faded.y);
    float y1 = mix(x01, x11, faded.y);
    return mix(y0, y1, faded.z);
}

float normalized_noise(vec3 point)
{
    return clamp(0.5 + 0.5 * gradient_noise(point), 0.0, 1.0);
}

float fractal_sum(vec3 point)
{
    float result = 0.0;
    float amplitude = 0.5;
    float totalAmplitude = 0.0;
    for (int layer = 0; layer < 5; ++layer)
    {
        result += amplitude * normalized_noise(point);
        totalAmplitude += amplitude;
        point *= 2.0;
        amplitude *= 0.5;
    }
    return result / totalAmplitude;
}

float turbulence(vec3 point)
{
    float result = 0.0;
    float amplitude = 0.5;
    float totalAmplitude = 0.0;
    for (int layer = 0; layer < 5; ++layer)
    {
        result += amplitude * abs(gradient_noise(point));
        totalAmplitude += amplitude;
        point *= 2.0;
        amplitude *= 0.5;
    }
    return clamp(result / totalAmplitude * 2.0, 0.0, 1.0);
}

float marble(vec3 point)
{
    float n = fractal_sum(point);
    return 0.5 + 0.5 * sin((point.x + n * 3.0) * PI);
}

float wood(vec3 point)
{
    float rings = length(point.xy) + 0.25 * fractal_sum(point * 1.75);
    return fract(rings * 8.0);
}

void main()
{
    vec3 point = vec3(vTextureCoordinates * PERIOD_F, uZ);
    if (uPattern == 1)
    {
        fNoise = fractal_sum(point);
    }
    else if (uPattern == 2)
    {
        fNoise = turbulence(point);
    }
    else if (uPattern == 3)
    {
        fNoise = marble(point * 0.25);
    }
    else if (uPattern == 4)
    {
        fNoise = wood(point * 0.2);
    }
    else
    {
        fNoise = normalized_noise(point);
    }
}
