#pragma once

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

#include "lvector.h"
#include "pdobs.h"
#include "scene/3d/spatial.h"

namespace Lawn {
class LBitField_Dynamic;
}

class LPortal;

class LRoom : public Spatial {
	GDCLASS(LRoom, Spatial);

public:
	// returns distance to closest plane, if 0 or negative, inside
	float is_point_within(const Vector3 &p_pos) const;

	int get_num_portals() const { return _portals.size(); }
	LPortal *get_portal(int p_id) { return _portals[p_id]; }
	void add_portal(LPortal *p_portal) { _portals.push_back(p_portal); }

	void clear();
	void show_debug(bool p_show);

private:
	// note that the room can also contain portals which are in another room
	// pointing into this one
	Lawn::LVector<LPortal *> _portals;

public:
	Lawn::PDobList _doblist;
	int _first_static = 0;
	int _num_statics = 0;
	int _room_ID = -1;

	// makes sure lrooms are not converted more than once per
	// call to rooms_convert
	int _conversion_tick = -1;

	// planes forming convex hull of room
	Lawn::LVector<Plane> _planes;
	Geometry::MeshData _bound_mesh_data;
	
	AABB _aabb;
};
