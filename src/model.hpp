#ifndef MODEL_HPP
#define MODEL_HPP

#include "utils.hpp"

struct Model
{
    std::vector<Point> vertices;
    std::vector<Triangle> triangles;
    std::vector<Triangle> textures;
    std::vector<Triangle> normals;

    Model(const std::string filename, int width, int height, float scale = 1.0)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            exit(1);
        }

        std::string line;

        while (!file.eof())
        {
            std::getline(file, line);
            if (line[0] == 'v' && line[1] == ' ')
            {
                Point p;
                sscanf(line.c_str(), "v %f %f %f", &p.x, &p.y, &p.z);

                // Denormalize the point to match the screen size
                p.x = ((p.x + 1.) * 0.5 * width);
                p.y = ((p.y + 1.) * 0.5 * height);
                p.z *= 800;

                float r = (p.x + 1.) * 0.5 * 255;
                float g = (p.y + 1.) * 0.5 * 255;
                float b = (p.z + 1.) * 0.5 * 255;
                p.color = TGAColor(r, g, b, 255);

                vertices.push_back(p);
            }

            if (line[0] == 'f' && line[1] == ' ')
            {
                struct tmp
                {
                    int p1;
                    int p2;
                    int p3;
                };
                tmp shape;
                tmp texture;
                tmp normal;
                sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &shape.p1, &texture.p1, &normal.p1, &shape.p2, &texture.p2, &normal.p2, &shape.p3, &texture.p3, &normal.p3);

                // Decrement the indices to match the 0-based array
                shape.p1--;
                shape.p2--;
                shape.p3--;

                texture.p1--;
                texture.p2--;
                texture.p3--;

                normal.p1--;
                normal.p2--;
                normal.p3--;

                triangles.push_back(Triangle(vertices[shape.p1], vertices[shape.p2], vertices[shape.p3]));
                textures.push_back(Triangle(vertices[texture.p1], vertices[texture.p2], vertices[texture.p3]));
                normals.push_back(Triangle(vertices[normal.p1], vertices[normal.p2], vertices[normal.p3]));
            }
        }
    }

    void drawVertices(TGAImage &image, TGAColor color)
    {
#pragma omp parallel for
        for (Point p : vertices)
        {
            image.set(p.x, p.y, color);
        }
    }

    void drawTriangles(TGAImage &image, TGAColor color)
    {
#pragma omp parallel for
        for (Triangle t : triangles)
        {
            draw_line(t.p1, t.p2, image, color);
            draw_line(t.p1, t.p3, image, color);
            draw_line(t.p2, t.p3, image, color);
        }
    }

    void fillTriangles(TGAImage &image)
    {
#pragma omp parallel for
        for (Triangle t : triangles)
        {
            fillTriangle(t, image, randomColor());
        }
    }

    void fillTrianglesLerp(TGAImage &image)
    {
#pragma omp parallel for
        for (Triangle t : triangles)
        {
            fillTriangleLerp(t, image);
        }
    }

    void fillTrianglesWithBackFaceCulling(TGAImage &image)
    {
#pragma omp parallel for
        for (int i = 0; i < triangles.size(); i++)
        {
            fillTriangleWithBackFaceCulling(triangles[i], normals[i], image);
        }
    }
};

#endif // MODEL_HPP