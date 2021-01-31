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
#include "pitem.h"

class MeshInstance;
class LRoom;

namespace Lawn {

class PRoomManager;

// per room
class PDobList {
public:
	PDob &get_dob(unsigned int ui) { return _dobs[ui]; }
	const PDob &get_dob(unsigned int ui) const { return _dobs[ui]; }
	int get_num_dobs() const { return _dobs.size(); }

	PDob *request(Spatial *p_node, const AABB &p_aabb, int p_room_id);
	void clear() { _dobs.clear(); }

	void mark_up_to_date(uint32_t p_tick) {
		if (p_tick == _last_update_tick)
			return;

		_last_update_tick = p_tick;
		_dobs.clear();
	}

	bool is_up_to_date(uint32_t p_current_tick) const { return _last_update_tick == p_current_tick; }

private:
	Lawn::LVector<PDob> _dobs;
	uint32_t _last_update_tick = -1;
};

class PDobs {
public:
	bool update(PRoomManager &p_room_manager);
	void set_godot_node_ID(ObjectID p_id) { _godot_dobs_ID = p_id; }
	void cull_untraced();

	void clear() {
		_dobs_this_frame.clear();
	}

private:
	void found_node(PRoomManager &p_room_manager, Spatial *p_node);
	void found_mesh_instance(PRoomManager &p_room_manager, MeshInstance *p_mi);

	// node that the dobs hang off
	ObjectID _godot_dobs_ID = -1;

	struct DobLink {
		LRoom *room;
		uint32_t dob_number;
	};

	// secondary list of dobs this frame .. if any are not hit by the tracer, we will cull them
	Lawn::LVector<DobLink> _dobs_this_frame;
};

} // namespace Lawn
