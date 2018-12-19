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
		float _area;

        PerspectiveSensor(const JsonObject json);

        float generatePrimaryRay(const SensorSample &sample, Ray &r) const override;

		Vec3f We(const Ray &r, Point2f *p_raster = nullptr) const override;
		void Pdf_We(const Ray &r, float &pdf_pos, float &pdf_dir) const override;
		Vec3f sample_Wi(const IntersectInfo &isect_info, const Point2f &u, Vec3f &wi, float &pdf, Point2f *p_raster, VisibilityTester *vt) const override;
    };
}
#endif //HOMURA_PERSPECTIVE_H_
