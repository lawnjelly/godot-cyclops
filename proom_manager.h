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

#include "pcamera.h"
#include "pdobs.h"
#include "prooms.h"
#include "prooms_lookup.h"
#include "pstatics.h"
#include "ptracer.h"
#include "scene/3d/spatial.h"

class LRoomManager;

namespace Lawn {

class PRoomManager {
	friend class PConverter;
	//friend class ::LRoomManager;
public:
	void frame_update(Node *p_any_node);
	void clear();

	// internal
	PRooms _rooms;
	PStatics _statics;
	PCamera _camera;
	PTracer _tracer;
	PDobs _dobs;
	PRoomsLookup _rooms_lookup;

	// funcs
	bool resolve_roomlist_path(Node *p_anynode);
	bool resolve_camera_path(Node *p_anynode); // needs a node in the scene graph to use get_node
	bool resolve_dobs_path(Node *p_anynode);

	// accessible from UI
	NodePath _settings_path_roomlist;
	NodePath _settings_path_camera;
	NodePath _settings_path_dobs;

	void set_active(bool p_active);
	bool get_active() const { return _active; }

	void set_show_debug(bool p_show);
	bool get_show_debug() const { return _show_debug; }

	uint32_t get_tick() const { return _tick; }

	// wrapper func
	int find_room_within(const Vector3 &p_pos, int p_previous_room_id) const {
		return _rooms_lookup.find_room_within(_rooms, p_pos, p_previous_room_id);
	}

	PRoomManager();

private:
	template <class NODE_TYPE>
	NODE_TYPE *_resolve_path(Node *p_anynode, NodePath p_path) const;

	PTraceResult _trace_results[2];

	bool _active = false;

	// when not converted, the updates will be switched off
	bool _converted = false;

	// portals, room hulls etc
	bool _show_debug = true;

	// flip between the 2 each trace, so we can determine
	// how visibility changed and which need hiding and showing
	PTraceResult *_trace_result_curr = &_trace_results[0];
	PTraceResult *_trace_result_prev = &_trace_results[1];

	uint32_t _tick = -1;
};

} // namespace Lawn
