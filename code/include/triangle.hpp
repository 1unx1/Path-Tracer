#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

class Triangle : public Object3D
{

public:
	Triangle() = delete;

	// a b c are three vertex positions of the triangle
	Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m)
		: Object3D(m), vertices({a, b, c}), setVN(false), vNorms({Vector3f::ZERO, Vector3f::ZERO, Vector3f::ZERO})
	{
		Vector3f e1 = a - b;
		Vector3f e2 = a - c;
		Vector3f cross = Vector3f::cross(e1, e2);
		if (cross == Vector3f::ZERO)
		{
			// TODO, error
		}
		else
		{
			float x = 0, y = 0, z = 0;
			if (e1.x() * e2.y() - e2.x() * e1.y() != 0)
			{
				x = cross[0] / cross[2];
				y = cross[1] / cross[2];
				z = 1;
			}
			else if (cross[1] != 0)
			{
				x = cross[0] / cross[1];
				y = 1;
				z = cross[2] / cross[1];
			}
			else if (cross[0] != 0)
			{
				x = 1;
				y = cross[1] / cross[0];
				z = cross[2] / cross[0];
			}
			normal = Vector3f(x, y, z).normalized();
		}
	}

	void setVNorms(const Vector3f &a, const Vector3f &b, const Vector3f &c)
	{
		vNorms[0] = a;
		vNorms[1] = b;
		vNorms[2] = c;
	}

	bool intersect(const Ray &r, Hit &h, float tmin) override
	{
		Vector3f e1 = vertices[0] - vertices[1];
		Vector3f e2 = vertices[0] - vertices[2];
		Vector3f s = vertices[0] - r.getOrigin();
		const Vector3f &d = r.getDirection();
		float div = Matrix3f(d, e1, e2).determinant();
		if (div == 0)
			return false;
		float t = Matrix3f(s, e1, e2).determinant() / div;
		float beta = Matrix3f(d, s, e2).determinant() / div;
		float gamma = Matrix3f(d, e1, s).determinant() / div;

		if (t > 0 && t < h.getT() && t >= tmin && 0 <= beta && beta <= 1 && 0 <= gamma && gamma <= 1 && beta + gamma <= 1)
		{
			Vector3f n;
			if (setVN)
			{
				Vector3f p = r.pointAtParameter(t);
				Vector3f v0 = vertices[0] - p, v1 = vertices[1] - p, v2 = vertices[2] - p;
				float s0 = Vector3f::cross(v1, v2).length(),
					  s1 = Vector3f::cross(v2, v0).length(),
					  s2 = Vector3f::cross(v0, v1).length();
				n = (s0 * vNorms[0] + s1 * vNorms[1] + s2 * vNorms[2]).normalized();
			}
			else
			{
				n = normal;
			}
			h.set(t, material, n, material->diffuseColor);
			return true;
		}
		else
			return false;
	}

	bool get_bounding_box(Box &box) const override
	{
		float x_min = vertices[0].x(), x_max = vertices[0].x();
		float y_min = vertices[0].y(), y_max = vertices[0].y();
		float z_min = vertices[0].z(), z_max = vertices[0].z();

		for (int i = 0; i < 3; i++)
		{
			if (vertices[i].x() > x_max)
			{
				x_max = vertices[i].x();
			}
			if (vertices[i].x() < x_min)
			{
				x_min = vertices[i].x();
			}
			if (vertices[i].y() > y_max)
			{
				y_max = vertices[i].y();
			}
			if (vertices[i].y() < y_min)
			{
				y_min = vertices[i].y();
			}
			if (vertices[i].z() > z_max)
			{
				z_max = vertices[i].z();
			}
			if (vertices[i].z() < z_min)
			{
				z_min = vertices[i].z();
			}
		}

		box.lo = Vector3f(x_min, y_min, z_min);
		box.hi = Vector3f(x_max, y_max, z_max);
		return true;
	}

	Vector3f normal;
	Vector3f vertices[3];
	bool setVN;
	Vector3f vNorms[3];
};

#endif // TRIANGLE_H
