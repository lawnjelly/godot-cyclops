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

#include "prooms.h"
#include "pdebug.h"

using namespace Lawn;

PRooms::PRooms() {
	_roomlist_node = nullptr;
}

void PRooms::clear() {
	_room_nodes.clear(true);
}

void PRooms::set_roomlist_node(Spatial *p_roomlist) {
	if (p_roomlist) {
		LPRINT(2, "\tset_roomlist_node");
	} else {
		LPRINT(2, "\tset_roomlist_node NULL");
	}

	_roomlist_node = nullptr;
	_roomlist_godot_ID = 0;

	if (p_roomlist) {
		_roomlist_node = p_roomlist;
		_roomlist_godot_ID = _roomlist_node->get_instance_id();
		LPRINT(2, "\t\troomlist was godot ID " + itos(_roomlist_godot_ID));
	}
}
