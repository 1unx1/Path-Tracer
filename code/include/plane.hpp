#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Plane : public Object3D
{
public:
    Plane() {}

    // function: ax + by + cz = d
    Plane(const Vector3f &normal, float d, Material *m)
        : Object3D(m), normal(normal), d(-d) {}

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        float dot = Vector3f::dot(normal, r.getDirection());
        if (dot == 0)
            return false;
        float t = -(d + Vector3f::dot(normal, r.getOrigin())) / dot;

        if (t > 0 && t < h.getT() && t >= tmin)
        {
            Vector3f n = (dot < 0 ? normal : -normal).normalized();
            h.set(t, material, n, material->diffuseColor);
            return true;
        }
        else
            return false;
    }

    bool get_bounding_box(Box &box) const override
    {
        return false;
    }

protected:
    Vector3f normal;
    float d;
};

#endif // PLANE_H
