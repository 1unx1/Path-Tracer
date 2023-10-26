#ifndef BVH_H
#define BVH_H

#include <vecmath.h>
#include <vector>
#include <random>
#include <algorithm>
#include "ray.hpp"
#include "define.h"
#include "object3d.hpp"
#include "box.hpp"

bool compare(const Object3D *a, const Object3D *b, int axis)
{
    Box aBox, bBox;
    if (!a->get_bounding_box(aBox) || !b->get_bounding_box(bBox))
    {
        std::cerr << "No bounding box in Object3D!\n";
    }
    return aBox.lo[axis] < bBox.lo[axis];
}

bool compare_x(const Object3D *a, const Object3D *b)
{
    return compare(a, b, 0);
}

bool compare_y(const Object3D *a, const Object3D *b)
{
    return compare(a, b, 1);
}

bool compare_z(const Object3D *a, const Object3D *b)
{
    return compare(a, b, 2);
}

class BVHNode : public Object3D
{
public:
    Object3D *lNode;
    Object3D *rNode;
    Box box;

    BVHNode(std::vector<Object3D *> &objects, int begin, int end)
    {
        int axis = int(rand() / (RAND_MAX + 1.0) * 3);
        assert(axis >= 0 && axis <= 2);
        auto comparator = axis == 0 ? compare_x : (axis == 1 ? compare_y : compare_z);

        int size = end - begin;
        if (size == 1)
        {
            lNode = rNode = objects[begin];
        }
        else if (size == 2)
        {
            if (comparator(objects[begin], objects[begin + 1]))
            {
                lNode = objects[begin];
                rNode = objects[begin + 1];
            }
            else
            {
                lNode = objects[begin + 1];
                rNode = objects[begin];
            }
        }
        else
        {
            std::sort(objects.begin() + begin, objects.begin() + end, comparator);
            int mid = begin + size / 2;
            lNode = new BVHNode(objects, begin, mid);
            rNode = new BVHNode(objects, mid, end);
        }

        Box lBox, rBox;
        if (!lNode->get_bounding_box(lBox) || !rNode->get_bounding_box(rBox))
        {
            std::cerr << "No bounding box in Object3D!\n";
        }

        box.surrounding(lBox, rBox);
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        float t_min;
        if (!box.intersect(r, t_min))
        {
            return false;
        }
        bool lHit = lNode->intersect(r, h, t_min);
        bool rHit = rNode->intersect(r, h, t_min);
        return lHit || rHit;
    }

    bool get_bounding_box(Box &box) const override
    {
        box = this->box;
        return true;
    }
};

#endif