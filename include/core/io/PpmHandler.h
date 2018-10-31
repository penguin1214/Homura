/**
* Created by Jinglei Yang on 3/15/18.
* jinglei.yang.96@gmail.com
* Copyright © 2018 Jinglei Yang. All rights reserved.
*/
#ifndef HOMURA_PPMHANDLER_H
#define HOMURA_PPMHANDLER_H
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "../../core/math/Geometry.h"

namespace Homura {
	class PPMHandler {
	public:
		static void writePPM(char *fn, std::vector<Vec3f> buffer, int w, int h);
	};
}

#endif //HOMURA_PPMHANDLER_H
