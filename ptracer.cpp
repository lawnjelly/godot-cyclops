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

#include "ptracer.h"
#include "lportal.h"
#include "lroom.h"
#include "pcamera.h"
#include "pdebug.h"

using namespace Lawn;

void PTracer::trace(PRoomManager &p_room_manager, PTraceResult &r_result) {
	_room_manager = &p_room_manager;
	_cam = &_room_manager->_camera;
	_result = &r_result;

	// all the statics should be not hit to start with
	_result->clear();

	// if the camera is not in a room do nothing
	if (_cam->get_room_id() == -1)
		return;

	// start off the trace with the planes from the camera
	LVector<Plane> cam_planes;
	cam_planes.copy_from(_cam->get_planes());

	trace_recursive(0, _cam->get_room_id(), cam_planes);
}

void PTracer::cull_dobs(LRoom *p_room, const LVector<Plane> &p_planes) {
	// if room is out of date, there are no valid dobs in it
	if (!p_room->_doblist.is_up_to_date(_room_manager->get_tick())) {
		return;
	}

	int num_dobs = p_room->_doblist.get_num_dobs();

	for (int n = 0; n < num_dobs; n++) {
		PDob &dob = p_room->_doblist.get_dob(n);

		bool bShow = true;

		// estimate the radius .. for now
		const AABB &bb = dob.get_aabb();

		//		print("\t\t\tculling object " + pObj->get_name());

		for (int p = 0; p < p_planes.size(); p++) {
			//				float dist = planes[p].distance_to(pt);
			//				print("\t\t\t\t" + itos(p) + " : dist " + String(Variant(dist)));

			float r_min, r_max;
			bb.project_range_in_plane(p_planes[p], r_min, r_max);

			//		print("\t\t\t\t" + itos(p) + " : r_min " + String(Variant(r_min)) + ", r_max " + String(Variant(r_max)));

			if (r_min > 0.0f) {
				//LPRINT_RUN(2, "\tout of view");
				bShow = false;
				break;
			}
		}

		if (bShow) {
			dob.get_node()->set_visible(bShow);
			// mark as traced .. any dobs that are not traced
			// will be culled afterwards
			dob._traced_this_frame = true;
		}

	} // for n through dobs
}

void PTracer::cull_statics(LRoom *p_room, const LVector<Plane> &p_planes) {
	int last_static = p_room->_first_static + p_room->_num_statics;

	for (int n = p_room->_first_static; n < last_static; n++) {
		PStatic &stat = _room_manager->_statics.get_static(n);

		bool bShow = true;

		// estimate the radius .. for now
		const AABB &bb = stat.get_aabb();

		//		print("\t\t\tculling object " + pObj->get_name());

		for (int p = 0; p < p_planes.size(); p++) {
			//				float dist = planes[p].distance_to(pt);
			//				print("\t\t\t\t" + itos(p) + " : dist " + String(Variant(dist)));

			float r_min, r_max;
			bb.project_range_in_plane(p_planes[p], r_min, r_max);

			//		print("\t\t\t\t" + itos(p) + " : r_min " + String(Variant(r_min)) + ", r_max " + String(Variant(r_max)));

			if (r_min > 0.0f) {
				//LPRINT_RUN(2, "\tout of view");
				bShow = false;
				break;
			}
		}

		// bypass the bitfield for now and just show / hide
		//stat.show(bShow);

		if (bShow) {
			// set the visible bit if not set
			if (_result->bf_visible_statics.check_and_set(n)) {
				// if wasn't previously set, add to the visible list
				_result->visible_static_ids.push_back(n);
			}
		}

	} // for n through statics
}

void PTracer::trace_recursive(int p_depth, int p_room_id, const LVector<Plane> &p_planes) {
	// prevent too much depth
	if (p_depth > 8) {
		LPRINT_RUN(2, "\t\t\tDEPTH LIMIT REACHED");
		WARN_PRINT_ONCE("LPortal Depth Limit reached (seeing through > 8 portals)");
		return;
	}

	// get the room
	LRoom *room = _room_manager->_rooms.get_room(p_room_id);

	cull_statics(room, p_planes);
	cull_dobs(room, p_planes);

	int num_portals = room->get_num_portals();

	for (int p = 0; p < num_portals; p++) {
		LPortal *portal = room->get_portal(p);

		// everything depends on whether the portal is incoming or outgoing.
		int outgoing = 1;

		int room_a_id = portal->get_linked_room_id(0);
		if (room_a_id != p_room_id) {
			outgoing = 0;
			assert(portal->get_linked_room_id(1) == p_room_id);
		}

		// cull by portal angle to camera.

		// NEW! I've come up with a much better way of culling portals by direction to camera...
		// instead of using dot product with a varying view direction, we simply find which side of the portal
		// plane the camera is on! If it is behind, the portal can be seen through, if in front, it can't! :)
		float dist_cam = portal->get_plane().distance_to(_cam->get_pos());

		if (!outgoing)
			dist_cam = -dist_cam;

		//		LPRINT_RUN(2, "\tPORTAL " + itos (p) + " (" + itos(port_id) + ") " + port.get_name());
		if (dist_cam >= 0.0f) // was >
		{
			LPRINT_RUN(2, "\t\tCULLED (back facing)");
			continue;
		}

		// is it culled by the planes?
		LPortal::eClipResult overall_res = LPortal::eClipResult::CLIP_INSIDE;

		// while clipping to the planes we maintain a list of partial planes, so we can add them to the
		// recursive next iteration of planes to check
		static LVector<int> partial_planes;
		partial_planes.clear();

		// for portals, we want to ignore the near clipping plane, as we might be right on the edge of a doorway
		// and still want to look through the portal.
		// So we are starting this loop from 1, ASSUMING that plane zero is the near clipping plane.
		// If it isn't we would need a different strategy
		// Note that now this only occurs for the first portal out of the current room. After that,
		// 0 is passed as first_portal_plane, because the near plane will probably be irrelevant,
		// and we are now not necessarily copying the camera planes.
		int first_portal_plane = 0;

		for (int l = first_portal_plane; l < p_planes.size(); l++) {
			LPortal::eClipResult res = portal->ClipWithPlane(p_planes[l]);

			switch (res) {
				case LPortal::eClipResult::CLIP_OUTSIDE:
					overall_res = res;
					break;
				case LPortal::eClipResult::CLIP_PARTIAL:
					overall_res = res;
					partial_planes.push_back(l);
					break;
				default: // suppress warning
					break;
			}

			if (overall_res == LPortal::eClipResult::CLIP_OUTSIDE)
				break;
		}

		// this portal is culled
		if (overall_res == LPortal::eClipResult::CLIP_OUTSIDE) {
			LPRINT_RUN(2, "\t\tCULLED (outside planes)");
			continue;
		}

		// trace through this portal to the next room
		int linked_room_id = portal->get_linked_room_id(outgoing);

		if (linked_room_id != -1) {

			// we need some new planes
			unsigned int pool_mem = _planes_pool.request();
			if (pool_mem != (unsigned int)-1) {
				// get a vector of planes from the pool
				LVector<Plane> &new_planes = _planes_pool.get(pool_mem);
				new_planes.clear();

				// if portal is totally inside the planes, don't copy the old planes
				if (overall_res != LPortal::eClipResult::CLIP_INSIDE) {
					// only copy the partial planes that the portal cuts through
					for (int n = 0; n < partial_planes.size(); n++)
						new_planes.push_back(p_planes[partial_planes[n]]);
				}

				portal->add_planes(_cam->get_pos(), new_planes, outgoing != 0);

				trace_recursive(p_depth + 1, linked_room_id, new_planes);

				// no longer need these planes, free again
				_planes_pool.free(pool_mem);
			} // pool mem allocated
			else {
				// planes pool is empty!
				// This will happen if the view goes through shedloads of portals
				// The solution is either to increase the plane pool size, or not build levels
				// with views through multiple portals. Looking through multiple portals is likely to be
				// slow anyway because of the number of planes to test.
				WARN_PRINT_ONCE("planes pool is empty");
			}

		} // if a linked room exists
	} // for p through portals
}
