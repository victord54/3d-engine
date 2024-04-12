#include <iostream>
#include <filesystem>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <string>

#include "tgaimage.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "camera.hpp"

#include "engine.hpp"

#define WIDTH 800
#define HEIGHT 800

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor &color)
{
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

int main(int argc, char const *argv[])
{
    const TGAColor white = TGAColor(255, 255, 255, 255);
    const TGAColor black = TGAColor(0, 0, 0, 255);
    const TGAColor red = TGAColor(255, 0, 0, 255);
    const TGAColor green = TGAColor(0, 255, 0, 255);
    const TGAColor blue = TGAColor(0, 0, 255, 255);

    vec3 eye = vec3(0, 0, 2.1);
    vec3 lookat = vec3(0, 0, 0);
    double fov = 90, near = 0.1, far = 1000;

    Engine engine(WIDTH, HEIGHT, Camera(eye, lookat, fov, near, far));
    engine.addModel("obj/african_head/african_head.obj");

    const int width = WIDTH - 1;
    const int height = HEIGHT - 1;

    engine.model.set_diffusemap("obj/african_head/african_head_diffuse.tga");
    engine.model.set_normalmap("obj/african_head/african_head_nm.tga");
    engine.model.set_specularmap("obj/african_head/african_head_spec.tga");

    engine.setLight(vec3(0, 0, 1));

    int angle = 0;
    if (argc > 1)
    {
        angle = std::stoi(argv[1]);
    }

    // Transformation matrix
    mat4 T = translate(vec3(0, 0, 0));
    mat4 S = scale(vec3(1, 1, 1));
    mat4 R = rotate(vec3(0, angle, 0));
    mat4 M = T * S * R;

    engine.model.M = M;
    engine.draw();
    // Create out folder if it doesn't exist
    std::filesystem::create_directory("out");
    engine.frameBuffer.flip_vertically();
    std::string out_file;
    if (argc > 1)
    {
        // out_file = "out/output_" + std::to_string(angle) + ".tga";
        out_file = "out/output.tga";
    }
    else
    {
        out_file = "out/output.tga";
    }
    engine.frameBuffer.write_tga_file(out_file.c_str());
    return 0;
}
