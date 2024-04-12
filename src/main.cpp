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

vec3 barycentric(vec2 p0, vec2 p1, vec2 p2, vec2 p)
{
    vec3 u = cross(vec3(p2.x - p0.x, p1.x - p0.x, p0.x - p.x), vec3(p2.y - p0.y, p1.y - p0.y, p0.y - p.y));
    if (std::abs(u.z) < 1)
    {
        return vec3(-1, 1, 1);
    }
    return vec3(1.0 - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void fill_triangle(std::vector<vec3> vertices, std::vector<vec4> world_normals, std::vector<vec4> world_textures, double *zbuffer, TGAImage &image, Model &model, vec3 light_dir, mat4 M)
{
    vec3 p0 = vertices[0];
    vec3 p1 = vertices[1];
    vec3 p2 = vertices[2];

    vec4 wn0 = world_normals[0];
    vec4 wn1 = world_normals[1];
    vec4 wn2 = world_normals[2];

    vec4 wt0 = world_textures[0];
    vec4 wt1 = world_textures[1];
    vec4 wt2 = world_textures[2];

    TGAImage &texture = model.diffusemap_;
    TGAImage &normal_map = model.normalmap_;
    TGAImage &specular_map = model.specularmap_;

    // Boundig box
    int minX = std::min(p0.x, std::min(p1.x, p2.x));
    int minY = std::min(p0.y, std::min(p1.y, p2.y));
    int maxX = std::max(p0.x, std::max(p1.x, p2.x));
    int maxY = std::max(p0.y, std::max(p1.y, p2.y));

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            TGAColor p_color = TGAColor(255, 255, 255, 255);
            vec3 p = vec3(x, y, 0);
            vec3 bc = barycentric(vec2(p0.x, p0.y), vec2(p1.x, p1.y), vec2(p2.x, p2.y), vec2(p.x, p.y));
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;

            // Z-buffer
            p.z = p0.z * bc.x + p1.z * bc.y + p2.z * bc.z;
            int idx = x + y * WIDTH;
            if (zbuffer[idx] > p.z)
            {
                zbuffer[idx] = p.z;

                // Goroud shading
                vec3 normal = normalize(vec3(wn0.x * bc.x + wn1.x * bc.y + wn2.x * bc.z,
                                             wn0.y * bc.x + wn1.y * bc.y + wn2.y * bc.z,
                                             wn0.z * bc.x + wn1.z * bc.y + wn2.z * bc.z));
                // double intensity = dot(normal, light_dir);

                // UV mapping
                vec3 uv = vec3(wt0.x * bc.x + wt1.x * bc.y + wt2.x * bc.z,
                               wt0.y * bc.x + wt1.y * bc.y + wt2.y * bc.z,
                               wt0.z * bc.x + wt1.z * bc.y + wt2.z * bc.z);

                // Normal mapping
                int tx = uv.x * normal_map.get_width();
                int ty = uv.y * normal_map.get_height();
                TGAColor n_color = normal_map.get(tx, ty);
                vec3 n = vec3(n_color.r / 255.0 * 2 - 1,
                              n_color.g / 255.0 * 2 - 1,
                              n_color.b / 255.0 * 2 - 1);
                // Transform normal to world space
                vec4 n4 = vec4(n.x, n.y, n.z, 1);
                vec4 n4_ = M * n4;
                n = vec3(n4_.x, n4_.y, n4_.z);
                double intensity = std::max(dot(normalize(n), light_dir), 0.);

                // // Texture mapping
                tx = uv.x * texture.get_width();
                ty = uv.y * texture.get_height();
                p_color = texture.get(tx, ty);

                // Specular mapping
                vec3 r = normalize(2 * n * dot(n, light_dir) - light_dir);
                double spec = std::pow(std::max(r.z, 0.0), specular_map.get(tx, ty).raw[0]);
                // double spec = 0;
                // std::clog << "specular map: " << specular_map.get(tx, ty).raw[1] << std::endl;

                // Apply shading to color
                // p_color = p_color * intensity;
                p_color.r *= (intensity + 0.6 * spec);
                p_color.g *= (intensity + 0.6 * spec);
                p_color.b *= (intensity + 0.6 * spec);

                // Apply ambient light
                int ambient = 5;

                p_color.r = std::min(255, std::max(0, (int)p_color.r) + ambient);
                p_color.g = std::min(255, std::max(0, (int)p_color.g) + ambient);
                p_color.b = std::min(255, std::max(0, (int)p_color.b) + ambient);

                image.set(x, y, p_color);
            }
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

    int angle = 0;
    if (argc > 1)
    {
        angle = std::stoi(argv[1]);
    }

    const int width = WIDTH - 1;
    const int height = HEIGHT - 1;

    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    Model model("obj/african_head/african_head.obj");

    model.set_diffusemap("obj/african_head/african_head_diffuse.tga");
    model.set_normalmap("obj/african_head/african_head_nm.tga");
    model.set_specularmap("obj/african_head/african_head_spec.tga");
    vec3 eye = vec3(0, 0, 2.1);
    vec3 lookat = vec3(0, 0, 0);
    double fov = 90, near = 0.1, far = 1000;
    Camera camera(eye, lookat, fov, near, far);

    vec3 light_dir = normalize(vec3(0, 0.2, 1));

    // Transformation matrix
    mat4 T = translate(vec3(0, 0, 0));
    mat4 S = scale(vec3(1, 1, 1));
    mat4 R = rotate(vec3(0, angle, 0));
    mat4 M = T * S * R;

    double zbuffer[WIDTH * HEIGHT];

    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        zbuffer[i] = std::numeric_limits<double>::max();
    }
#pragma omp parallel
    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<int> face = model.face(i);
        std::vector<int> faceNormal = model.faceNormal(i);
        std::vector<int> faceTexture = model.faceTexture(i);

        vec3 mp0 = model.vert(face[0]);
        vec3 mp1 = model.vert(face[1]);
        vec3 mp2 = model.vert(face[2]);

        vec3 mn0 = model.normal(faceNormal[0]);
        vec3 mn1 = model.normal(faceNormal[1]);
        vec3 mn2 = model.normal(faceNormal[2]);

        vec3 mt0 = model.texture(faceTexture[0]);
        vec3 mt1 = model.texture(faceTexture[1]);
        vec3 mt2 = model.texture(faceTexture[2]);
        // Convert to homogeneous coordinates
        vec4 wt0 = vec4(mt0.x, mt0.y, mt0.z, 1);
        vec4 wt1 = vec4(mt1.x, mt1.y, mt1.z, 1);
        vec4 wt2 = vec4(mt2.x, mt2.y, mt2.z, 1);

        // Apply transformation matrix to world coordinates
        vec4 wp0 = M * vec4(mp0.x, mp0.y, mp0.z, 1);
        vec4 wp1 = M * vec4(mp1.x, mp1.y, mp1.z, 1);
        vec4 wp2 = M * vec4(mp2.x, mp2.y, mp2.z, 1);

        vec4 wn0 = M * vec4(mn0.x, mn0.y, mn0.z, 1);
        vec4 wn1 = M * vec4(mn1.x, mn1.y, mn1.z, 1);
        vec4 wn2 = M * vec4(mn2.x, mn2.y, mn2.z, 1);

        // std::clog << "World vertices " << wp0 << ", " << wp1 << ", " << wp2 << std::endl;

        // Apply camera view matrix
        vec4 cp0 = camera.viewMatrix() * wp0;
        vec4 cp1 = camera.viewMatrix() * wp1;
        vec4 cp2 = camera.viewMatrix() * wp2;

        // Apply camera projection matrix
        vec4 pp0 = camera.projectionMatrix() * cp0;
        vec4 pp1 = camera.projectionMatrix() * cp1;
        vec4 pp2 = camera.projectionMatrix() * cp2;

        // Perspective divide
        vec3 p0 = camera.perspectiveDivide(pp0);
        vec3 p1 = camera.perspectiveDivide(pp1);
        vec3 p2 = camera.perspectiveDivide(pp2);

        // Convert to screen coordinates
        p0.x = (p0.x + 1) * width / 2;
        p0.y = (p0.y + 1) * height / 2;
        p1.x = (p1.x + 1) * width / 2;
        p1.y = (p1.y + 1) * height / 2;
        p2.x = (p2.x + 1) * width / 2;
        p2.y = (p2.y + 1) * height / 2;

        std::vector<vec3> vertices = {p0, p1, p2};
        std::vector<vec4> world_vertices = {wp0, wp1, wp2};
        std::vector<vec4> world_normals = {wn0, wn1, wn2};
        std::vector<vec4> world_textures = {wt0, wt1, wt2};

        // Draw triangle
        fill_triangle(vertices, world_normals, world_textures, zbuffer, image, model, light_dir, M);
    }

    // Create out folder if it doesn't exist
    std::filesystem::create_directory("out");
    image.flip_vertically();
    std::string out_file;
    if (argc > 1)
    {
        out_file = "out/output_" + std::to_string(angle) + ".tga";
    }
    else
    {
        out_file = "out/output.tga";
    }
    image.write_tga_file(out_file.c_str());
    return 0;
}
