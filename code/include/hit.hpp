#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit
{
public:
    // constructors
    Hit()
    {
        material = nullptr;
        t = 1e38;
    }

    Hit(float _t, Material *m, const Vector3f &n)
    {
        t = _t;
        material = m;
        normal = n;
        color = Vector3f::ZERO;
    }

    Hit(const Hit &h)
    {
        t = h.t;
        material = h.material;
        normal = h.normal;
        color = h.color;
    }

    // destructor
    ~Hit() = default;

    float getT() const
    {
        return t;
    }

    Material *getMaterial() const
    {
        return material;
    }

    const Vector3f &getNormal() const
    {
        return normal;
    }

    const Vector3f &getColor() const
    {
        return color;
    }

    void set(float t, Material *material, const Vector3f &norm, const Vector3f &color)
    {
        this->t = t;
        this->material = material;
        this->normal = norm;
        this->color = color;
    }

private:
    float t;
    Material *material;
    Vector3f normal;
    Vector3f color;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
