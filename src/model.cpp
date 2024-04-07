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
        if (!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            vec3 vt;
            for (int i = 0; i < 3; i++)
            {
                iss >> vt[i];
            }
            textures_.push_back(vt);
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
            std::vector<int> ft;
            int itrash, idx, idxt, idxn;
            iss >> trash;
            while (iss >> idx >> trash >> idxt >> trash >> idxn)
            {
                // in wavefront obj all indices start at 1, not zero
                idx--;
                idxt--;
                idxn--;
                f.push_back(idx);
                fn.push_back(idxn);
                ft.push_back(idxt);
            }
            faces_.push_back(f);
            faceNormals_.push_back(fn);
            faceTextures_.push_back(ft);
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

vec3 Model::texture(int i)
{
    return textures_[i];
}

std::vector<int> Model::faceTexture(int idx)
{
    return faceTextures_[idx];
}