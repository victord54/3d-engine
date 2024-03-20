#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include <fstream>
#include <sstream>

#include <algorithm>
#include <filesystem>

#include "tgaimage.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

TGAColor randomColor()
{
    return TGAColor(rand() % 255, rand() % 255, rand() % 255, 255);
}

struct Point
{
    float x;
    float y;
    float z;

    TGAColor color;

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
    Point p1;
    Point p2;
    Point p3;

    float alpha;
    float beta;
    float gamma;

    Triangle() : p1(), p2(), p3() {}
    Triangle(Point p1, Point p2, Point p3) : p1(p1), p2(p2), p3(p3) {}

    // Overload the << operator to print the triangle
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t)
    {
        os << "Triangle: {" << t.p1 << "; " << t.p2 << "; " << t.p3 << "}";
        return os;
    }

    void setBarycentricCoordinates(float a, float b, float c)
    {
        alpha = a;
        beta = b;
        gamma = c;
    }
};

struct Vec3f
{
    float x, y, z;
    Vec3f() : x(0), y(0), z(0) {}

    Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3f operator-(const Vec3f &v) const
    {
        return Vec3f(x - v.x, y - v.y, z - v.z);
    }

    Vec3f operator^(const Vec3f &v) const
    {
        return Vec3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    Vec3f normalize()
    {
        float norm = std::sqrt(x * x + y * y + z * z);
        return Vec3f(x / norm, y / norm, z / norm);
    }

    float operator*(const Vec3f &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3f operator*(float k) const
    {
        return Vec3f(x * k, y * k, z * k);
    }

    Vec3f operator+(const Vec3f &v) const
    {
        return Vec3f(x + v.x, y + v.y, z + v.z);
    }

    Vec3f operator/(float k) const
    {
        return Vec3f(x / k, y / k, z / k);
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

void draw_line(Point &p1, Point &p2, TGAImage &image, TGAColor color)
{
    draw_line(p1.x, p1.y, p2.x, p2.y, image, color);
}

bool isInsideTriangle(Point &p, Triangle &t)
{
    float alpha = ((t.p2.y - t.p3.y) * (p.x - t.p3.x) + (t.p3.x - t.p2.x) * (p.y - t.p3.y)) /
                  ((t.p2.y - t.p3.y) * (t.p1.x - t.p3.x) + (t.p3.x - t.p2.x) * (t.p1.y - t.p3.y));

    float beta = ((t.p3.y - t.p1.y) * (p.x - t.p3.x) + (t.p1.x - t.p3.x) * (p.y - t.p3.y)) /
                 ((t.p2.y - t.p3.y) * (t.p1.x - t.p3.x) + (t.p3.x - t.p2.x) * (t.p1.y - t.p3.y));

    float gamma = 1.0f - alpha - beta;

    t.setBarycentricCoordinates(alpha, beta, gamma);

    return alpha > 0 && beta > 0 && gamma > 0;
}

void fillTriangle(Triangle &t, TGAImage &image, TGAColor color)
{
    // Bounding box
    int minX = std::min(std::min(t.p1.x, t.p2.x), t.p3.x);
    int minY = std::min(std::min(t.p1.y, t.p2.y), t.p3.y);
    int maxX = std::max(std::max(t.p1.x, t.p2.x), t.p3.x);
    int maxY = std::max(std::max(t.p1.y, t.p2.y), t.p3.y);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            Point p = {(float)x, (float)y, 0.0};
            if (isInsideTriangle(p, t))
            {
                image.set(x, y, color);
            }
        }
    }
}

void fillTriangleLerp(Triangle &t, TGAImage &image)
{
    // Bounding box
    int minX = std::min(std::min(t.p1.x, t.p2.x), t.p3.x);
    int minY = std::min(std::min(t.p1.y, t.p2.y), t.p3.y);
    int maxX = std::max(std::max(t.p1.x, t.p2.x), t.p3.x);
    int maxY = std::max(std::max(t.p1.y, t.p2.y), t.p3.y);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            Point p = {(float)x, (float)y, 0.0};
            if (isInsideTriangle(p, t))
            {
                float alpha = t.alpha;
                float beta = t.beta;
                float gamma = t.gamma;

                TGAColor color = TGAColor(
                    alpha * t.p1.color.r + beta * t.p2.color.r + gamma * t.p3.color.r,
                    alpha * t.p1.color.g + beta * t.p2.color.g + gamma * t.p3.color.g,
                    alpha * t.p1.color.b + beta * t.p2.color.b + gamma * t.p3.color.b,
                    255);

                image.set(x, y, color);
            }
        }
    }
}

void fillTriangleWithBackFaceCulling(Triangle &t, Triangle &normal, TGAImage &image, int *zBuffer)
{
    // Bounding box
    int minX = std::min(std::min(t.p1.x, t.p2.x), t.p3.x);
    int minY = std::min(std::min(t.p1.y, t.p2.y), t.p3.y);
    int maxX = std::max(std::max(t.p1.x, t.p2.x), t.p3.x);
    int maxY = std::max(std::max(t.p1.y, t.p2.y), t.p3.y);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            Point p = {(float)x, (float)y, 0.0};
            if (isInsideTriangle(p, t))
            {
                Vec3f p1 = {t.p1.x, t.p1.y, t.p1.z};
                Vec3f p2 = {t.p2.x, t.p2.y, t.p2.z};
                Vec3f p3 = {t.p3.x, t.p3.y, t.p3.z};
                Vec3f normal = (p3 - p1) ^ (p2 - p1);
                Vec3f lightDir = Vec3f(0, 0, -1);
                float dot = normal.normalize() * lightDir.normalize();
                if (dot < 0)
                {
                    continue;
                }

                // Z-buffering
                if (zBuffer[y * image.get_width() + x] < p.z)
                {
                    zBuffer[y * image.get_width() + x] = p.z;
                }
                else
                {
                    continue;
                }

                TGAColor color = TGAColor(255, 255, 255, 255);

                float alpha = t.alpha;
                float beta = t.beta;
                float gamma = t.gamma;

                color = TGAColor(
                    alpha * t.p1.color.r + beta * t.p2.color.r + gamma * t.p3.color.r,
                    alpha * t.p1.color.g + beta * t.p2.color.g + gamma * t.p3.color.g,
                    alpha * t.p1.color.b + beta * t.p2.color.b + gamma * t.p3.color.b,
                    255);

                color.r = std::min(255, std::max(0, (int)(color.r * dot)));
                color.g = std::min(255, std::max(0, (int)(color.g * dot)));
                color.b = std::min(255, std::max(0, (int)(color.b * dot)));

                image.set(x, y, color);
            }
        }
    }
}

Point worldToScreen(Point p, int width, int height)
{
    // Denormalize the point to match the screen size
    p.x = ((p.x + 1.) * 0.5 * width);
    p.y = ((p.y + 1.) * 0.5 * height);
    p.z *= 800;

    return p;
}

#endif // UTILS_HPP