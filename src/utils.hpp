#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#include "tgaimage.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const int width = 500;
const int height = 500;

struct Point
{
    float x;
    float y;
    float z;

    Point() : x(0), y(0), z(0) {}
    Point(float x, float y, float z) : x(x), y(y), z(z) {}

    // Overload the << operator to print the point
    friend std::ostream &operator<<(std::ostream &os, const Point &p)
    {
        os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
        return os;
    }
};

struct Triangle
{
    int p1;
    int p2;
    int p3;

    Triangle() : p1(), p2(), p3() {}
    Triangle(int p1, int p2, int p3) : p1(p1), p2(p2), p3(p3) {}

    // Overload the << operator to print the triangle
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t)
    {
        os << "Triangle: {" << t.p1 << "; " << t.p2 << "; " << t.p3 << "}";
        return os;
    }
};

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
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

    int y = y0;
    int d_error = 2 * std::abs(y1 - y0);
    int error = 0;

    for (int x = x0; x < x1; x++)
    {

        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        error += d_error;
        if (error > x1 - x0)
        {
            y += ((y1 > y0) ? 1 : -1);
            error -= 2 * (x1 - x0);
        }
    }
}

void draw_line(Point p1, Point p2, TGAImage &image, TGAColor color)
{
    draw_line(p1.x, p1.y, p2.x, p2.y, image, color);
}

#endif // UTILS_HPP