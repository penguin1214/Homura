/**
 * Created by Jinglei Yang on 3/11/18.
 * jinglei.yang.96@gmail.com
 * Copyright © 2018 Jinglei Yang. All rights reserved.
 */
#include "renderer/Buffer.h"
#include <iostream>

namespace Homura{
    Buffer::Buffer(int w, int h) : _width(w), _height(h) {
        _data.resize(_width*_height);   // TODO: failed with reserve()?
    }

    Buffer::~Buffer() {}

    void Buffer::addSample(int index, Vec3f value) {
        _data[index] += value;
    }
}
