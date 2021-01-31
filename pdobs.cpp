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

#include "pdobs.h"
#include "lroom.h"
#include "proom_manager.h"
#include "scene/3d/mesh_instance.h"

using namespace Lawn;

PDob *PDobList::request(Spatial *p_node, const AABB &p_aabb, int p_room_id) {
	PDob *d = _dobs.request();
	d->create(p_node, p_aabb, p_room_id);
	return d;
}

////////////////////////////////////

bool PDobs::update(PRoomManager &p_room_manager) {
	if (_godot_dobs_ID == (ObjectID)-1)
		return false;

	Node *parent = Object::cast_to<Node>(ObjectDB::get_instance(_godot_dobs_ID));
	if (!parent)
		return false;

	_dobs_this_frame.clear();

	for (int n = 0; n < parent->get_child_count(); n++) {
		Spatial *child = Object::cast_to<Spatial>(parent->get_child(n));
		if (child) {
			found_node(p_room_manager, child);
		}
	}

	return true;
}

void PDobs::found_node(PRoomManager &p_room_manager, Spatial *p_node) {
	MeshInstance *mi = Object::cast_to<MeshInstance>(p_node);

	if (mi) {
		found_mesh_instance(p_room_manager, mi);
	}

	// recurse
	for (int n = 0; n < p_node->get_child_count(); n++) {
		Spatial *child = Object::cast_to<Spatial>(p_node->get_child(n));
		if (child) {
			found_node(p_room_manager, child);
		}
	}
}

void PDobs::found_mesh_instance(PRoomManager &p_room_manager, MeshInstance *p_mi) {
	// which room is it in?
	int room_id = p_room_manager._rooms.find_room_within(p_mi->get_global_transform().origin, -1);

	// outside world. Cull? NYI
	if (room_id == -1)
		return;

	// aabb
	const AABB &aabb = p_mi->get_transformed_aabb();

	LRoom *lroom = p_room_manager._rooms.get_room(room_id);

	lroom->_doblist.mark_up_to_date(p_room_manager.get_tick());

	// dob doesn't actually need room_id, but hey ho
	PDob *dob = lroom->_doblist.request(p_mi, aabb, room_id);

	DobLink dl;
	dl.room = lroom;
	dl.dob_number = lroom->_doblist.get_num_dobs() - 1;
	_dobs_this_frame.push_back(dl);

	// reset this
	dob->_traced_this_frame = false;
}

void PDobs::cull_untraced() {
	for (int n = 0; n < _dobs_this_frame.size(); n++) {
		const DobLink &dl = _dobs_this_frame[n];

		PDob &dob = dl.room->_doblist.get_dob(dl.dob_number);
		if (!dob._traced_this_frame) {
			dob.get_node()->hide();
		}
	}
}
