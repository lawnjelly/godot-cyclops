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

class LPortal;
class LRoom;

namespace Lawn {

class PRoomManager;

class PConverter {
public:
	void convert(PRoomManager &p_room_manager);

private:
	void convert_rooms_recursive(PRoomManager &p_room_manager, Spatial *p_node);
	void convert_room(PRoomManager &p_room_manager, Spatial *p_node);
	void convert_portal(LRoom *p_room, MeshInstance *pMI);
	bool convert_room_hull(LRoom *p_room, const Vector<Vector3> &p_room_pts);

	void find_statics_recursive(PRoomManager &p_room_manager, LRoom *p_lroom, Spatial *p_node, Vector<Vector3> &room_pts);
	void second_pass_portals(Spatial *p_roomlist);

	// useful funcs
	bool name_starts_with(const Node *pNode, String szSearch) const;
	String find_name_after(Node *pNode, String szStart) const;
	void bound_findpoints(MeshInstance *p_mi, Vector<Vector3> &r_room_pts, AABB &r_aabb);
	void add_plane_if_unique(Lawn::LVector<Plane> &p_planes, const Plane &p);

	template <class T>
	T *change_node_type(Spatial *pNode, String szPrefix, bool bDelete = true);

	LVector<LPortal *> _portals;

	static int _conversion_tick;
};

template <class T>
T *PConverter::change_node_type(Spatial *pNode, String szPrefix, bool bDelete) {
	String szFullName = pNode->get_name();

	// owner should normally be root
	Node *pOwner = pNode->get_owner();

	// create the new class T object
	T *pNew = memnew(T);
	pNew->set_name(szPrefix + szFullName);

	// needs to set owner to appear in IDE
	pNode->get_parent()->add_child(pNew);
	pNew->set_owner(pOwner);

	// new lroom should have same transform
	pNew->set_transform(pNode->get_transform());

	// delete old node
	if (bDelete) {
		//		memdelete(pNode);

		pNode->queue_delete();

		//		// delete immediately
		//		if (pNode->is_inside_tree()) {
		//			pNode->get_tree()->_flush_delete_queue();
		//		} else {
		//			SceneTree::get_singleton()->_flush_delete_queue();
		//		}
	}

	return pNew;
}

} // namespace Lawn
