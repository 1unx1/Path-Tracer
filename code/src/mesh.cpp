#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

bool Mesh::intersect(const Ray &r, Hit &h, float tmin)
{

    // Optional: Change this brute force method into a faster one.
    bool result = false;
    for (int triId = 0; triId < (int)t.size(); ++triId)
    {
        TriangleIndex &triIndex = t[triId];
        Triangle triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);
        triangle.normal = n[triId];
        if (!vn.empty())
        {
            triangle.setVN = true;
            triangle.setVNorms(vn[triIndex[0]], vn[triIndex[1]], vn[triIndex[2]]);
        }
        result |= triangle.intersect(r, h, tmin);
    }
    return result;
}

bool Mesh::get_bounding_box(Box &box) const
{
    float x_min = v[0].x(), x_max = v[0].x();
    float y_min = v[0].y(), y_max = v[0].y();
    float z_min = v[0].z(), z_max = v[0].z();

    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i].x() > x_max)
        {
            x_max = v[i].x();
        }
        if (v[i].x() < x_min)
        {
            x_min = v[i].x();
        }
        if (v[i].y() > y_max)
        {
            y_max = v[i].y();
        }
        if (v[i].y() < y_min)
        {
            y_min = v[i].y();
        }
        if (v[i].z() > z_max)
        {
            z_max = v[i].z();
        }
        if (v[i].z() < z_min)
        {
            z_min = v[i].z();
        }
    }

    box.lo = Vector3f(x_min, y_min, z_min);
    box.hi = Vector3f(x_max, y_max, z_max);
    return true;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material)
{

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open())
    {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    std::string vnTok("vn");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true)
    {
        std::getline(f, line);
        if (f.eof())
        {
            break;
        }
        if (line.size() < 3)
        {
            continue;
        }
        if (line.at(0) == '#')
        {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok)
        {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        }
        else if (tok == fTok)
        {
            if (line.find(bslash) != std::string::npos)
            {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++)
                {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                t.push_back(trig);
            }
            else
            {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++)
                {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                t.push_back(trig);
            }
        }
        else if (tok == texTok)
        {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        }
        else if (tok == vnTok)
        {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            vn.push_back(vec);
        }
    }
    computeNormal();

    f.close();
}

void Mesh::computeNormal()
{
    n.resize(t.size());
    for (int triId = 0; triId < (int)t.size(); ++triId)
    {
        TriangleIndex &triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}
