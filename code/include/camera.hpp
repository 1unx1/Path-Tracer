#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include "define.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH)
    {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

class PerspectiveCamera : public Camera
{

public:
    // angle is in radian.
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
                      const Vector3f &up, int imgW, int imgH,
                      float angle, float focalLen = 1, float aperture = 0,
                      float time1 = 0, float time2 = 0)
        : Camera(center, direction, up, imgW, imgH),
          angle(angle), cx(imgW / 2), cy(imgH / 2),
          focalLen(focalLen), aperture(aperture), f(imgH / (2 * tan(angle / 2))),
          time1(time1), time2(time2) {}

    Ray generateRay(const Vector2f &point) override
    {
        float x = focalLen * (point.x() - cx) / f;
        float y = focalLen * (cy - point.y()) / f;
        float dx = RND1 * aperture;
        float dy = RND1 * aperture;
        Vector3f dir = Vector3f(x - dx, y - dy, focalLen).normalized();
        Matrix3f r(horizontal, -up, direction);
        dir = r * dir;
        float time = time1 + float(rand()) / RAND_MAX * (time2 - time1);
        return Ray(center + horizontal * dx - up * dy, dir, time);
    }

private:
    float angle;
    float cx;
    float cy;
    float focalLen;
    float aperture;
    float f;
    float time1;
    float time2;
};

#endif // CAMERA_H
