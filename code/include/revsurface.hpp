#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include "box.hpp"
#include "define.h"
#include <tuple>

class RevSurface : public Object3D
{
    Curve *pCurve;
    Box box;

public:
    RevSurface(Curve *pCurve, Material *material) : Object3D(material), pCurve(pCurve)
    {
        // Check flat.
        for (const auto &cp : pCurve->getControls())
        {
            if (cp.z() != 0.0)
            {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        // set box
        box.setVertices(Vector3f(-pCurve->radius, pCurve->y_min - 3, -pCurve->radius),
                        Vector3f(pCurve->radius, pCurve->y_max + 3, pCurve->radius));
    }

    ~RevSurface() override
    {
        delete pCurve;
    }

    void getUV(const Ray &r, const float &t, float &theta, float &mu) const
    {
        Vector3f point(r.pointAtParameter(t));
        theta = atan2(-point.z(), point.x()) + M_PI;
        mu = (pCurve->y_max - point.y()) / (pCurve->y_max - pCurve->y_min);
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        // use bounding box to get initial value
        float t, theta, mu;
        if (!box.intersect(r, t) || t > h.getT())
        {
            return false;
        }
        getUV(r, t, theta, mu);
        Vector3f norm, point;
        if (!newton_method(r, t, theta, mu, norm, point))
        {
            return false;
        }
        if (!isnormal(mu) || !isnormal(theta) || !isnormal(t))
        {
            return false;
        }
        if (t < tmin || t > h.getT() || mu < pCurve->range[0] || mu > pCurve->range[1])
        {
            return false;
        }
        float u = theta / (2 * M_PI);
        float v = -(mu - pCurve->range[0]) / (pCurve->range[1] - pCurve->range[0]);
        h.set(t, material, norm.normalized(), material->getColor(u, v));
        return true;
    }

    bool get_bounding_box(Box &box) const override
    {
        box = this->box;
        return true;
    }

    Vector3f getPoint(const double &theta, const double &mu, Vector3f &dtheta, Vector3f &dmu) const
    {
        Vector3f point;
        Quat4f rot;
        rot.setAxisAngle(theta, Vector3f::UP);
        Matrix3f rotMat = Matrix3f::rotation(rot);
        CurvePoint cp = pCurve->getVT(mu);
        point = rotMat * cp.V;
        dmu = rotMat * cp.T;
        dtheta = Vector3f(-cp.V.x() * sin(theta), 0, -cp.V.x() * cos(theta));
        return point;
    }

    bool newton_method(const Ray &r, float &t, float &theta, float &mu, Vector3f &norm, Vector3f &point) const
    {
        Vector3f dmu, dtheta;
        for (int i = 0; i < NEWTON_STEPS; ++i)
        {
            if (theta < 0.0)
                theta += 2 * M_PI;
            if (theta >= 2 * M_PI)
                theta = fmod(theta, 2 * M_PI);

            if (mu >= pCurve->range[1])
                mu = pCurve->range[1] - DELTA;
            if (mu <= pCurve->range[0])
                mu = pCurve->range[0] + DELTA;

            point = getPoint(theta, mu, dtheta, dmu);
            Vector3f f = r.pointAtParameter(t) - point;
            double disSquare = f.squaredLength();
            norm = Vector3f::cross(dmu, dtheta);
            if (disSquare < NEWTON_EPSILON)
            {
                return true;
            }

            float dot = Vector3f::dot(r.getDirection(), norm);
            t -= Vector3f::dot(dmu, Vector3f::cross(dtheta, f)) / dot;
            mu -= Vector3f::dot(r.getDirection(), Vector3f::cross(dtheta, f)) / dot;
            theta += Vector3f::dot(r.getDirection(), Vector3f::cross(dmu, f)) / dot;
        }
        return false;
    }
};

#endif // REVSURFACE_HPP
