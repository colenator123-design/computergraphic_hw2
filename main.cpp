// NCCU CG23 Hw2 ray tracing step2

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cfloat> //FLT_MAX

#include "vec3.h"
#include "ray.h"
#include "geo.h"

float MAX(float a, float b) { return (a > b) ? a : b; }
using namespace std;
int MAX_STEP = 5;

vec3 shading(vec3 &lightsource, vec3 &intensity, hit_record ht, vec3 kd, const vector<sphere> &list)
{
	/*
		To-do:
		define L, N by yourself
	*/
	vec3 L = unit_vector(lightsource - ht.p);
	vec3 N = unit_vector(ht.nv);
	ray shadowRay(ht.p, L);

	int intersect = -1;
	hit_record rec;
	float closest = FLT_MAX;
	/*
		To-do:
		To find whether the shadowRay hit other object,
		you should run the function "hit" of all the hitable you created
	*/
	for (unsigned int i = 0; i < list.size(); i++)
	{
		hit_record tmp;
		if (list[i].hit(shadowRay, 0.0001f, closest, tmp))
		{
			intersect = 1;
			if (tmp.t < closest)
			{
				closest = tmp.t;
				rec = tmp;
			}
		}
	}

	if (intersect == -1)
	{
		return kd * intensity * MAX(0, dot(N, unit_vector(L)));
	}
	else
	{
		return vec3(0, 0, 0);
	}
}

vec3 skybox(const ray &r)
{
	vec3 uni_direction = unit_vector(r.direction());
	float t = 0.5 * (uni_direction.y() + 1);

	return (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1.0);
}

vec3 trace(const ray &r, const vector<sphere> &list, int depth)
{
	if (depth >= MAX_STEP)
	{
		return skybox(r); // or return vec3(0, 0, 0);
	}

	int intersect = -1;
	hit_record rec;
	float closest = FLT_MAX;
	/*
		To-do:
		To find the nearest object from the origin of the ray,
		you should run the function "hit" of all the hitable you created
	*/
	for (unsigned int i = 0; i < list.size(); i++)
	{
		hit_record tmp;
		if (list[i].hit(r, 0.0001f, closest, tmp))
		{
			if (tmp.t < closest)
			{
				intersect = i;
				closest = tmp.t;
				rec = tmp;
			}
		}
	}

	if (intersect != -1)
	{
		/*
			To-do:
			1.compute the local color by shading function
			2.compute the reflected color by
				2.1 compute the reflected direction
				2.2 define a reflected ray by rec.p and the direction in 2.1
				2.3 run trace(reflected_ray, list, depth+1);
			3.compute the transmitted color by
				3.1 compute the transmitted direction by Snell's law
				3.2 define a transmitted ray by rec.p and the direction in 3.1
				3.3 run trace( transmitted_ray, list, depth+1 );
			4.return the color by the parameter w_r, w_t and the 3 color you
		   computed.
		*/
		vec3 lightPos = vec3(-10, 10, 0), lightIntensity = vec3(1.0, 1.0, 1.0);
		vec3 localColor, reflectColor, transColor;
		localColor =
			shading(lightPos, lightIntensity, rec, list[intersect].kd, list);
		ray reflectRay = ray(rec.p, reflect(rec.p - r.O, rec.nv));
		reflectColor = trace(reflectRay, list, depth + 1);
		float n = (dot(r.D, rec.nv) > 0.0f) ? 8.0f : 1.0f / 8.0f;
		ray refractRay = ray(rec.p, refract(rec.p - r.O, rec.nv, n));
		transColor = trace(refractRay, list, depth + 1);

		vec3 color =
			(1.0f - list[intersect].w_r - list[intersect].w_t) * localColor +
			list[intersect].w_r * reflectColor +
			list[intersect].w_t * transColor;
		return color;
	}
	else
	{
		return skybox(r);
	}
}

int main()
{
	int width = 200;
	int height = 100;
	srand(time(NULL));

	// camera and projection plane
	vec3 lower_left_corner(-2, -1, -1);
	vec3 origin(0, 0, 0);
	vec3 horizontal(4, 0, 0);
	vec3 vertical(0, 2, 0);

	vec3 colorlist[8] = {vec3(0.8, 0.3, 0.3), vec3(0.3, 0.8, 0.3), vec3(0.3, 0.3, 0.8),
						 vec3(0.8, 0.8, 0.3), vec3(0.3, 0.8, 0.8), vec3(0.8, 0.3, 0.8),
						 vec3(0.8, 0.8, 0.8), vec3(0.3, 0.3, 0.3)};

	// test scene with spheres
	vector<sphere> hitable_list;
	hitable_list.push_back(sphere(vec3(0, -100.5, -2), 100)); // ground
	hitable_list.push_back(sphere(vec3(0, 0, -2), 0.5, vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.9f));
	hitable_list.push_back(sphere(vec3(1, 0, -1.75), 0.5, vec3(1.0f, 1.0f, 1.0f), 0.9f, 0.0f));
	hitable_list.push_back(sphere(vec3(-1, 0, -2.25), 0.5, vec3(1.0f, 0.7f, 0.3f), 0.0f, 0.0f));
	for (int i = 0; i < 48; i++) {
		float xr = ((float)rand() / (float)(RAND_MAX)) * 6.0f - 3.0f;
		float zr = ((float)rand() / (float)(RAND_MAX)) * 3.0f - 1.5f;
		int cindex = rand() % 8;
		float rand_reflec = ((float)rand() / (float)(RAND_MAX));
		//float rand_refrac = ((float)rand() / (float)(RAND_MAX));
		hitable_list.push_back(sphere(vec3(xr, -0.4, zr - 2), 0.1, colorlist[cindex], rand_reflec, 0.0f));
	}

	fstream file;
	file.open("ray.ppm", ios::out);
	file << "P3\n"
		 << width << " " << height << "\n255\n";
	for (int j = height - 1; j >= 0; j--)
	{
		for (int i = 0; i < width; i++)
		{
			float u = float(i) / float(width);
			float v = float(j) / float(height);
			ray r(origin, lower_left_corner + u * horizontal + v * vertical);
			vec3 c = trace(r, hitable_list, 0);

			file << (int)(c.r() * 255) << " "
				 << (int)(c.g() * 255) << " "
				 << (int)(c.b() * 255) << endl;
		}
	}

	return 0;
}
