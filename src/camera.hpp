#pragma once
#include "geometry.hpp"
struct Camera
{
    vec3 pos;
    vec3 lookAt;

    vec3 x;
    vec3 y;
    vec3 z;

    double fov;
    double near;
    double far;

    double aspect;

    Camera(vec3 pos = vec3(0, 0, 0), vec3 lookAt = vec3(0, 0, 1), double fov = 90, double near = 0.1, double far = 1000, double aspect = 1) : pos(pos), lookAt(lookAt), fov(fov), near(near), far(far), aspect(aspect)
    {
        update();
    }

    void setView(vec3 a_pos, vec3 a_lookAt)
    {
        pos = a_pos;
        lookAt = a_lookAt;
        update();
    }

    void update()
    {
        z = normalize(pos - lookAt);
        x = normalize(cross(vec3(0, 1, 0), z));
        y = normalize(cross(z, x));
    }

    mat4 viewMatrix()
    {
        mat4 translation = mat4::identity();
        translation[0][3] = -pos.x;
        translation[1][3] = -pos.y;
        translation[2][3] = -pos.z;

        mat4 rotation = mat4::identity();
        rotation[0][0] = x.x;
        rotation[0][1] = x.y;
        rotation[0][2] = x.z;
        rotation[1][0] = y.x;
        rotation[1][1] = y.y;
        rotation[1][2] = y.z;
        rotation[2][0] = z.x;
        rotation[2][1] = z.y;
        rotation[2][2] = z.z;

        return rotation * translation;
    }

    mat4 projectionMatrix()
    {
        double fovRad = std::tan((fov * 0.5) * (M_PI / 180));
        mat4 matrix = mat4::identity();
        matrix[0][0] = 1 / (aspect * fovRad);
        matrix[1][1] = 1 / fovRad;
        matrix[2][2] = (near + far) / (near - far);
        matrix[2][3] = (2 * near * far) / (near - far);
        matrix[3][2] = -1;
        matrix[3][3] = 0;
        return matrix;
    }

    vec3 perspectiveDivide(vec4 point)
    {
        mat4 matrix = mat4::identity();
        matrix[3][2] = -1 / pos.z;
        mat<4, 1> p;
        p[0][0] = point.x;
        p[1][0] = point.y;
        p[2][0] = point.z;
        p[3][0] = point.w;

        mat<4, 1> result = matrix * p;

        result[0][0] /= result[3][0];
        result[1][0] /= result[3][0];
        result[2][0] /= result[3][0];

        vec3 v;
        v.x = result[0][0];
        v.y = result[1][0];
        v.z = result[2][0];

        return v;
    }
};
