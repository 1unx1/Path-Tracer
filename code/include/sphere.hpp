#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Sphere : public Object3D
{
public:
    Sphere() : center(0, 0, 0), radius(1) {}

    Sphere(const Vector3f &center, float radius, Material *material)
        : Object3D(material), center(center), radius(radius) {}

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        Ray ray(r.getOrigin(), r.getDirection().normalized());
        Vector3f l = center - ray.getOrigin();
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
            Vector3f norm = (ray.pointAtParameter(t) - center).normalized();
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
        box = Box(center - Vector3f(radius), center + Vector3f(radius));
        return true;
    }

protected:
    Vector3f center;
    float radius;
};

#endif
