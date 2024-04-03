#pragma once
#include "geometry.hpp"
struct Camera
{
    vec3 pos;
    vec3 lookAt;

    vec3 x;
    vec3 y;
    vec3 z;

    Camera(vec3 pos = vec3(0, 0, 0), vec3 lookAt = vec3(0, 0, 1)) : pos(pos), lookAt(lookAt)
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
        mat4 matrix = mat4::identity();
        matrix[0][0] = x.x;
        matrix[1][0] = x.y;
        matrix[2][0] = x.z;
        matrix[0][1] = y.x;
        matrix[1][1] = y.y;
        matrix[2][1] = y.z;
        matrix[0][2] = -z.x;
        matrix[1][2] = -z.y;
        matrix[2][2] = -z.z;
        matrix[3][0] = -dot(x, pos);
        matrix[3][1] = -dot(y, pos);
        matrix[3][2] = -dot(z, pos);
        return matrix;
    }
};
