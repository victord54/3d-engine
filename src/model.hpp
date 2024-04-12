#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "geometry.hpp"
#include "tgaimage.hpp"

struct Model
{
    std::vector<vec3> vertices_;
    std::vector<std::vector<int>> faces_;
    std::vector<vec3> normals_;
    std::vector<std::vector<int>> faceNormals_;
    std::vector<vec3> textures_;
    std::vector<std::vector<int>> faceTextures_;

    TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;

    mat4 M;

    Model() {}
    Model(const std::string filename);

    int nverts();
    int nfaces();
    vec3 vert(int i);
    std::vector<int> face(int idx);
    vec3 normal(int i);
    std::vector<int> faceNormal(int idx);
    vec3 texture(int i);
    std::vector<int> faceTexture(int idx);

    void set_diffusemap(const std::string filename);
    void set_normalmap(const std::string filename);
    void set_specularmap(const std::string filename);

    TGAColor diffuse(const vec2 &uv);
    vec3 normalmap(const vec2 &uv);
    double specular(const vec2 &uv);
};
