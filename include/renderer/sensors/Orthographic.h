/**
 * Created by Jinglei Yang on 3/13/18.
 * jinglei.yang.96@gmail.com
 * Copyright © 2018 Jinglei Yang. All rights reserved.
 */
#ifndef HOMURA_ORTHOGRAPHIC_H_
#define HOMURA_ORTHOGRAPHIC_H_

#include "Sensor.h"

namespace Homura {
    class OrthographicSensor : public ProjectiveSensor {
    public:
        Vec3f _dxSensor, _dySensor;

        OrthographicSensor(const Mat4f &ctw, Film *film, Bound2f screen_window = Bound2f(Point3f(-1, -1, 0), Point3f(1, 1, 0)));
		OrthographicSensor(const JsonObject json);

        float generatePrimaryRay(const PixelSample &sample, Ray &r) const override;
    };
}

#endif //HOMURA_ORTHOGRAPHIC_H_
