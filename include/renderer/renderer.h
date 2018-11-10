#ifndef HOMURA_RENDERER_H_
#define HOMURA_RENDERER_H_

#include <iostream>
#include <chrono>
#include "Scene.h"
#include "primitives/TriangleMesh.h"
#include "sensors/Orthographic.h"
#include "../core/io/PpmHandler.h"
#include "core/io/JsonDocument.h"
#include "renderer/integrators/Whitted.h"
#include "renderer/integrators/DirectIntegrator.h"
#include "renderer/integrators/PathTraceIntegrator.h"

namespace Homura {
	class Renderer {
	public:
		std::shared_ptr<Scene> _scene;
		std::unique_ptr<Integrator> _integrator;
	public:
		Renderer(std::shared_ptr<Scene> scene, std::unique_ptr<Integrator> integrator)
			: _scene(std::move(scene)), _integrator(std::move(integrator)) {}

		Renderer(std::string file_path) {
			JsonDocument scene_document(file_path);
			_scene = std::make_shared<Scene>(scene_document);

			auto integrator_json = scene_document["integrator"];
			std::string integrator_type = integrator_json["type"].getString();

			if (integrator_type == "whitted")
				_integrator = std::unique_ptr<Integrator>(new Whitted(std::shared_ptr<Scene>(_scene), integrator_json));
			else if (integrator_type == "direct")
				_integrator = std::unique_ptr<Integrator>(new DirectIntegrator(std::shared_ptr<Scene>(_scene), integrator_json));
			else if (integrator_type == "path_trace")
				_integrator = std::unique_ptr<Integrator>(new PathTraceIntegrator(std::shared_ptr<Scene>(_scene), integrator_json));
			else
				;
		}

		void render() {
			if (_integrator == nullptr) {
				std::cout << "Integrator not initialized! Stop rendering." << std::endl;
				return;
			}
			std::cout << "Start rendering..." << std::endl;

			auto start = std::chrono::high_resolution_clock::now();
			_integrator->render();
			auto stop = std::chrono::high_resolution_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
			std::cout << "Rendering Time: " << duration.count() << "s." << std::endl;
		}
	};
}


#endif // !HOMURA_RENDERER_H_
