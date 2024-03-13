#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

#include "utils.hpp"

struct Model {
    std::vector<Point> vertices;
    std::vector<Triangle> triangles;

    Model(const char *filename, int width, int height, float scale = 1.0) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            exit(1);
        }

        std::string line;

        while (!file.eof())
        {
            std::getline(file, line);
            std::cout <<  line << std::endl;
            if (line[0] == 'v' && line[1] == ' ') {
                Point p;
                sscanf(line.c_str(), "v %f %f %f", &p.x, &p.y, &p.z);

                // Denormalize the point to match the screen size
                p.x = ((p.x + 1.) * 0.5 * width);
                p.y = ((p.y + 1.) * 0.5 * height);
                vertices.push_back(p);
            }

            if (line[0] == 'f' && line[1] == ' ') {
                Triangle shape;
                Triangle texture;
                Triangle normal;
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

                triangles.push_back(shape);
            }
        }
    }

    void drawVertices(TGAImage &image, TGAColor color) {
        int i = 0;
        for (Point p : vertices) {
            image.set(p.x, p.y, color);
        }
    }

    void drawTriangles(TGAImage &image, TGAColor color) {
        for (Triangle t : triangles) {
            draw_line(vertices[t.p1], vertices[t.p2], image, color);
            draw_line(vertices[t.p1], vertices[t.p3], image, color);
            draw_line(vertices[t.p2], vertices[t.p3], image, color);
        }
    }
};

#endif // MODEL_HPP