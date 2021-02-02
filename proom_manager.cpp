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

#include "proom_manager.h"
#include "pdebug.h"
#include "scene/3d/camera.h"

using namespace Lawn;

void PRoomManager::clear() {
	_converted = false;

	_rooms.clear();
	_statics.clear();
	_camera.clear();
	_dobs.clear();
	_rooms_lookup.clear();
}

void PRoomManager::frame_update(Node *p_any_node) {
	if (!_active || !_converted)
		return;

	// keep count of ticks so rooms that have no dobs can be recognised as out of date
	_tick++;

	if (!_camera.update(*this)) {
		resolve_camera_path(p_any_node);
		return;
	}

	if (!_dobs.update(*this)) {
		resolve_dobs_path(p_any_node);
	}

	// flip the trace results
	SWAP(_trace_result_curr, _trace_result_prev);

	// do the trace
	_tracer.trace(*this, *_trace_result_curr);

	// show those that were not previously shown
	for (int n = 0; n < _trace_result_curr->visible_static_ids.size(); n++) {
		uint32_t static_id = _trace_result_curr->visible_static_ids[n];

		// if not previously showing, show
		if (_trace_result_prev->bf_visible_statics.get_bit(static_id) == 0) {
			PStatic &stat = _statics.get_static(static_id);
			stat.show(true);
		}
	}

	// hide those that were previously shown but are now not visible
	for (int n = 0; n < _trace_result_prev->visible_static_ids.size(); n++) {
		uint32_t static_id = _trace_result_prev->visible_static_ids[n];

		// if previously showing, and now not, hide
		if (_trace_result_curr->bf_visible_statics.get_bit(static_id) == 0) {
			PStatic &stat = _statics.get_static(static_id);
			stat.show(false);
		}
	}

	_dobs.cull_untraced();
}

template <class NODE_TYPE>
NODE_TYPE *PRoomManager::_resolve_path(Node *p_anynode, NodePath p_path) const {
	//	LPRINT(2, "resolve_path " + p_path);

	if (p_anynode->has_node(p_path)) {
		//LPRINT(2, "has_node");
		NODE_TYPE *node = Object::cast_to<NODE_TYPE>(p_anynode->get_node(p_path));
		if (node) {
			return node;
		} else {
			WARN_PRINT("node is incorrect type");
		}
	}

	return nullptr;
}

bool PRoomManager::resolve_dobs_path(Node *p_anynode) {
	Spatial *node = _resolve_path<Spatial>(p_anynode, _settings_path_dobs);

	if (node)
		_dobs.set_godot_node_ID(node->get_instance_id());
	else
		_dobs.set_godot_node_ID(-1);

	return node != nullptr;
}

bool PRoomManager::resolve_camera_path(Node *p_anynode) // needs a node in the scene graph to use get_node
{
	Camera *node = _resolve_path<Camera>(p_anynode, _settings_path_camera);

	if (node)
		_camera.set_godot_node_ID(node->get_instance_id());
	else
		_camera.set_godot_node_ID(-1);

	return node != nullptr;
}

bool PRoomManager::resolve_roomlist_path(Node *p_anynode) {
	Spatial *node = _resolve_path<Spatial>(p_anynode, _settings_path_roomlist);
	_rooms.set_roomlist_node(node);

	return node != nullptr;
}

void PRoomManager::set_show_debug(bool p_show) {
	for (int n = 0; n < _rooms.get_num_rooms(); n++) {
		LRoom *lroom = _rooms.get_room(n);
		lroom->show_debug(p_show);
	}
	_show_debug = p_show;
}

void PRoomManager::set_active(bool p_active) {
	// noop?
	if (_active == p_active)
		return;

	// go through all statics, either turn them all on or all off
	bool show = p_active == false;

	for (int n = 0; n < _statics.get_num_statics(); n++) {
		PStatic &s = _statics.get_static(n);
		s.show(show);

		// don't rely on the visibility state from our show function
		// (user may have changed it manually)
		if (show)
			s.get_node()->show();
		else
			s.get_node()->hide();
	}

	_trace_results[0].clear();
	_trace_results[1].clear();

	_active = p_active;
}

PRoomManager::PRoomManager() {
	_active = Engine::get_singleton()->is_editor_hint() == false;
}
