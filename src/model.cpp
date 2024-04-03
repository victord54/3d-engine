#include <iostream>
#include "model.hpp"

Model::Model(const std::string filename)
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            vec3 v;
            for (int i = 0; i < 3; i++)
            {
                iss >> v[i];
            }
            vertices_.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash)
            {
                idx--;
                f.push_back(idx);
            }
            faces_.push_back(f);
        }
    }
}

int Model::nverts()
{
    return vertices_.size();
}

int Model::nfaces()
{
    return faces_.size();
}

vec3 Model::vert(int i)
{
    return vertices_[i];
}

std::vector<int> Model::face(int idx)
{
    return faces_[idx];
}