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

#include "prooms_lookup.h"
#include "proom_manager.h"

using namespace Lawn;

void PRoomsLookup::clear() {
	_grid_unit = 0.0f;
	_grid_start = Vector3();
	_grid_size[0] = 0;
	_grid_size[1] = 0;
	_grid_size[2] = 0;
	_x_times_y = 0;
}

int PRoomsLookup::find_room_within(const PRooms &p_rooms, const Vector3 &p_pos, int p_previous_room_id) const {
	float closest = FLT_MAX;
	int closest_room_id = -1;

	// first try previous room
	if (p_previous_room_id != -1) {
		closest = p_rooms.get_room(p_previous_room_id)->is_point_within(p_pos);
		closest_room_id = p_previous_room_id;

		if (closest < 0.0f)
			return p_previous_room_id;
	}

	// local
	int num_rooms = p_rooms.get_num_rooms();

	// keep track of which rooms we have already checked ..
	// on stack so in cache.
	uint8_t *rooms_checked = (uint8_t *)alloca(num_rooms);
	zeromem(rooms_checked, num_rooms);

	// mark previous room as checked
	if (p_previous_room_id != -1) {
		rooms_checked[p_previous_room_id] = 1;
	}

	// go through the list of the voxel
	const Voxel &voxel = find_nearest_voxel(p_pos);

	// this goes through a list of the closest rooms to the voxel.
	// and finds the closest of these rooms to the point.
	// but terminates when it is actually inside a room, which should be
	// the most common case.
	for (int n = 0; n < Voxel::_max_rooms; n++) {
		uint16_t room_id = voxel.room_id[n];

		if (room_id == Voxel::ROOM_NULL)
			continue;

		if (room_id == p_previous_room_id)
			continue;

		// mark room as checked
		rooms_checked[room_id] = 1;

		const LRoom *lroom = p_rooms.get_room(room_id);
		float dist = lroom->is_point_within(p_pos);
		if (dist < closest) {
			closest = dist;
			closest_room_id = room_id;

			// if we are actually inside a room, terminate early, no need to search more
			if (dist < 0.0f)
				break;
		}
	}

	if (closest < 0.0f)
		return closest_room_id;

	// last ditch attempt, try all rooms.
	// this will be slow but should very rarely run
	for (int n = 0; n < p_rooms.get_num_rooms(); n++) {

		// already checked? ignore
		if (rooms_checked[n])
			continue;

		const LRoom *lroom = p_rooms.get_room(n);
		float dist = lroom->is_point_within(p_pos);
		if (dist < closest) {
			closest = dist;
			closest_room_id = n;
		}
	}

	return closest_room_id;
}

void PRoomsLookup::create(const PRoomManager &p_room_manager) {
	const PRooms &rooms = p_room_manager._rooms;

	// noop
	if (!rooms.get_num_rooms())
		return;

	AABB world_aabb = rooms.get_room(0)->_aabb;

	// first calculate aabb of all rooms
	for (int n = 1; n < rooms.get_num_rooms(); n++) {
		world_aabb.merge_with(rooms.get_room(n)->_aabb);
	}

	// the unit size is defined by the longest axis
	_grid_unit = world_aabb.get_longest_axis_size() / _grid_size_max;
	_grid_start = world_aabb.position;

	// these will be approximate .. it doesn't matter
	_grid_size[0] = (world_aabb.size.x / _grid_unit) + 1;
	_grid_size[1] = (world_aabb.size.y / _grid_unit) + 1;
	_grid_size[2] = (world_aabb.size.z / _grid_unit) + 1;
	_x_times_y = _grid_size[0] * _grid_size[1];

	_grid.resize(_x_times_y * _grid_size[2]);

	// now fill each grid square
	AABB bb;
	bb.size = Vector3(_grid_unit, _grid_unit, _grid_unit);

	for (int z = 0; z < _grid_size[2]; z++) {
		for (int y = 0; y < _grid_size[1]; y++) {
			for (int x = 0; x < _grid_size[0]; x++) {
				bb.position = Vector3(x * _grid_unit, y * _grid_unit, z * _grid_unit);
				bb.position += _grid_start;

				fill_voxel(x, y, z, bb, rooms);
			}
		}
	}
}

void PRoomsLookup::fill_voxel(int p_x, int p_y, int p_z, const AABB &p_aabb, const PRooms &rooms) {
	Voxel &voxel = get_voxel(p_x, p_y, p_z);

	Vector3 pos = (p_aabb.size * 0.5f) + p_aabb.position;

	const int max_rooms = Voxel::_max_rooms;

	float closest_dist[max_rooms];
	int closest_room[max_rooms];

	for (int n = 0; n < max_rooms; n++) {
		closest_dist[n] = FLT_MAX;
		closest_room[n] = Voxel::ROOM_NULL;
	}

	for (int n = 0; n < rooms.get_num_rooms(); n++) {
		const LRoom *lroom = rooms.get_room(n);
		float dist = lroom->is_point_within(pos);

		// ignore if doesn't make the top list
		if (dist >= closest_dist[max_rooms - 1])
			continue;

		for (int i = 0; i < max_rooms; i++) {
			if (dist < closest_dist[i]) {
				// move all the rest back by 1
				for (int j = max_rooms - 1; j > i; j--) {
					closest_room[j] = closest_room[j - 1];
					closest_dist[j] = closest_dist[j - 1];
				}

				// store the new value
				closest_dist[i] = dist;
				closest_room[i] = n;
				break;
			} // if it was one of the closest
		} // for i through the list in the voxel

	} // for n through rooms

	// store the result in the voxel
	for (int n = 0; n < max_rooms; n++) {
		voxel.room_id[n] = closest_room[n];
	}
}
