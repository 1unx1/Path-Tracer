#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "texture.hpp"
#include "stb_image.h"

Texture::Texture(const char *textureFile)
{
    if (strlen(textureFile) > 0)
    {
        pic = stbi_load(textureFile, &w, &h, &c, 0);
        printf("Texture file: %s loaded. Size: %dx%dx%d\n", textureFile, w, h,
               c);
    }
    else
    {
        pic = nullptr;
    }
}

Vector3f Texture::getColor(int u, int v) const
{
    if (pic == nullptr)
        return Vector3f::ZERO;
    u = u < 0 ? 0 : (u > w - 1 ? w - 1 : u);
    v = v < 0 ? 0 : (v > h - 1 ? h - 1 : v);
    int idx = (v * w + u) * c;
    return Vector3f(pic[idx + 0], pic[idx + 1], pic[idx + 2]) / 255.;
}

Vector3f Texture::getColor(float u, float v) const
{
    if (pic == nullptr)
        return Vector3f::ZERO;
    u -= int(u);
    v -= int(v);
    if (u < 0)
        u++;
    if (v < 0)
        v++;
    u = u * w;
    v = (1 - v) * h;
    int iu = int(u), iv = int(v);
    float a = u - iu, b = v - iv;
    Vector3f ret = Vector3f::ZERO;
    ret += (1 - a) * (1 - b) * getColor(iu, iv);
    ret += a * (1 - b) * getColor(iu + 1, iv);
    ret += (1 - a) * b * getColor(iu, iv + 1);
    ret += a * b * getColor(iu + 1, iv + 1);
    return ret;
}