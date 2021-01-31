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

#include "lportal.h"
#include "lroom.h"

using namespace Lawn;

bool LPortal::_portal_plane_convention = false;

void LPortal::_bind_methods() {
	// main functions
	//ClassDB::bind_method(D_METHOD("rooms_convert", "verbose", "delete lights"), &LRoomManager::rooms_convert);
	ClassDB::bind_method(D_METHOD("set_portal_active", "p_active"), &LPortal::set_portal_active);
	ClassDB::bind_method(D_METHOD("get_portal_active"), &LPortal::get_portal_active);

	ClassDB::bind_method(D_METHOD("set_linked_room", "p_room"), &LPortal::set_linked_room);
	ClassDB::bind_method(D_METHOD("get_linked_room"), &LPortal::get_linked_room);

	ADD_GROUP("Portal", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "portal_active"), "set_portal_active", "get_portal_active");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "linked_room"), "set_linked_room", "get_linked_room");
}

void LPortal::set_portal_active(bool p_active) {
	_settings_active = p_active;
}

bool LPortal::get_portal_active() const {
	return _settings_active;
}

void LPortal::create(int p_room_id_from) {
	clear();
	_linkedroom_ID[0] = p_room_id_from;
}

void LPortal::show_debug(bool p_show) {
	set_visible(p_show);
}

void LPortal::set_linked_room(const NodePath &link_path) {
	_settings_path_linkedroom = link_path;

	LRoom *linkedroom = nullptr;
	if (has_node(link_path)) {
		linkedroom = Object::cast_to<LRoom>(get_node(link_path));
	}

	if (linkedroom) {
		_linkedroom_godot_ID = linkedroom->get_instance_id();
		_linkedroom_ID[1] = linkedroom->_room_ID;

		// add this portal as a backlink to the room linked
		linkedroom->add_portal(this);
	} else {
		_linkedroom_godot_ID = 0;
	}
}

NodePath LPortal::get_linked_room() const {
	return _settings_path_linkedroom;
}

LPortal::LPortal() {
	_settings_active = true;
	_linkedroom_ID[0] = -1;
	_linkedroom_ID[1] = -1;
}

// create the cached data from mesh instance, sort winding etc
void LPortal::update_from_mesh() {
	_pts_world.clear();
	_pt_centre = Vector3();
	_plane = Plane();

	// some godot jiggery pokery to get the mesh verts in local space
	Ref<Mesh> rmesh = get_mesh();

	if (rmesh->get_surface_count() == 0) {
		String sz;
		sz = "Portal '" + get_name() + "' has no surfaces, ignoring";
		WARN_PRINT(sz);
		return;
	}

	Array arrays = rmesh->surface_get_arrays(0);
	PoolVector<Vector3> vertices = arrays[VS::ARRAY_VERTEX];

	Transform trans = get_global_transform();

	int nPoints = vertices.size();
	ERR_FAIL_COND(nPoints < 3);

	_pts_world.clear();

	for (int n = 0; n < nPoints; n++) {
		Vector3 ptWorld = trans.xform(vertices[n]);

		// new!! test for duplicates. Some geometry may contain duplicate verts in portals which will muck up
		// the winding etc...
		bool bDuplicate = false;
		for (int m = 0; m < _pts_world.size(); m++) {
			Vector3 ptDiff = ptWorld - _pts_world[m];
			if (ptDiff.length() < 0.001f) {
				bDuplicate = true;
				break;
			}
		}

		if (!bDuplicate) {
			_pts_world.push_back(ptWorld);
		}
	}

	sort_verts_clockwise(_portal_plane_convention);
	plane_from_points();
}

void LPortal::sort_verts_clockwise(bool portal_plane_convention) {
	Vector<Vector3> &verts = _pts_world;

	// We first assumed first 3 determine the desired normal

	// find normal
	Plane plane;
	if (portal_plane_convention)
		plane = Plane(verts[0], verts[2], verts[1]);
	else
		plane = Plane(verts[0], verts[1], verts[2]);

	Vector3 ptNormal = plane.normal;

	// find centroid
	int nPoints = verts.size();

	_pt_centre = Vector3(0, 0, 0);

	for (int n = 0; n < nPoints; n++) {
		_pt_centre += verts[n];
	}
	_pt_centre /= nPoints;

	// now algorithm
	for (int n = 0; n < nPoints - 2; n++) {
		Vector3 a = verts[n] - _pt_centre;
		a.normalize();

		Plane p = Plane(verts[n], _pt_centre, _pt_centre + ptNormal);

		double SmallestAngle = -1;
		int Smallest = -1;

		for (int m = n + 1; m < nPoints; m++) {
			if (p.distance_to(verts[m]) > 0.0f) {
				Vector3 b = verts[m] - _pt_centre;
				b.normalize();

				double Angle = a.dot(b);

				if (Angle > SmallestAngle) {
					SmallestAngle = Angle;
					Smallest = m;
				}
			} // which side

		} // for m

		// swap smallest and n+1 vert
		if (Smallest != -1) {
			Vector3 temp = verts[Smallest];
			verts.set(Smallest, verts[n + 1]);
			verts.set(n + 1, temp);
		}
	} // for n

	// the vertices are now sorted, but may be in the opposite order to that wanted.
	// we detect this by calculating the normal of the poly, then flipping the order if the normal is pointing
	// the wrong way.
	plane = Plane(verts[0], verts[1], verts[2]);

	if (ptNormal.dot(plane.normal) < 0.0f) {
		// reverse order of verts
		reverse_winding_order();
	}
}

void LPortal::reverse_winding_order() {
	Vector<Vector3> &verts = _pts_world;
	Vector<Vector3> copy = verts;

	for (int n = 0; n < verts.size(); n++) {
		verts.set(n, copy[verts.size() - n - 1]);
	}
}

void LPortal::plane_from_points() {
	if (_pts_world.size() < 3) {
		WARN_PRINT("Portal must have at least 3 vertices");
		return;
	}
	// create plane from points
	_plane = Plane(_pts_world[0], _pts_world[1], _pts_world[2]);

	//print("\t\t\t\t\tPlane normal world space : " + m_Plane);
}

void LPortal::debug_check_plane_validity(const Plane &p) const {
	assert(p.distance_to(_pt_centre) < 0.0f);
}

LPortal::eClipResult LPortal::ClipWithPlane(const Plane &p) const {
	int nOutside = 0;
	int nPoints = _pts_world.size();

	for (int n = 0; n < nPoints; n++) {
		float d = p.distance_to(_pts_world[n]);

		if (d >= 0.0f)
			nOutside++;
	}

	if (nOutside == nPoints) {
		LPRINT_RUN(2, "\t\tOutside plane " + p);
		return CLIP_OUTSIDE;
	}

	if (nOutside == 0)
		return CLIP_INSIDE;

	return CLIP_PARTIAL;
}

void LPortal::clear() {
	_settings_active = true;
	_linkedroom_godot_ID = 0;
	_linkedroom_ID[0] = -1;
	_linkedroom_ID[1] = -1;
	_pts_world.clear();
	_pt_centre = Vector3();
	_plane = Plane();
}

// add clipping planes to the vector formed by each portal edge and the camera
void LPortal::add_planes(const Vector3 &p_cam, LVector<Plane> &r_planes, bool p_outgoing) const {
	// short version
	const Vector<Vector3> &pts = _pts_world;

	int nPoints = pts.size();
	ERR_FAIL_COND(nPoints < 3);

	Plane p;

	int offset_a, offset_b;
	if (p_outgoing) {
		offset_a = 0;
		offset_b = -1;
	} else {
		offset_a = -1;
		offset_b = 0;
	}

	for (int n = 1; n < nPoints; n++) {
		p = Plane(p_cam, pts[n + offset_a], pts[n + offset_b]);

		// detect null plane
		//		if (p.normal.length_squared() < 0.1f)
		//		{
		//			print("NULL plane detected from points : ");
		//			print(ptCam + pts[n] + pts[n-1]);
		//		}
		r_planes.push_back(p);
		debug_check_plane_validity(p);
	}

	// first and last
	if (p_outgoing)
		p = Plane(p_cam, pts[0], pts[nPoints - 1]);
	else
		p = Plane(p_cam, pts[nPoints - 1], pts[0]);

	r_planes.push_back(p);
	debug_check_plane_validity(p);

	// debug
	//	if (!manager.m_bDebugPlanes)
	//		return;

	//	for (int n=0; n<nPoints; n++)
	//	{
	//		manager.m_DebugPlanes.push_back(pts[n]);
	//	}
}
