#include <iostream>

#include "tgaimage.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "camera.hpp"

#define WIDTH 800
#define HEIGHT 800

const TGAColor red = TGAColor(255, 0, 0, 255);

mat4 translate(const vec3 &v)
{
    mat4 T = mat4::identity();
    T[0][3] = v[0];
    T[1][3] = v[1];
    T[2][3] = v[2];
    return T;
}

mat4 scale(const vec3 &v)
{
    mat4 S = mat4::identity();
    S[0][0] = v[0];
    S[1][1] = v[1];
    S[2][2] = v[2];
    return S;
}

mat4 rotate(const vec3 &angles)
{
    // Convert angles to radians
    vec3 angles_ = angles * (M_PI / 180.0);
    mat4 Rx = mat4::identity();
    Rx[1][1] = std::cos(angles_[0]);
    Rx[1][2] = -std::sin(angles_[0]);
    Rx[2][1] = std::sin(angles_[0]);
    Rx[2][2] = std::cos(angles_[0]);

    mat4 Ry = mat4::identity();
    Ry[0][0] = std::cos(angles_[1]);
    Ry[0][2] = std::sin(angles_[1]);
    Ry[2][0] = -std::sin(angles_[1]);
    Ry[2][2] = std::cos(angles_[1]);

    mat4 Rz = mat4::identity();
    Rz[0][0] = std::cos(angles_[2]);
    Rz[0][1] = -std::sin(angles_[2]);
    Rz[1][0] = std::sin(angles_[2]);
    Rz[1][1] = std::cos(angles_[2]);

    return Rz * Ry * Rx;
}

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
    const int width = WIDTH - 1;
    const int height = HEIGHT - 1;
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    Camera camera;
    camera.setView(vec3(0, 0, 0), vec3(0, 0, 1)); // BUG: The camera y origin is inverted in the viewMatrix method of the Camera struct
    Model model("obj/african_head/african_head.obj");
    std::clog << "Model loaded with " << model.nverts() << " vertices and " << model.nfaces() << " faces" << std::endl;

    // Transformation matrix
    mat4 T = translate(vec3(0, 0, 0));
    mat4 S = scale(vec3(1, 1, 1));
    mat4 R = rotate(vec3(0, 0, 0));
    mat4 M = T * S * R;

    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<int> face = model.face(i);
        for (int j = 0; j < 3; j++)
        {
            vec3 mp0 = model.vert(face[j]);
            vec3 mp1 = model.vert(face[(j + 1) % 3]);
            std::clog << "mp0: " << mp0 << " mp1: " << mp1 << std::endl;

            // Transforming the vertices from model space to world space
            vec4 wp0 = M * vec4(mp0.x, mp0.y, mp0.z, 1);
            vec4 wp1 = M * vec4(mp1.x, mp1.y, mp1.z, 1);
            std::clog << "wp0: " << wp0 << " wp1: " << wp1 << std::endl;

            // Transforming the vertices from world space to camera space
            // TODO: Check if the matrix multiplication is correct
            vec4 cp0 = camera.viewMatrix() * wp0;
            vec4 cp1 = camera.viewMatrix() * wp1;
            std::clog << "cp0: " << cp0 << " cp1: " << cp1 << std::endl;

            // TODO: Transforming the vertices from camera space to projection space

            // TODO: Transforming the vertices from projection space to screen space

            // TODO: Drawing the triangle

            vec2 p0 = vec2((cp0.x + 1.0) * width / 2.0, (cp0.y + 1.0) * height / 2.0);
            vec2 p1 = vec2((cp1.x + 1.0) * width / 2.0, (cp1.y + 1.0) * height / 2.0);

            draw_line(p0.x, p0.y, p1.x, p1.y, image, red);
        }
    }

    // Create out folder if it doesn't exist
    std::filesystem::create_directory("out");
    image.flip_vertically();
    image.write_tga_file("out/output.tga");
    return 0;
}
