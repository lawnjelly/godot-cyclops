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
#include "scene/3d/spatial.h"

class LRoom;

namespace Lawn {

class PRooms {
	friend class PConverter;

public:
	PRooms();
	void set_roomlist_node(Spatial *p_roomlist);
	void clear();

	Spatial *get_roomlist() const { return _roomlist_node; }

	LRoom *get_room(int p_id) {
		return _room_nodes[p_id];
	}
	const LRoom *get_room(int p_id) const {
		return _room_nodes[p_id];
	}
	int get_num_rooms() const { return _room_nodes.size(); }

private:
	Spatial *_roomlist_node;
	ObjectID _roomlist_godot_ID;

	LVector<LRoom *> _room_nodes;
};

} // namespace Lawn
