/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "IDemo.hpp"
#include "assets/Reloader.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "opengl/FrameBuffer.hpp"
#include "opengl/Shader.hpp"

namespace demos
{
    class D07GradientNoise : public IDemo
    {
    public:
        D07GradientNoise();

        void Update() override;
        void Draw() const override;
        void ImGuiDraw() override;
        void SetDisplaySize(int width, int height) override;

    private:
        void setupNoiseFrameBuffer();
        void buildSurfaceMesh();
        void updateSpectatorCamera();

    private:
        assets::Reloader    assetReloader;
        opengl::FrameBuffer noiseFrameBuffer;
        opengl::Shader      generateGradientNoiseShader;
        opengl::Shader      displayTextureShader;
        opengl::Shader      surfaceShader;
        graphics::SubMesh   quadMesh;
        graphics::SubMesh   surfaceMesh;
        glm::mat4           projectionMatrix{ 1.0f };
        graphics::Camera    camera;
        glm::mat4           orthoProjectionMatrix{ 1.0f };
        int                 textureSize = 1024;
        float               tileScale{ 1.0f };
        float               targetTileScale{ 0.25f };
        float               targetZ = 0.0f;
        float               z       = 0.0f;

        struct ViewNoise
        {
            enum Type
            {
                Texture,
                Surface
            };
        };

        ViewNoise::Type viewNoise = ViewNoise::Surface;

        struct Pattern
        {
            enum Type
            {
                PlainGradient,
                FractalSum,
                Turbulence,
                Marble,
                Wood,
            };
        };

        Pattern::Type pattern = Pattern::PlainGradient;

        struct
        {
            int x = 0, y = 0, width = 0, height = 0;
        } viewport;

        struct
        {
            bool      IsLookingAround = false;
            glm::vec3 MoveDirection{ 0.0f };
            float     Yaw              = 0.0f;
            float     Pitch            = 0.0f;
            float     radiansPerSecond = 4.0f * 3.1415f;
            float     unitsPerSecond   = 2.5f;
        } spectatorCamera;

        int   stacks       = 200;
        int   slices       = 200;
        float surfaceScale = 10.0f;
        float heightScale  = 1.0f;
    };
}
