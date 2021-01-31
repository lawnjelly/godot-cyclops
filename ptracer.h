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

#include "lbitfield_dynamic.h"
#include "lplanes_pool.h"
#include "lvector.h"

class LRoom;

namespace Lawn {

class PRoomManager;
class PCamera;

// a bitfield for which statics have been hit this time,
// and a list of showing statics
class PTraceResult {
public:
	void create(int p_num_statics) {
		bf_visible_statics.create(p_num_statics);
	}
	void clear() {
		bf_visible_statics.blank();
		visible_static_ids.clear();
	}

	LBitField_Dynamic bf_visible_statics;
	LVector<uint32_t> visible_static_ids;
};

class PTracer {
public:
	void trace(PRoomManager &p_room_manager, PTraceResult &r_result);

private:
	void trace_recursive(int p_depth, int p_room_id, const LVector<Plane> &p_planes);

	void cull_statics(LRoom *p_room, const LVector<Plane> &p_planes);
	void cull_dobs(LRoom *p_room, const LVector<Plane> &p_planes);

	PRoomManager *_room_manager = nullptr;
	PCamera *_cam = nullptr;
	PTraceResult *_result = nullptr;

	LPlanesPool _planes_pool;
};

} // namespace Lawn
