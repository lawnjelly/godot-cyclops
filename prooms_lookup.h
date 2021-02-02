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

#include "core/local_vector.h"
#include "core/math/vector3.h"

namespace Lawn {

class PRoomManager;
class PRooms;

class PRoomsLookup {
	// most maps will be wider than they are high
	const int _grid_size_max = 64;
	float _grid_unit = 0.0f;
	Vector3 _grid_start;
	int _grid_size[3];
	int _x_times_y;

	struct Voxel {
		static const int _max_rooms = 4;
		static const uint16_t ROOM_NULL = 65535;
		// closest rooms to this voxel (or -1)
		uint16_t room_id[_max_rooms];
	};

	LocalVector<Voxel> _grid;

	Voxel &get_voxel(int p_x, int p_y, int p_z) {
		int e = (p_z * _x_times_y) + (p_y * _grid_size[0]) + p_x;
		return _grid[e];
	}
	const Voxel &get_voxel(int p_x, int p_y, int p_z) const {
		int e = (p_z * _x_times_y) + (p_y * _grid_size[0]) + p_x;
		return _grid[e];
	}

	const Voxel &find_nearest_voxel(Vector3 p_pos) const {
		p_pos -= _grid_start;
		p_pos /= _grid_unit;
		p_pos = p_pos.round();
		int x = p_pos.x;
		int y = p_pos.y;
		int z = p_pos.z;
		x = CLAMP(x, 0, _grid_size[0] - 1);
		y = CLAMP(y, 0, _grid_size[1] - 1);
		z = CLAMP(z, 0, _grid_size[2] - 1);
		return get_voxel(x, y, z);
	}

public:
	void create(const PRoomManager &p_room_manager);
	void clear();

	int find_room_within(const PRooms &p_rooms, const Vector3 &p_pos, int p_previous_room_id) const;

private:
	void fill_voxel(int p_x, int p_y, int p_z, const AABB &p_aabb, const PRooms &rooms);
};

} // namespace Lawn
