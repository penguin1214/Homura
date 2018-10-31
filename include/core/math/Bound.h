/**
* Created by Jinglei Yang on 3/13/18.
* jinglei.yang.96@gmail.com
* Copyright © 2018 Jinglei Yang. All rights reserved.
*/
#ifndef HOMURA_BOUND_H
#define HOMURA_BOUND_H

#include "Geometry.h"

namespace Homura {
	class Bound2f {
	public:
		Point3f _min, _max;

		Bound2f(Point3f min, Point3f max) : _min(min), _max(max) {}
	};

	////////////////////////////////////////////////////////

	class Bound3 {};
}

#endif //HOMURA_BOUND_H
