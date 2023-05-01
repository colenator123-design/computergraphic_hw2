#ifndef GEOH
#define GEOH
#include "ray.h"

using namespace std;

class material;

typedef struct hit_record
{
	float t;
	vec3 p;
	vec3 nv;
} hit_record;

// geometry parent class
class hitable
{
public:
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const = 0;
};

class sphere : public hitable
{
public:
	sphere() {}
	sphere(vec3 c, float r, vec3 _kd = vec3(1.0, 1.0, 1.0), float w_ri = 0.0f, float w_ti = 0.0f) : center(c), radius(r), kd(_kd), w_r(w_ri), w_t(w_ti){};
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;

	vec3 center;
	float radius;
	vec3 kd;
	float w_r; // reflected
	float w_t; // transmitted
};

bool sphere::hit(const ray &r, float tmin, float tmax, hit_record &rec) const
{
	/*
	To-do:
		compute whether the ray intersect the sphere
	*/
	float A = dot(r.D, r.D);
	float B = 2.0 * dot(r.D, r.O - center);
	float C = dot(center - r.O, center - r.O) - radius * radius;
	float t = (-B - sqrt(B * B - 4.0 * A * C)) / (2.0 * A);
	rec.t = -1;
	if ((B * B - 4.0 * A * C) < 0.0f)
	{
		return false;
	}
	if (t >= tmax || t <= tmin)
	{
		if (t >= tmax || t <= tmin)
			return false;
	}
	rec.t = t;
	rec.p = r.point_at_parameter(t);
	rec.nv = r.point_at_parameter(t) - center;
	return true;
}

vec3 reflect(const vec3 &d, const vec3 &nv)
{
	// compute the reflect direction
	// To-do
	return (2 * nv * dot((nv), (-d)) + d);
}

vec3 refract(const vec3 &v, const vec3 &n, float r)
{
	// compute the refracted direction
	// To-do
	double cosI = -dot(n, v);
	double sinT2 = r * r * (1.0 - cosI * cosI);
	double cosT = sqrt(1.0 - sinT2);
	return (r * v + (r * cosI - cosT) * n);
}

#endif