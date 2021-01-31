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
#include "scene/3d/mesh_instance.h"

class LPortal : public MeshInstance {
	GDCLASS(LPortal, MeshInstance);

public:
	enum eClipResult {
		CLIP_OUTSIDE,
		CLIP_PARTIAL,
		CLIP_INSIDE,
	};

	void create(int p_room_id_from);

	// ui interface
	void set_linked_room(const NodePath &link_path);
	NodePath get_linked_room() const;
	void set_portal_active(bool p_active);
	bool get_portal_active() const;

	// use
	void add_planes(const Vector3 &p_cam, Lawn::LVector<Plane> &r_planes, bool p_outgoing) const;
	LPortal::eClipResult ClipWithPlane(const Plane &p) const;
	void clear();
	void show_debug(bool p_show);

	// create the cached data from mesh instance, sort winding etc
	void update_from_mesh();

	const Vector3 &get_direction() const { return _plane.normal; }
	const Plane &get_plane() const { return _plane; }
	int get_linked_room_id(int p_which) const { return _linkedroom_ID[p_which]; }

	LPortal();

	// whether the normal of the portal points outward (false) or inward (true)
	static bool _portal_plane_convention;

private:
	void sort_verts_clockwise(bool portal_plane_convention);
	void reverse_winding_order();
	void plane_from_points();
	void debug_check_plane_validity(const Plane &p) const;

	NodePath _settings_path_linkedroom;
	bool _settings_active;

	ObjectID _linkedroom_godot_ID;

	// within lportal
	// room from and to
	int _linkedroom_ID[2];

	// normal determined by winding order
	Vector<Vector3> _pts_world;
	Vector3 _pt_centre; // world
	Plane _plane;

protected:
	static void _bind_methods();
};
