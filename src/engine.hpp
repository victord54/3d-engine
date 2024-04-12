#pragma once

#include <vector>
#include <string>

#include "geometry.hpp"
#include "tgaimage.hpp"
#include "model.hpp"
#include "camera.hpp"

enum class RenderMode
{
    WIREFRAME,
    BACKFACE,
    GOURAUD,
    NORMALMAP,
    TEXTURE,
    FULL
};

struct Engine
{
    TGAImage frameBuffer;
    Model model;
    Camera camera;
    vec3 light_dir_;

    double *zBuffer;

    Engine(int width, int height, Camera camera) : camera(camera)
    {
        frameBuffer = TGAImage(width, height, TGAImage::RGB);
        zBuffer = new double[width * height];
        for (int i = 0; i < width * height; i++)
        {
            zBuffer[i] = std::numeric_limits<double>::max();
        }
    }

    void addModel(const std::string filename)
    {
        // models.push_back(Model(filename));
        model = Model(filename);
    }

    void setLight(vec3 light_dir)
    {
        light_dir_ = normalize(light_dir);
    }

    void draw(RenderMode render = RenderMode::FULL)
    {
        for (int i = 0; i < model.nfaces(); i++)
        {
            std::vector<int> face = model.face(i);
            std::vector<int> faceNormal = model.faceNormal(i);
            std::vector<int> faceTexture = model.faceTexture(i);

            vec3 modelPoints[3];
            vec3 modelNormals[3];
            vec3 modelTextures[3];

            for (int j = 0; j < 3; j++)
            {
                modelPoints[j] = model.vert(face[j]);
                modelNormals[j] = model.normal(faceNormal[j]);
                modelTextures[j] = model.texture(faceTexture[j]);
            }

            // Convert to homogeneous coordinates
            vec4 worldTextures[3];
            for (int j = 0; j < 3; j++)
            {
                worldTextures[j] = vec4(modelTextures[j].x, modelTextures[j].y, modelTextures[j].z, 1);
            }

            // Apply transformation matrix to world coordinates
            vec4 worldPoints[3];
            vec4 worldNormals[3];
            for (int j = 0; j < 3; j++)
            {
                worldPoints[j] = model.M * vec4(modelPoints[j].x, modelPoints[j].y, modelPoints[j].z, 1);
                worldNormals[j] = model.M * vec4(modelNormals[j].x, modelNormals[j].y, modelNormals[j].z, 1);
            }

            // Apply camera view matrix
            vec4 cameraPoints[3];
            for (int j = 0; j < 3; j++)
            {
                cameraPoints[j] = camera.viewMatrix() * worldPoints[j];
            }

            // Apply camera projection matrix
            vec4 projectedPoints[3];
            for (int j = 0; j < 3; j++)
            {
                projectedPoints[j] = camera.projectionMatrix() * cameraPoints[j];
            }

            // Perspective divide
            vec3 screenPoints[3];
            for (int j = 0; j < 3; j++)
            {
                screenPoints[j] = camera.perspectiveDivide(projectedPoints[j]);
            }

            // Convert to screen coordinates
            for (int j = 0; j < 3; j++)
            {
                screenPoints[j].x = (screenPoints[j].x + 1) * frameBuffer.get_width() / 2;
                screenPoints[j].y = (screenPoints[j].y + 1) * frameBuffer.get_height() / 2;
            }
            // Draw triangle
            if (render == RenderMode::WIREFRAME)
                drawWireframe(screenPoints);
            // else if (render == RenderMode::BACKFACE)
            //     drawTriangle(screenPoints, worldNormals);
            else if (render == RenderMode::GOURAUD)
                drawTriangleGS(screenPoints, worldNormals);
            else if (render == RenderMode::NORMALMAP)
                drawTriangleNM(screenPoints, worldTextures);
            else if (render == RenderMode::TEXTURE)
                drawTriangleT(screenPoints, worldNormals, worldTextures);
            else if (render == RenderMode::FULL)
                drawTriangleFull(screenPoints, worldTextures);
        }
    }

    void save(std::string filename)
    {
        // Create out folder if it doesn't exist
        std::filesystem::create_directory("out");
        frameBuffer.flip_vertically();
        std::string out_file = "out/" + filename;
        frameBuffer.write_tga_file(out_file.c_str());
    }

private:
    /* Fonctionne */
    void drawTriangleT(vec3 *screenPoints, vec4 *worldNormals, vec4 *worldTextures)
    {
        // Boundig box
        int minX, minY, maxX, maxY;
        boundingBox(screenPoints, &minX, &minY, &maxX, &maxY);

        for (int x = minX; x <= maxX; x++)
        {
            for (int y = minY; y <= maxY; y++)
            {
                TGAColor p_color = TGAColor(255, 255, 255, 255);
                vec3 p = vec3(x, y, 0);
                vec3 bc = barycentric(screenPoints[0], screenPoints[1], screenPoints[2], p);
                if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                    continue;

                // ZBuffer
                p.z = screenPoints[0].z * bc.x + screenPoints[1].z * bc.y + screenPoints[2].z * bc.z;
                int idx = x + y * frameBuffer.get_width();
                if (zBuffer[idx] <= p.z)
                    continue;

                zBuffer[idx] = p.z;

                // Goroud shading
                vec3 normal = normalize(vec3(worldNormals[0].x * bc.x + worldNormals[1].x * bc.y + worldNormals[2].x * bc.z,
                                             worldNormals[0].y * bc.x + worldNormals[1].y * bc.y + worldNormals[2].y * bc.z,
                                             worldNormals[0].z * bc.x + worldNormals[1].z * bc.y + worldNormals[2].z * bc.z));

                // UV mapping
                vec2 uv = vec2(worldTextures[0].x * bc.x + worldTextures[1].x * bc.y + worldTextures[2].x * bc.z,
                               worldTextures[0].y * bc.x + worldTextures[1].y * bc.y + worldTextures[2].y * bc.z);
                double intensity = dot(normal, light_dir_);

                // Texture mapping
                p_color = model.diffuse(uv);

                p_color.r *= intensity;
                p_color.g *= intensity;
                p_color.b *= intensity;

                frameBuffer.set(x, y, p_color);
            }
        }
    }

    /* Fonctionne */
    void drawTriangleGS(vec3 *screenPoints, vec4 *worldNormals)
    {
        // Boundig box
        int minX, minY, maxX, maxY;
        boundingBox(screenPoints, &minX, &minY, &maxX, &maxY);

        for (int x = minX; x <= maxX; x++)
        {
            for (int y = minY; y <= maxY; y++)
            {
                TGAColor p_color = TGAColor(255, 255, 255, 255);
                vec3 p = vec3(x, y, 0);
                vec3 bc = barycentric(screenPoints[0], screenPoints[1], screenPoints[2], p);
                if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                    continue;

                // ZBuffer
                p.z = screenPoints[0].z * bc.x + screenPoints[1].z * bc.y + screenPoints[2].z * bc.z;
                int idx = x + y * frameBuffer.get_width();
                if (zBuffer[idx] <= p.z)
                    continue;

                zBuffer[idx] = p.z;

                // Goroud shading
                vec3 normal = normalize(vec3(worldNormals[0].x * bc.x + worldNormals[1].x * bc.y + worldNormals[2].x * bc.z,
                                             worldNormals[0].y * bc.x + worldNormals[1].y * bc.y + worldNormals[2].y * bc.z,
                                             worldNormals[0].z * bc.x + worldNormals[1].z * bc.y + worldNormals[2].z * bc.z));
                double intensity = dot(normal, light_dir_);

                p_color.r *= intensity;
                p_color.g *= intensity;
                p_color.b *= intensity;

                frameBuffer.set(x, y, p_color);
            }
        }
    }

    /* Fonctionne */
    void drawTriangleFull(vec3 *screenPoints, vec4 *worldTextures)
    {
        // Boundig box
        int minX, minY, maxX, maxY;
        boundingBox(screenPoints, &minX, &minY, &maxX, &maxY);

        for (int x = minX; x <= maxX; x++)
        {
            for (int y = minY; y <= maxY; y++)
            {
                TGAColor p_color = TGAColor(255, 255, 255, 255);
                vec3 p = vec3(x, y, 0);
                vec3 bc = barycentric(screenPoints[0], screenPoints[1], screenPoints[2], p);
                if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                    continue;

                // ZBuffer
                p.z = screenPoints[0].z * bc.x + screenPoints[1].z * bc.y + screenPoints[2].z * bc.z;
                int idx = x + y * frameBuffer.get_width();
                if (zBuffer[idx] <= p.z)
                    continue;

                zBuffer[idx] = p.z;

                // UV mapping
                vec2 uv = vec2(worldTextures[0].x * bc.x + worldTextures[1].x * bc.y + worldTextures[2].x * bc.z,
                               worldTextures[0].y * bc.x + worldTextures[1].y * bc.y + worldTextures[2].y * bc.z);
                vec3 normal = model.normalmap(uv);
                vec4 homogeneous_normal = vec4(normal.x, normal.y, normal.z, 1);
                vec4 world_normal = model.M * homogeneous_normal;
                normal = vec3(world_normal.x, world_normal.y, world_normal.z);

                double intensity = dot(normal, light_dir_);

                // Specular mapping
                vec3 r = normalize(2 * normal * dot(normal, light_dir_) - light_dir_);
                double specular = pow(std::max(r.z, 0.0), model.specular(uv));

                // Texture mapping
                p_color = model.diffuse(uv);

                p_color.r *= (intensity + 0.6 * specular);
                p_color.g *= (intensity + 0.6 * specular);
                p_color.b *= (intensity + 0.6 * specular);

                int ambiant = 5;

                p_color.r = std::min(255, std::max(0, int(p_color.r + ambiant)));
                p_color.g = std::min(255, std::max(0, int(p_color.g + ambiant)));
                p_color.b = std::min(255, std::max(0, int(p_color.b + ambiant)));

                frameBuffer.set(x, y, p_color);
            }
        }
    }

    /* Fonctionne */
    void drawTriangleNM(vec3 *screenPoints, vec4 *worldTextures)
    {
        // Boundig box
        int minX, minY, maxX, maxY;
        boundingBox(screenPoints, &minX, &minY, &maxX, &maxY);

        for (int x = minX; x <= maxX; x++)
        {
            for (int y = minY; y <= maxY; y++)
            {
                TGAColor p_color = TGAColor(255, 255, 255, 255);
                vec3 p = vec3(x, y, 0);
                vec3 bc = barycentric(screenPoints[0], screenPoints[1], screenPoints[2], p);
                if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                    continue;

                // ZBuffer
                p.z = screenPoints[0].z * bc.x + screenPoints[1].z * bc.y + screenPoints[2].z * bc.z;
                int idx = x + y * frameBuffer.get_width();
                if (zBuffer[idx] <= p.z)
                    continue;

                zBuffer[idx] = p.z;

                // UV mapping
                vec2 uv = vec2(worldTextures[0].x * bc.x + worldTextures[1].x * bc.y + worldTextures[2].x * bc.z,
                               worldTextures[0].y * bc.x + worldTextures[1].y * bc.y + worldTextures[2].y * bc.z);
                vec3 normal = model.normalmap(uv);
                vec4 homogeneous_normal = vec4(normal.x, normal.y, normal.z, 1);
                vec4 world_normal = model.M * homogeneous_normal;
                normal = vec3(world_normal.x, world_normal.y, world_normal.z);

                double intensity = dot(normal, light_dir_);

                p_color.r *= intensity;
                p_color.g *= intensity;
                p_color.b *= intensity;

                frameBuffer.set(x, y, p_color);
            }
        }
    }

    /* Fonctionne */
    void drawWireframe(vec3 *screenPoints)
    {
        for (int i = 0; i < 3; i++)
        {
            int j = (i + 1) % 3;
            line(screenPoints[i], screenPoints[j], frameBuffer, TGAColor(255, 255, 255, 255));
        }
    }

    void boundingBox(vec3 *screenPoints, int *minX, int *minY, int *maxX, int *maxY)
    {
        *minX = std::min(screenPoints[0].x, std::min(screenPoints[1].x, screenPoints[2].x));
        *minY = std::min(screenPoints[0].y, std::min(screenPoints[1].y, screenPoints[2].y));
        *maxX = std::max(screenPoints[0].x, std::max(screenPoints[1].x, screenPoints[2].x));
        *maxY = std::max(screenPoints[0].y, std::max(screenPoints[1].y, screenPoints[2].y));
    }

    void line(vec3 &p1, vec3 &p2, TGAImage &image, const TGAColor &color)
    {
        int x0 = p1.x;
        int y0 = p1.y;
        int x1 = p2.x;
        int y1 = p2.y;
        bool steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1))
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx = x1 - x0;
        int dy = y1 - y0;
        int derror2 = std::abs(dy) * 2;
        int error2 = 0;
        int y = y0;
        for (int x = x0; x <= x1; x++)
        {
            if (steep)
            {
                image.set(y, x, color);
            }
            else
            {
                image.set(x, y, color);
            }
            error2 += derror2;
            if (error2 > dx)
            {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx * 2;
            }
        }
    }
};
