/// TODO: clean up code in this file!!!

//#include <iostream>
//#include "core/math/Geometry.h"
//#include "renderer/renderer.h"
//#include "core/sampler/StratifiedSampler.h"
//#include "renderer/integrators/Whitted.h"
//#include "GUI/opengl_utils.h"
//#include "core/io/ObjHandler.h"
//
//using namespace Homura;
//
//std::vector<Vec2f> samples;
//
//void displayFunc() {
//	glClearColor(1.0, 1.0, 1.0, 0.0);
//
//	glColor3f(0.0, 0.0, 0.0);
//	glLineWidth(1.0);
//
//	drawGrid(0.0, 10.0, 0.0, 10.0, 1.0f, 1.0f);
//
//	glColor3f(0.0f, 0.0f, 1.0f); //blue color
//	glPointSize(3.0f);//set point size to 10 pixels
//
//	glBegin(GL_POINTS);
//	for (int i = 0; i < samples.size(); i++) {
//		glVertex2f(samples[i][0], samples[i][1]);
//	}
//	glEnd();
//
//	glFlush();
//}
//
//void drawSampleDiag() {
//	char *myargv[1];
//	int myargc = 1;
//	myargv[0] = _strdup("Homura");
//	glutInit(&myargc, myargv);
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//	glutInitWindowSize(800, 800);
//	glutCreateWindow("Homura");
//	glViewport(-1.0, -1.0, 12.0, 12.0);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(-1.0, 11.0, -1.0, 11.0);
//	glutDisplayFunc(displayFunc);
//	glutMainLoop();
//}
//
//int main() {
//#if 0
//	int w = 100; int h = 100;
//	Buffer *b = new Buffer(w, h);
//	Film *film = new Film(w, h, b);
//	Bound2f screen_window(Point3f(-1, -1, 0), Point3f(1, 1, 0));
//	Mat4f cam2world;
//	cam2world = cam2world * cam2world.translate(Vec3f(0, 0, 1));
//	cam2world = cam2world * cam2world.rotateXYZ(Vec3f(0, 0, 0));
//	OrthographicSensor *cam = new OrthographicSensor(cam2world, screen_window, film);
//	Scene *scene = new Scene(cam);
//	std::vector<Point3f> verts = { Point3f(-1,-1,-3), Point3f(1,-1,-3), Point3f(0,1,-3), Point3f(0, 0, -5), Point3f(4, 0, -5), Point3f(3, 3, -5) };
//	std::vector<Vec3i> idxs = { Vec3i(0,1,2), Vec3i(3,4,5) };
//	TriangleMesh triangles(verts, idxs);
//	TriangleMesh *triangles_ptr = &triangles;
//	scene->_shapes.push_back(triangles_ptr);
//	StratifiedSampler *sampler = new StratifiedSampler(1, 1, true, 4);
//	Whitted *integrator = new Whitted(1, scene, sampler);
//
//	/// Second phase: main rendering loop
//	/// Camera+Sampler -> ray -> Li() -> Film
//	Homura::Renderer renderer(scene, integrator);
//	renderer.render();
//#else
//	std::vector<Point3f> verts;
//	std::vector<Vec3i> indecies;
//	std::vector<Vec3f> normals;
//	ObjHandler::readObj("D:/Scenes/cube.obj", verts, indecies, normals);
//
//	std::cout << indecies.size() << std::endl;
//	for (int i = 0; i < verts.size(); i++) {
//		std::cout << verts[i] << std::endl;
//		std::cout << indecies[i] << std::endl;
//	}
//#endif
//
//	return 0;
//}
//
