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

int main(int argc, char const *argv[])
{
    // Define the angle of the model by passing it as an argument to the program
    int angle = 0;
    if (argc > 1)
    {
        angle = std::stoi(argv[1]);
    }

    // Camera parameters
    vec3 eye = vec3(0, 0, 2.1);
    vec3 lookat = vec3(0, 0, 0);
    double fov = 90, near = 0.1, far = 1000;
    Camera camera(eye, lookat, fov, near, far);

    // Create the engine
    Engine engine(WIDTH, HEIGHT, camera);
    engine.addModel("obj/african_head/african_head.obj");

    // Set the textures for the model
    engine.model.set_diffusemap("obj/african_head/african_head_diffuse.tga");
    engine.model.set_normalmap("obj/african_head/african_head_nm.tga");
    engine.model.set_specularmap("obj/african_head/african_head_spec.tga");

    // Set the light
    engine.setLight(vec3(0, 0, 1));

    // Transformation matrix
    mat4 T = translate(vec3(0, 0, 0));
    mat4 S = scale(vec3(1, 1, 1));
    mat4 R = rotate(vec3(0, angle, 0));
    mat4 M = T * S * R;

    // Draw the model after applying the transformation matrix
    engine.model.M = M;
    engine.draw(RenderMode::FULL);

    // Save the output image
    if (argc >= 2)
    {
        engine.save("output_" + std::to_string(angle) + ".tga");
    }
    else
    {
        engine.save("output.tga");
    }
    return 0;
}
