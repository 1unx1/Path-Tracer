#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vecmath.h>
struct Texture
{
    unsigned char *pic;
    int w, h, c;
    Vector3f getColor(int u, int v) const;

public:
    Texture(const char *textureFile);
    Vector3f getColor(float u, float v) const;
};

#endif