#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>

class Group : public Object3D
{
public:
    Group() : list(std::vector<Object3D *>()) {}

    explicit Group(int num_objects) : list(std::vector<Object3D *>())
    {
        for (int i = 0; i < num_objects; i++)
            list.push_back(nullptr);
    }

    ~Group() override
    {
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        float t = INFINITY;
        for (Object3D *obj : list)
        {
            if (obj->intersect(r, h, tmin) && h.getT() < t)
            {
                t = h.getT();
            }
        }
        return !std::isinf(t);
    }

    bool get_bounding_box(Box &box) const override
    {
        return false;
    }

    void addObject(int index, Object3D *obj)
    {
        list[index] = obj;
    }

    int getGroupSize() const
    {
        return list.size();
    }

    std::vector<Object3D *> list;
};

#endif
