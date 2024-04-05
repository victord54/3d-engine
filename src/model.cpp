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
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            vec3 n;
            for (int i = 0; i < 3; i++)
            {
                iss >> n[i];
            }
            normals_.push_back(n);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            std::vector<int> fn;
            int itrash, idx, idxn;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> idxn)
            {
                idx--; // in wavefront obj all indices start at 1, not zero
                idxn--;
                f.push_back(idx);
                fn.push_back(idxn);
            }
            faces_.push_back(f);
            faceNormals_.push_back(fn);
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

vec3 Model::normal(int i)
{
    return normals_[i];
}

std::vector<int> Model::faceNormal(int idx)
{
    return faceNormals_[idx];
}