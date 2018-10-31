/**
* Created by Jinglei Yang on 3/15/18.
* jinglei.yang.96@gmail.com
* Copyright © 2018 Jinglei Yang. All rights reserved.
*/
#include "core/io/PpmHandler.h"

namespace Homura {
	void PPMHandler::writePPM(char *fn, std::vector<Vec3f> buffer, int w, int h) {
		/*
		FILE *fp = fopen(fn, "wb");
		unsigned char *char_buffer = new unsigned char[w*h*3];
		for (int i = 0; i < w*h; i+=3) {
		char_buffer[i] = (unsigned char)buffer[i].x();
		char_buffer[i+1] = (unsigned char)buffer[i].y();
		char_buffer[i+2] = (unsigned char)buffer[i].z();
		}
		fprintf(fp, "P6\n%d %d\n\255\n", w, h);
		fwrite(char_buffer, sizeof(unsigned char), w*h*3, fp);
		fclose(fp);
		*/
		std::ofstream ofs(fn, std::ios::out | std::ios::binary);
		ofs << "P6\n" << w << " " << h << "\n255\n";
		for (int i = 0; i < w*h; i++) {
			ofs << (unsigned char)(buffer[i].x());
			ofs << (unsigned char)(buffer[i].y());
			ofs << (unsigned char)(buffer[i].z());
		}
		ofs.flush();
	}
}