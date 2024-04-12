#pragma once
#include <cmath>

template <int n>
struct vec
{
    double data[n];

    vec()
    {
        for (int i = 0; i < n; i++)
        {
            data[i] = 0;
        }
    }

    double &operator[](int i)
    {
        return data[i];
    }

    const double &operator[](int i) const
    {
        return data[i];
    }
};

template <int n>
std::ostream &operator<<(std::ostream &out, const vec<n> &a)
{
    out << "(";
    for (int i = 0; i < n; i++)
    {
        out << a[i] << (i == n - 1 ? "" : ", ");
    }
    out << ")";
    return out;
}

template <int n>
vec<n> operator+(const vec<n> &a, const vec<n> &b)
{
    vec<n> c;
    for (int i = 0; i < n; i++)
    {
        c[i] = a[i] + b[i];
    }
    return c;
}

template <int n>
vec<n> operator-(const vec<n> &a, const vec<n> &b)
{
    vec<n> c;
    for (int i = 0; i < n; i++)
    {
        c[i] = a[i] - b[i];
    }
    return c;
}

template <int n>
vec<n> operator*(double k, const vec<n> &a)
{
    vec<n> c;
    for (int i = 0; i < n; i++)
    {
        c[i] = k * a[i];
    }
    return c;
}

template <int n>
vec<n> operator*(const vec<n> &a, double k)
{
    return k * a;
}

template <int n>
vec<n> operator/(const vec<n> &a, double k)
{
    return (1 / k) * a;
}

template <int n>
double dot(const vec<n> &a, const vec<n> &b)
{
    double d = 0;
    for (int i = 0; i < n; i++)
    {
        d += a[i] * b[i];
    }
    return d;
}

template <int n>
double norm(const vec<n> &a)
{
    return std::sqrt(dot(a, a));
}

template <int n>
vec<n> normalize(const vec<n> &a)
{
    return (1 / norm(a)) * a;
}

template <int n>
vec<n> cross(const vec<n> &a, const vec<n> &b)
{
    static_assert(n == 3, "cross product is only defined for 3D vectors");
    vec<n> c;
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
    return c;
}

template <>
struct vec<2>
{
    double x, y;

    vec(double x = 0, double y = 0) : x(x), y(y) {}

    double &operator[](int i)
    {
        return i == 0 ? x : y;
    }

    const double &operator[](int i) const
    {
        return i == 0 ? x : y;
    }
};

template <>
struct vec<3>
{
    double x, y, z;

    vec(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    double &operator[](int i)
    {
        return i == 0 ? x : (i == 1 ? y : z);
    }

    const double &operator[](int i) const
    {
        return i == 0 ? x : (i == 1 ? y : z);
    }
};

template <>
struct vec<4>
{
    double x, y, z, w;

    vec(double x = 0, double y = 0, double z = 0, double w = 0) : x(x), y(y), z(z), w(w) {}

    double &operator[](int i)
    {
        return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w));
    }

    const double &operator[](int i) const
    {
        return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w));
    }
};

using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

template <int m, int n>
struct mat
{
    double data[m][n];

    mat()
    {
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                data[i][j] = 0;
            }
        }
    }

    double *operator[](int i)
    {
        return data[i];
    }

    const double *operator[](int i) const
    {
        return data[i];
    }

    static mat<m, n> identity()
    {
        mat<m, n> a;
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                a[i][j] = i == j ? 1 : 0;
            }
        }
        return a;
    }

    static mat<m, n> zero()
    {
        return mat<m, n>();
    }
};

template <int m, int n, int l>
mat<m, l> operator*(const mat<m, n> &a, const mat<n, l> &b)
{
    mat<m, l> c;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < l; j++)
        {
            c[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return c;
}

template <int m, int n>
vec<m> operator*(const mat<m, n> &a, const vec<n> &b)
{
    vec<m> c;
    for (int i = 0; i < m; i++)
    {
        c[i] = 0;
        for (int j = 0; j < n; j++)
        {
            c[i] += a[i][j] * b[j];
        }
    }
    return c;
}

template <int m, int n>
mat<n, m> invert(const mat<m, n> &a)
{
    mat<n, m> b;
    double det = 0;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            b[j][i] = (i + j) % 2 == 0 ? 1 : -1;
            det += b[j][i] * a[j][i];
        }
    }
    if (det == 0)
    {
        throw std::runtime_error("Matrix is not invertible");
    }
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            b[j][i] /= det;
        }
    }
    return b;
}

using mat2 = mat<2, 2>;
using mat3 = mat<3, 3>;
using mat4 = mat<4, 4>;

inline vec3 barycentric(vec2 p0, vec2 p1, vec2 p2, vec2 p)
{
    vec3 u = cross(vec3(p2.x - p0.x, p1.x - p0.x, p0.x - p.x), vec3(p2.y - p0.y, p1.y - p0.y, p0.y - p.y));
    if (std::abs(u.z) < 1)
    {
        return vec3(-1, 1, 1);
    }
    return vec3(1.0 - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

inline mat4 translate(const vec3 &v)
{
    mat4 T = mat4::identity();
    T[0][3] = v[0];
    T[1][3] = v[1];
    T[2][3] = v[2];
    return T;
}

inline mat4 scale(const vec3 &v)
{
    mat4 S = mat4::identity();
    S[0][0] = v[0];
    S[1][1] = v[1];
    S[2][2] = v[2];
    return S;
}

inline mat4 rotate(const vec3 &angles)
{
    // Convert angles to radians
    vec3 angles_ = angles * (M_PI / 180.0);
    mat4 Rx = mat4::identity();
    Rx[1][1] = std::cos(angles_[0]);
    Rx[1][2] = -std::sin(angles_[0]);
    Rx[2][1] = std::sin(angles_[0]);
    Rx[2][2] = std::cos(angles_[0]);

    mat4 Ry = mat4::identity();
    Ry[0][0] = std::cos(angles_[1]);
    Ry[0][2] = std::sin(angles_[1]);
    Ry[2][0] = -std::sin(angles_[1]);
    Ry[2][2] = std::cos(angles_[1]);

    mat4 Rz = mat4::identity();
    Rz[0][0] = std::cos(angles_[2]);
    Rz[0][1] = -std::sin(angles_[2]);
    Rz[1][0] = std::sin(angles_[2]);
    Rz[1][1] = std::cos(angles_[2]);

    return Rz * Ry * Rx;
}