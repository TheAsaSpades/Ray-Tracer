#pragma once
#include <algorithm>
#include <fstream>
#include <vector>

#include "Shape.h"
#include "Camera.h"



class RayTracer {
public:
	Camera cameras[2]; //0 = Left eye, 1 = right eye
	int width, height;

	RayTracer() { cameras[0] = Camera(); cameras[1] = Camera(); width = 500; height = 500; }
	RayTracer(Camera cam[2], float w, float h) { cameras[0] = cam[0], cameras[1] = cam[1]; width = w; height = h; }

	void trace(Sphere sphere, Quad quad, Vector3 light, int samples, const char * file)
	{
		std::ofstream out(file);
		out << "P3\n" << width * 2 << ' ' << height << ' ' << "255\n";
		Vector3 color;
		Camera camera = cameras[0];
		float offset = -width / 10.0f;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width * 2; x++) {

				//switch cameras for stereo
				if (x == width) {
					camera = cameras[1];
					offset = width / 10.0;
				}
				//reset color
				color = Vector3(0, 0, 0);
				for (int s = 0; s < samples; s++) {

					//find new random camera position
					float t = 2 * PI * (rand() / (float)RAND_MAX) * camera.aperature;
					float u = (rand() / (float)RAND_MAX)*camera.aperature + (rand() / (float)RAND_MAX) * camera.aperature;
					float r = u > camera.aperature ? 2 * camera.aperature - u : u;

					//create new ray
					Vector3 newPos = Vector3(camera.position.x + (r*cos(t)), camera.position.y + (r*sin(t)), camera.position.z);
					Ray ray = Ray(newPos, Vector3((x % width + offset), y, 0));

					//find intersections and set color
					float spT;
					Vector3 P;
					//Sphere *sph = checkSphereIntersect(ray, spheres);
					//Quad qd;
					bool sphInter = sphere.intersect(ray, spT);
					//if (sph != nullptr) {
					//	//sph = &spheres[0];
					//	sphInter = sph->intersect(ray, spT);
					//}
					bool qdInter = quad.intersect(ray, P);


					if (sphInter)
					{
						Vector3 pos = ray.origin + (ray.direction - ray.origin) * spT;
						Vector3 norm = (pos - sphere.center).normalize();
						float dif = std::max(0.0f, norm.dot((light - pos).normalize()));

						if(dif > 1 || dif < 0)
							printf("dif = %f x = %d, y = %d\n", dif,x,y);
						if (sphere.texMap)
							color = color + sphere.getTex(norm) * dif;
						else
							color = color + sphere.color *dif;
						//color = color + WHITE + WHITE;
					}
					 else if (qdInter)
					{
						float dif = std::max(0.0f, quad.normal.dot((light - P).normalize()));
						if (quad.texMap)
							color = color + quad.getTex(P) * dif;
						else
							color = color + quad.color * dif;
					}
					else
						color = color + BLACK;
				}

				// write out color
				out << (int)color.x / samples << ' '
					<< (int)color.y / samples << ' '
					<< (int)color.z / samples << '\n';
			}
			//reset cameras
			camera = cameras[0];
			offset = -width / 10.0f;
		}
	}


	Sphere* checkSphereIntersect(Ray ray, std::vector<Sphere> &spheres) {
		Sphere *sOut = nullptr;
		float minSphere = 10000000000, t;
		for (int s = 0; s < spheres.size(); s++) {
			if (spheres[s].intersect(ray, t)) {
				if (t < minSphere) {
					minSphere = t;
					sOut = &spheres[s];
				}
			}
		}
		return sOut;
	}

	bool checkQuadIntersect(Ray ray, std::vector<Quad> quads, Quad *qOut) {
		Vector3 minQuad = Vector3(100000000000, 10000000000000, 100000000000), P;
		for (int q = 0; q < quads.size(); q++) {
			if (quads[q].intersect(ray, P)) {
				if (P.z < minQuad.z) {
					minQuad = P;
					qOut = &quads[q];
				}
			}
		}
		return (qOut != NULL);
	}
	
	
};
