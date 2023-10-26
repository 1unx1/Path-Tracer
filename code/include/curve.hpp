#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include "define.h"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint
{
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D
{
protected:
    std::vector<Vector3f> controls;

public:
    float y_max, y_min, radius;
    float range[2];

    explicit Curve(std::vector<Vector3f> points)
        : controls(std::move(points)), y_max(-INF), y_min(INF), radius(0)
    {
        for (auto &point : controls)
        {
            y_max = fmax(point.y(), y_max);
            y_min = fmin(point.y(), y_min);
            radius = fmax(fmax(radius, (float)fabs(point.x())), (float)fabs(point.z()));
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        return false;
    }

    std::vector<Vector3f> &getControls()
    {
        return controls;
    }

    virtual CurvePoint getVT(float t) = 0;

    bool get_bounding_box(Box &box) const override
    {
        return false;
    }
};

class BezierCurve : public Curve
{
public:
    explicit BezierCurve(const std::vector<Vector3f> &points)
        : Curve(points), N(points.size() - 1)
    {
        range[0] = 0;
        range[1] = 1;
        if (points.size() < 4 || points.size() % 3 != 1)
        {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        c = std::vector<std::vector<float>>(N + 1, std::vector<float>(N + 1, 0));
        for (int i = 0; i <= N; i++)
        {
            if (i)
            {
                for (int j = 1; j <= N; j++)
                    c[i][j] = c[i - 1][j] + c[i - 1][j - 1];
                c[i][0] = c[i - 1][0];
            }
            else
            {
                for (int j = 1; j <= N; j++)
                    c[i][j] = 0;
                c[i][0] = 1;
            }
        }
    }

    CurvePoint getVT(float t) override
    {
        Vector3f ft = Vector3f::ZERO;
        Vector3f df = Vector3f::ZERO;
        for (int i = 0; i <= N; i++)
        {
            auto p = controls[i];
            ft += Bezier(i, N, t) * p;
            df += BezierPrime(i, N, t) * p;
        }
        return (CurvePoint){ft, df};
    }

protected:
    int N;
    std::vector<std::vector<float>> c;

    float Bezier(int i, int n, float t)
    {
        return c[n][i] * pow(1 - t, n - i) * pow(t, i);
    }

    float BezierPrime(int i, int n, float t)
    {
        return n * (Bezier(i - 1, n - 1, t) - Bezier(i, n - 1, t));
    }
};

class BsplineCurve : public Curve
{
public:
    BsplineCurve(const std::vector<Vector3f> &points)
        : Curve(points), N(points.size() - 1), K(3)
    {
        B = new float[N + K + 1];
        dB = new float[N + K + 1];
        // interval: [l, r) = [t_i, t_i+1)
        // I = [t_k, t_n+1]
        range[0] = tpad(K);
        range[1] = tpad(N + 1);
        if (points.size() < 4)
        {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }

    ~BsplineCurve() override
    {
        delete[] B;
        delete[] dB;
    }

    CurvePoint getVT(float t) override
    {
        for (int i = 0; i <= N + K; i++)
        {
            B[i] = (tpad(i) <= t && t < tpad(i + 1));
        }
        for (int p = 1; p <= K; p++)
        {
            for (int i = 0; i <= N + K - p; i++)
            {
                if (p == K)
                {
                    dB[i] = p * (B[i] / (tpad(i + p) - tpad(i)) - B[i + 1] / (tpad(i + p + 1) - tpad(i + 1)));
                }
                B[i] = (t - tpad(i)) / (tpad(i + p) - tpad(i)) * B[i] + (tpad(i + p + 1) - t) / (tpad(i + p + 1) - tpad(i + 1)) * B[i + 1];
            }
        }
        Vector3f V = Vector3f::ZERO;
        Vector3f T = Vector3f::ZERO;
        for (int i = 0; i <= N; i++)
        {
            auto p = controls[i];
            V += B[i] * p;
            T += dB[i] * p;
        }
        return (CurvePoint){V, T};
    }

protected:
    int N;
    int K;
    float *B, *dB;
    float tpad(int i) const
    {
        return i / float(N + K + 1);
    }
};

#endif // CURVE_HPP
