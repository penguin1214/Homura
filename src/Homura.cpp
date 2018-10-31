#include <iostream>
#include <core/math/Geometry.h>
#include <core/sampler/StratifiedSampler.h>
#include <renderer/renderer.h>
#include <renderer/integrators/Whitted.h>
#include <core/io/JsonDocument.h>
#include "rapidjson/document.h"
#include "core/io/FileUtil.h"
#include "core/math/CoordinateSystem.h"
#include <string>
#include "renderer/Ray.h"

using namespace Homura;

int main() {
	//int w = 100; int h = 100;
	//Buffer *b = new Buffer(w, h);
	//Film *film = new Film(w, h, b);
	//Bound2f screen_window(Point3f(-1, -1, 0), Point3f(1, 1, 0));
	//Mat4f cam2world;
	//// compute cam2world matrix according to lookat matrix
	//cam2world = cam2world * cam2world.translate(Vec3f(0, 0, 1));
	//cam2world = cam2world * cam2world.rotateXYZ(Vec3f(0, 0, 0));
	//OrthographicSensor *cam = new OrthographicSensor(cam2world, film, screen_window);
	//Scene *scene = new Scene(cam);
	//std::vector<Point3f> verts = { Point3f(-1,-1,-3), Point3f(1,-1,-3), Point3f(0,1,-3), Point3f(0, 0, -5), Point3f(4, 0, -5), Point3f(3, 3, -5) };
	//std::vector<Vec3i> idxs = { Vec3i(0,1,2), Vec3i(3,4,5) };
	//TriangleMesh triangles(verts, idxs);
	//TriangleMesh *triangles_ptr = &triangles;
	//scene->_shapes.push_back(triangles_ptr);
	//StratifiedSampler *sampler = new StratifiedSampler(1, 1, true, 4);
	//Whitted *integrator = new Whitted(1, scene, sampler);

	/// Second phase: main rendering loop
	/// Camera+Sampler -> ray -> Li() -> Film
	//Homura::Renderer renderer("/Users/jinglei/Code/Homura/scenes/scene.json");
	Homura::Renderer renderer("C:/Users/Jingl/Code/Homura/scenes/scene.json");
	renderer.render();
//	JsonDocument json("D:/Code/Homura/scenes/scene.json");
//	Mat4f lookat;
//	json["sensor"]["transform"].fill(lookat);
//	std::cout << lookat << std::endl;
//	Point3f cam = lookat * Point3f(0, 0, 0);
//	std::cout << cam << std::endl;
}