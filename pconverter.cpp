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

#include "pconverter.h"
#include "core/math/quick_hull.h"
#include "lportal.h"
#include "lroom.h"
#include "pdebug.h"
#include "proom_manager.h"
#include "prooms.h"
#include "scene/3d/mesh_instance.h"

using namespace Lawn;

int PConverter::_conversion_tick = 0;

bool PConverter::name_starts_with(const Node *pNode, String szSearch) const {
	int sl = szSearch.length();

	String name = pNode->get_name();
	int l = name.length();

	if (l < sl)
		return false;

	String szStart = name.substr(0, sl);

	//print_line("\t\tNameStartsWith szStart is " + szStart);

	if (szStart == szSearch)
		return true;

	return false;
}

String PConverter::find_name_after(Node *pNode, String szStart) const {
	String szRes;
	String name = pNode->get_name();
	szRes = name.substr(szStart.length());

	// because godot doesn't support multiple nodes with the same name, we will strip e.g. a number
	// after an @ on the end of the name...
	// e.g. portal_kitchen@2
	for (int c = 0; c < szRes.length(); c++) {
		if (szRes[c] == '*') {
			// remove everything after and including this character
			szRes = szRes.substr(0, c);
			break;
		}
	}

	//print("\t\tNameAfter is " + szRes);
	return szRes;
}

void PConverter::convert(PRoomManager &p_room_manager) {
	LPRINT(2, "Convert");

	// the conversion tick is a count of how many times convert has been called.
	// it is used to ensure that rooms etc are only converted once per tick ..
	// otherwise an LRoom can get hit twice.
	_conversion_tick++;

	p_room_manager.clear();

	PRooms &rooms = p_room_manager._rooms;

	_portals.clear(true);

	if (!rooms.get_roomlist()) {
		ERR_FAIL_MSG("PConverter::convert roomlist is NULL");
	}

	convert_rooms_recursive(p_room_manager, rooms.get_roomlist());

	second_pass_portals(rooms.get_roomlist());

	// prepare the trace results
	p_room_manager._trace_results[0].create(p_room_manager._statics.get_num_statics());
	p_room_manager._trace_results[1].create(p_room_manager._statics.get_num_statics());

	// show debug according to settings
	p_room_manager.set_show_debug(p_room_manager.get_show_debug());

	// mark conversion complete
	p_room_manager._converted = true;
}

void PConverter::second_pass_portals(Spatial *p_roomlist) {
	LPRINT(4, "SecondPass_Portals");

	// this only works in the editor
	//	Node *pRootNode = SceneTree::get_singleton()->get_edited_scene_root();

	for (int n = 0; n < _portals.size(); n++) {
		LPortal *pPortal = _portals[n];
		String szLinkRoom = find_name_after(pPortal, "lportal_");
		szLinkRoom = "lroom_" + szLinkRoom;

		String sz = "\tLinkRoom : " + szLinkRoom;

		LRoom *pLinkedRoom = Object::cast_to<LRoom>(p_roomlist->find_node(szLinkRoom, true, false));
		if (pLinkedRoom) {
			sz += "\tfound.";
			NodePath path = pPortal->get_path_to(pLinkedRoom);
			pPortal->set_linked_room(path);
		} else
			sz += "\tnot found.";

		LPRINT(4, sz);
	}
}

void PConverter::convert_portal(LRoom *p_room, MeshInstance *pMI) {
	String szFullName = pMI->get_name();
	LPRINT(4, "ConvertPortal : " + szFullName);

	LPortal *pPortal = Object::cast_to<LPortal>(pMI);

	// if not an lportal already, convert the node type
	if (!pPortal) {
		pPortal = change_node_type<LPortal>(pMI, "l", false);

		// copy mesh
		pPortal->set_mesh(pMI->get_mesh());
		if (pMI->get_surface_material_count())
			pPortal->set_surface_material(0, pMI->get_surface_material(0));

		pMI->queue_delete();
	}

	// create from the ID of the room the portal leads from
	pPortal->create(p_room->_room_ID);

	// link rooms
	pPortal->update_from_mesh();

	// keep a list of portals for second pass
	_portals.push_back(pPortal);

	// add the portal to the list of portals in the room
	p_room->add_portal(pPortal);
}

void PConverter::convert_room(PRoomManager &p_room_manager, Spatial *p_node) {
	// get the room part of the name
	String szFullName = p_node->get_name();

	//	String szRoom = find_name_after(p_node, "room_");

	// owner should normally be root
	Node *pOwner = p_node->get_owner();

	// is it already an lroom?
	LRoom *pLRoom = Object::cast_to<LRoom>(p_node);
	//bool already_lroom = pLRoom != nullptr;

	// if not already an lroom, convert the node and move all children
	if (!pLRoom) {
		// create an LRoom
		pLRoom = change_node_type<LRoom>(p_node, "l");

		// move each child
		while (p_node->get_child_count()) {
			Node *child = p_node->get_child(0);
			p_node->remove_child(child);

			// needs to set owner to appear in IDE
			pLRoom->add_child(child);
			child->set_owner(pOwner);
		}
	} else {
		// already hit this tick?
		if (pLRoom->_conversion_tick == _conversion_tick)
			return;
	}

	LPRINT(4, "ConvertRoom : " + szFullName);

	// make sure the room is blank, especially if already created
	pLRoom->clear();

	// mark so as only to convert once
	pLRoom->_conversion_tick = _conversion_tick;

	// add a new room to the room list
	pLRoom->_room_ID = p_room_manager._rooms._room_nodes.size();
	p_room_manager._rooms._room_nodes.push_back(pLRoom);

	for (int n = 0; n < pLRoom->get_child_count(); n++) {
		MeshInstance *child = Object::cast_to<MeshInstance>(pLRoom->get_child(n));

		if (child) {
			if (name_starts_with(child, "portal_") || name_starts_with(child, "lportal_")) {
				convert_portal(pLRoom, child);
			}
		}
	}

	// find portals and statics
	pLRoom->_first_static = p_room_manager._statics._statics.size();

	// points making up the room geometry, in world space, to create the convex hull
	Vector<Vector3> room_pts;

	for (int n = 0; n < pLRoom->get_child_count(); n++) {
		Spatial *child = Object::cast_to<Spatial>(pLRoom->get_child(n));

		if (child) {
			if (name_starts_with(child, "portal_") || name_starts_with(child, "lportal_")) {
				// ignore
			} else {
				find_statics_recursive(p_room_manager, pLRoom, child, room_pts);
			}
		}
	}

	// create convex hull
	convert_room_hull(pLRoom, room_pts);
}

void PConverter::add_plane_if_unique(Lawn::LVector<Plane> &p_planes, const Plane &p) {
	for (int n = 0; n < p_planes.size(); n++) {
		const Plane &o = p_planes[n];

		// this is a fudge factor for how close planes can be to be considered the same ...
		// to prevent ridiculous amounts of planes
		const float d = 0.08f;

		if (fabs(p.d - o.d) > d) continue;

		float dot = p.normal.dot(o.normal);
		if (dot < 0.98f) continue;

		// match!
		return;
	}

	// test
	//	Vector3 va(1, 0, 0);
	//	Vector3 vb(1, 0.2, 0);
	//	vb.normalize();
	//	float dot = va.dot(vb);
	//	print("va dot vb is " + String(Variant(dot)));

	// is unique
	//	print("\t\t\t\tAdding bound plane : " + p);

	p_planes.push_back(p);
}

bool PConverter::convert_room_hull(LRoom *p_room, const Vector<Vector3> &p_room_pts) {
	if (p_room_pts.size() > 3) {
		Geometry::MeshData md;
		Error err = QuickHull::build(p_room_pts, md);
		if (err == OK) {
			// get the planes
			for (int n = 0; n < md.faces.size(); n++) {
				const Plane &p = md.faces[n].plane;
				add_plane_if_unique(p_room->_planes, p);
			}

			LPRINT(2, "\t\t\tcontained " + itos(p_room->_planes.size()) + " planes.");

			// make a copy of the mesh data for debugging
			// note this could be avoided in release builds? NYI
			p_room->_bound_mesh_data = md;

			//			for (int f=0; f<md.faces.size(); f++)
			//			{
			//				String sz;
			//				sz = "face " + itos (f) + ", indices ";
			//				for (int i=0; i<md.faces[f].indices.size(); i++)
			//				{
			//					sz += itos(md.faces[f].indices[i]) + ", ";
			//				}
			//				LPRINT(2, sz);
			//			}

			return true;
		}
	}

	return false;
}

void PConverter::find_statics_recursive(PRoomManager &p_room_manager, LRoom *p_lroom, Spatial *p_node, Vector<Vector3> &room_pts) {
	MeshInstance *mi = Object::cast_to<MeshInstance>(p_node);

	if (mi) {
		LPRINT(2, "\tfound mesh " + mi->get_name());

		// static
		AABB aabb;
		bound_findpoints(mi, room_pts, aabb);

		// better to create AABB manually?
		//PStatic * stat =
		p_room_manager._statics.request(mi, aabb, p_lroom->_room_ID);
		p_lroom->_num_statics += 1;

		// new .. hide it if we are actively using lportal in the editor
		if (p_room_manager.get_active())
			mi->set_visible(false);
		else
			mi->set_visible(true);
	}

	for (int n = 0; n < p_node->get_child_count(); n++) {
		Spatial *child = Object::cast_to<Spatial>(p_node->get_child(n));

		find_statics_recursive(p_room_manager, p_lroom, child, room_pts);
	}
}

void PConverter::bound_findpoints(MeshInstance *p_mi, Vector<Vector3> &r_room_pts, AABB &r_aabb) {
	// max opposite extents .. note AABB storing size is rubbish in this aspect
	r_aabb.position = Vector3(FLT_MAX / 2, FLT_MAX / 2, FLT_MAX / 2);
	r_aabb.size = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// some godot jiggery pokery to get the mesh verts in local space
	Ref<Mesh> rmesh = p_mi->get_mesh();
	if (rmesh->get_surface_count() == 0) {
		String sz;
		sz = "MeshInstance '" + p_mi->get_name() + "' has no surfaces, ignoring";
		WARN_PRINT(sz);
		return;
	}

	Array arrays = rmesh->surface_get_arrays(0);

	// possible to have a meshinstance with no geometry .. don't want to crash
	if (!arrays.size()) {
		WARN_PRINT_ONCE("PConverter::bound_findpoints MeshInstance with no mesh, ignoring");
		return;
	}

	PoolVector<Vector3> vertices = arrays[VS::ARRAY_VERTEX];

	// convert to world space
	Transform trans = p_mi->get_global_transform();

	for (int n = 0; n < vertices.size(); n++) {
		Vector3 ptWorld = trans.xform(vertices[n]);
		r_room_pts.push_back(ptWorld);

		if (n) {
			r_aabb.expand_to(ptWorld);
		} else {
			r_aabb.position = ptWorld;
			r_aabb.size = Vector3(0, 0, 0);
		}
	}
}

void PConverter::convert_rooms_recursive(PRoomManager &p_room_manager, Spatial *p_node) {
	// is this a room?
	if (name_starts_with(p_node, "room_") || name_starts_with(p_node, "lroom_")) {
		convert_room(p_room_manager, p_node);
	}

	// recurse through children
	for (int n = 0; n < p_node->get_child_count(); n++) {
		Spatial *child = Object::cast_to<Spatial>(p_node->get_child(n));

		if (child) {
			convert_rooms_recursive(p_room_manager, child);
		}
	}
}
