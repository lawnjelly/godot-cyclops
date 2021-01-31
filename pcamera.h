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

namespace Lawn {

class PRoomManager;

// trace source, can be a camera or light
class PCamera {
public:
	// same order as godot
	enum ePlane {
		P_NEAR,
		P_FAR,
		P_LEFT,
		P_TOP,
		P_RIGHT,
		P_BOTTOM,
		P_TOTAL,
	};

	// same order as godot
	enum ePoint {
		PT_FAR_LEFT_TOP,
		PT_FAR_LEFT_BOTTOM,
		PT_FAR_RIGHT_TOP,
		PT_FAR_RIGHT_BOTTOM,
		PT_NEAR_LEFT_TOP,
		PT_NEAR_LEFT_BOTTOM,
		PT_NEAR_RIGHT_TOP,
		PT_NEAR_RIGHT_BOTTOM,
	};

	bool update(const PRoomManager &p_room_manager);
	void clear() {
		_room_ID = -1;
	}

	const Vector3 &get_pos() const { return _pos; }
	const Vector3 &get_dir() const { return _dir; }
	int get_room_id() const { return _room_ID; }

	const Lawn::LVector<Plane> &get_planes() const { return _planes; }

	void set_pos(const Vector3 &p_pos) { _pos = p_pos; }
	void set_dir(const Vector3 &p_dir) { _dir = p_dir; }
	void set_room_id(int p_id) { _room_ID = p_id; }

	void set_godot_node_ID(ObjectID p_id) { _godot_camera_ID = p_id; }

private:
	// world space .. culling done in world space
	Vector3 _pos;
	Vector3 _dir;
	int _room_ID = -1;

	ObjectID _godot_camera_ID = -1;

	Lawn::LVector<Plane> _planes;
	Lawn::LVector<Vector3> _points;
};

} // namespace Lawn
