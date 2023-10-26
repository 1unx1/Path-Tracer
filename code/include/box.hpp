#ifndef BOX_H
#define BOX_H

#include <vecmath.h>
#include <vector>
#include <random>
#include <algorithm>
#include "ray.hpp"
#include "define.h"

class Box
{
public:
    Vector3f lo, hi; // x_lo < x_hi, y_lo < y_hi, z_lo < z_hi

    Box() : lo(-INF), hi(INF) {}

    Box(const Vector3f &lo, const Vector3f &hi) : lo(lo), hi(hi) {}

    void setVertices(const Vector3f &lo, const Vector3f &hi)
    {
        this->lo = lo;
        this->hi = hi;
    }

    bool intersect(const Ray &r, float &t) const
    {
        const Vector3f &o = r.getOrigin(), d = r.getDirection();
        Vector3f t_min, t_max;
        // assert d.x, d.y, d.z != 0
        if (d.x() > 0)
        {
            t_min[0] = (lo.x() - o.x()) / d.x();
            t_max[0] = (hi.x() - o.x()) / d.x();
        }
        else
        {
            t_min[0] = (hi.x() - o.x()) / d.x();
            t_max[0] = (lo.x() - o.x()) / d.x();
        }
        if (d.y() > 0)
        {
            t_min[1] = (lo.y() - o.y()) / d.y();
            t_max[1] = (hi.y() - o.y()) / d.y();
        }
        else
        {
            t_min[1] = (hi.y() - o.y()) / d.y();
            t_max[1] = (lo.y() - o.y()) / d.y();
        }
        // check x & y
        if (t_min.x() > t_max.y() || t_min.y() > t_max.x())
            return false;

        if (d.z() > 0)
        {
            t_min[2] = (lo.z() - o.z()) / d.z();
            t_max[2] = (hi.z() - o.z()) / d.z();
        }
        else
        {
            t_min[2] = (hi.z() - o.z()) / d.z();
            t_max[2] = (lo.z() - o.z()) / d.z();
        }
        // check x & z and y & z
        if (t_min.x() > t_max.z() || t_min.z() > t_max.x() ||
            t_min.y() > t_max.z() || t_min.z() > t_max.y())
            return false;
        // intersecting t = maximum of t_min in 3 dims
        t = fmax(fmax(t_min.x(), t_min.y()), t_min.z());
        return true;
    }

    void surrounding(const Box &a, const Box &b)
    {
        lo = Vector3f(fmin(a.lo.x(), b.lo.x()),
                      fmin(a.lo.y(), b.lo.y()),
                      fmin(a.lo.z(), b.lo.z()));
        hi = Vector3f(fmax(a.hi.x(), b.hi.x()),
                      fmax(a.hi.y(), b.hi.y()),
                      fmax(a.hi.z(), b.hi.z()));
    }
};

#endif