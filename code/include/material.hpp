#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.hpp"
#include <iostream>

class Material
{
public:
    explicit Material(const Vector3f &d_color,
                      const Vector3f &e_color,
                      const Vector3f &s_color = Vector3f::ZERO,
                      const Vector3f &t = Vector3f(1, 0, 0),
                      float s = 0,
                      float r = 1,
                      const char *textureFile = "")
        : diffuseColor(d_color),
          specularColor(s_color),
          shininess(s),
          emissionColor(e_color),
          refRate(r),
          type(t), texture(textureFile) {}

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const
    {
        return diffuseColor;
    }

    Vector3f getColor(float u, float v) const
    {
        if (texture.pic == nullptr)
            return diffuseColor;
        else
            return texture.getColor(u, v);
    }

    Vector3f Shade(const Ray &r, const Hit &h,
                   const Vector3f &dirToLight, const Vector3f &lightColor)
    {
        const Vector3f &n = h.getNormal();
        float diffDot = Vector3f::dot(dirToLight, n);
        float specDot = Vector3f::dot(-r.getDirection(), 2 * (Vector3f::dot(n, dirToLight) * n) - dirToLight);
        Vector3f diff = diffuseColor * (diffDot < 0 ? 0 : diffDot);
        Vector3f spec = specularColor * pow(specDot < 0 ? 0 : specDot, shininess);
        return lightColor * (diff + spec);
    }

    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
    Vector3f emissionColor;
    float refRate;
    Vector3f type;
    Texture texture;
};

#endif // MATERIAL_H
