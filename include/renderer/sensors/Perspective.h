/**
 * Created by Jinglei Yang on 8/18/18.
 * jinglei.yang.96@gmail.com
 * Copyright © 2018 Jinglei Yang. All rights reserved.
 */
#ifndef HOMURA_PERSPECTIVE_H_
#define HOMURA_PERSPECTIVE_H_

#include "Sensor.h"

namespace Homura {
    class PerspectiveSensor: public ProjectiveSensor {
    public:
		float _vfov;	// vertical fov, in degree.
        Vec3f _dxSensor, _dySensor;

        PerspectiveSensor(const JsonObject json);

        float generatePrimaryRay(const PixelSample &sample, Ray &r) const override;
    };
}
#endif //HOMURA_PERSPECTIVE_H_
