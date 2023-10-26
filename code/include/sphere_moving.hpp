#ifndef SPHERE_MOVING_H
#define SPHERE_MOVING_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class SphereMoving : public Object3D
{
public:
    SphereMoving(const Vector3f &center1, const Vector3f &center2,
                 float time1, float time2, float radius,
                 Material *material)
        : Object3D(material), center1(center1), center2(center2),
          time1(time1), time2(time2), radius(radius) {}

    ~SphereMoving() override = default;

    Vector3f center(float time) const
    {
        return center1 + (time - time1) / (time2 - time1) * (center2 - center1);
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        Ray ray(r.getOrigin(), r.getDirection().normalized());
        Vector3f c = center(r.time);
        Vector3f l = c - ray.getOrigin();
        float side = l.squaredLength() - radius * radius;
        float tp = Vector3f::dot(l, ray.getDirection());
        if (side >= 0 && tp <= 0)
            return false;
        float d2 = l.squaredLength() - tp * tp;
        if (d2 > radius * radius)
            return false;
        float t = sqrt(radius * radius - d2);
        if (side <= 0)
            t = tp + t;
        else
            t = tp - t;

        if (t > 0 && t < h.getT() && t >= tmin)
        {
            Vector3f norm = (ray.pointAtParameter(t) - c).normalized();
            float u = 0.5 + atan2(norm.x(), norm.z()) / (2 * M_PI);
            float v = -0.5 + asin(norm.y()) / M_PI;
            h.set(t, material, norm, material->getColor(u, v));
            return true;
        }
        else
            return false;
    }

    bool get_bounding_box(Box &box) const override
    {
        Box box1 = Box(center1 - Vector3f(radius), center1 + Vector3f(radius));
        Box box2 = Box(center2 - Vector3f(radius), center2 + Vector3f(radius));
        box.surrounding(box1, box2);
        return true;
    }

protected:
    Vector3f center1, center2;
    float time1, time2;
    float radius;
};

#endif
