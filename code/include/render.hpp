#ifndef RENDER_H
#define RENDER_H

#include <time.h>
#include <random>
#include <omp.h>
#include "camera.hpp"
#include "group.hpp"
#include "image.hpp"
#include "material.hpp"
#include "scene_parser.hpp"
#include "define.h"
#include "bvh.hpp"

float randf()
{
    return float(rand()) / RAND_MAX;
}

float randf(float min, float max)
{
    return min + randf() * (max - min);
}

Vector3f random_in_unit_sphere()
{
    while (true)
    {
        Vector3f p = Vector3f(randf(-1, 1), randf(-1, 1), randf(-1, 1));
        if (p.squaredLength() >= 1)
            continue;
        return p;
    }
}

Vector3f ray_color(Ray r, const SceneParser &scene, BVHNode *root, const std::vector<Object3D *> &obj_no_box, int pt_depth = 5, bool useBVH = true)
{
    Group *grp = scene.getGroup();
    int depth = 0;
    float t_min = 1e-3;
    Vector3f color = Vector3f::ZERO, cf(1);
    while (true)
    {
        depth++;
        if (depth > pt_depth)
        {
            return color;
        }
        Hit h;
        if (useBVH)
        {
            float t = INFINITY;
            if (root->intersect(r, h, t_min) && h.getT() < t)
            {
                t = h.getT();
            }
            for (Object3D *obj : obj_no_box)
            {
                if (obj->intersect(r, h, t_min) && h.getT() < t)
                {
                    t = h.getT();
                }
            }
            if (std::isinf(t))
            {
                color += scene.getBackgroundColor();
                return color;
            }
        }
        else
        {
            if (!grp->intersect(r, h, t_min))
            {
                color += scene.getBackgroundColor();
                return color;
            }
        }
        r.origin += r.direction * h.getT();
        Material *material = h.getMaterial();
        Vector3f refColor(0), norm(h.getNormal());

        float type = RND2;

        if (type <= material->type.x())
        {
            // diffusion
            refColor = material->diffuseColor;
            r.direction = norm + random_in_unit_sphere();
        }
        else if (type <= material->type.x() + material->type.y())
        {
            // reflection
            refColor = material->specularColor;
            Vector3f v = -2 * norm * Vector3f::dot(r.direction, norm);
            r.direction = (r.direction + v).normalized();
        }
        else
        {
            // try refraction
            float refR = material->refRate;
            float r0 = (1 - refR) * (1 - refR) / ((1 + refR) * (1 + refR));
            if (Vector3f::dot(norm, r.direction) > 0)
            {
                norm.negate();
            }
            else
            {
                refR = 1 / refR;
            }
            float cosA = -Vector3f::dot(r.direction, norm);         // cos(A)
            float cosBSquare = 1 - refR * refR * (1 - cosA * cosA); // cos^2(B)
            float rProb = r0 + (1.0 - r0) * pow(1 - cosA,
                                                5.0); // Schlick-approximation
            if (cosBSquare > 0 && RND2 > rProb)
            {
                // real refraction
                refColor = Vector3f(1);
                r.direction = (r.direction * refR + norm * (refR * cosA - sqrt(cosBSquare))).normalized();
            }
            else
            {
                // reflection
                refColor = material->specularColor;
                r.direction = (r.direction + 2 * norm * cosA).normalized();
            }
        }
        if (h.getColor() != material->diffuseColor)
            refColor = h.getColor();
        color += material->emissionColor * cf;
        cf = cf * refColor;
    }
}

class PathTracer
{
public:
    const SceneParser &scene;
    int samples;
    int depth;
    const char *fout;

    PathTracer(SceneParser &scene, int samples, int depth, const char *fout)
        : scene(scene), samples(samples), depth(depth), fout(fout) {}

    void render()
    {
        std::vector<Object3D *> obj_with_box, obj_no_box;
        for (Object3D *obj : scene.getGroup()->list)
        {
            Box box;
            if (obj->get_bounding_box(box))
            {
                obj_with_box.push_back(obj);
            }
            else
            {
                obj_no_box.push_back(obj);
            }
        }
        BVHNode root(obj_with_box, 0, obj_with_box.size());
        Camera *camera = scene.getCamera();
        int w = camera->getWidth(), h = camera->getHeight();
        Image img(w, h);
        time_t start = time(NULL);
#pragma omp parallel for schedule(dynamic, 1)
        for (int y = 0; y < h; y++)
        {
            float elapsed = (time(NULL) - start), progress = (1. + y) / h;
            fprintf(stderr, "\rRendering (%d spp) %5.2f%% Time: %.2f/%.2f sec",
                    samples, progress * 100., elapsed, elapsed / progress);

            for (int x = 0; x < w; x++)
            {
                Vector3f color = Vector3f::ZERO;
                for (int s = 0; s < samples; s++)
                {
                    Ray r = camera->generateRay(Vector2f(x + RND1, y + RND1));
                    Vector3f ret = ray_color(r, scene, &root, obj_no_box, depth);
                    color += ret;
                }
                img.SetPixel(x, y, color / samples);
            }
        }
        img.SaveBMP(fout);
    }
};

#endif