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

#include "pitem.h"

using namespace Lawn;

bool PHidable::_detach = false;

void PHidable::show(bool p_show) {
	// noop
	if (_show == p_show)
		return;

	// new state
	_show = p_show;

	assert(_parent_node);

	if (_detach) {
		//String sz = "";
		if (_show) {
			//sz = "show ";
			// add to tree
			_parent_node->add_child(_node);
		} else {
			//sz = "hide ";
			// remove from tree
			_parent_node->remove_child(_node);
		}
		//sz += m_pParent->get_name();
		//sz += "->";
		//sz += m_pNode->get_name();
		//print_line(sz);
	} else {
		if (_show)
			_node->show();
		else
			_node->hide();
	}
}

void PHidable::create(Spatial *p_node, const AABB &p_aabb, int p_room_id) {
	CRASH_COND(!p_node);
	_node = p_node;


	_parent_node = _node->get_parent();
	_show = false;
	_room_ID = p_room_id;
	_aabb = p_aabb;
}

