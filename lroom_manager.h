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

#include "proom_manager.h"
#include "scene/3d/spatial.h"

class LRoomManager : public Spatial {
	GDCLASS(LRoomManager, Spatial);

protected:
	Lawn::PRoomManager _rm;

public:
	void set_rooms_path(const NodePath &p_path) {
		_rm._settings_path_roomlist = p_path;
	}
	NodePath get_rooms_path() const {
		return _rm._settings_path_roomlist;
	}
	void set_camera_path(const NodePath &p_path);

	NodePath get_camera_path() const {
		return _rm._settings_path_camera;
	}
	void set_dobs_path(const NodePath &p_path) {
		_rm._settings_path_dobs = p_path;
	}
	NodePath get_dobs_path() const {
		return _rm._settings_path_dobs;
	}

	void rooms_set_active(bool p_active);
	bool rooms_get_active() const;

	void set_show_debug(bool p_show);
	bool get_show_debug() const;

	void set_portal_plane_convention(bool p_reversed);
	bool get_portal_plane_convention() const;

	void rooms_convert();
	void rooms_clear();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	LRoomManager();
};
