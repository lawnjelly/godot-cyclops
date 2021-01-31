//	Copyright (c) 2021 Lawnjelly

//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:

//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.

//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#include "lroom.h"

float LRoom::is_point_within(const Vector3 &p_pos) const {
	// inside by default
	float closest_dist = -FLT_MAX;

	for (int n = 0; n < _planes.size(); n++) {
		float dist = _planes[n].distance_to(p_pos);
		if (dist > closest_dist) {
			closest_dist = dist;
		}
	}

	return closest_dist;
}

void LRoom::clear() {
	_portals.clear(true);
	_planes.clear(true);
	_doblist.clear();

	_first_static = 0;
	_num_statics = 0;
	_room_ID = -1;
}

void LRoom::show_debug(bool p_show) {
	for (int n = 0; n < _portals.size(); n++) {
		_portals[n]->show_debug(p_show);
	}
}
