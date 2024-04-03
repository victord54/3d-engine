#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "geometry.hpp"

struct Model
{
    std::vector<vec3> vertices_;
    std::vector<std::vector<int>> faces_;

    Model(const std::string filename);

    int nverts();
    int nfaces();
    vec3 vert(int i);
    std::vector<int> face(int idx);
};
