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

#include "scene/3d/spatial.h"

namespace Lawn {

class PHidable {
public:
	void show(bool p_show);
	void create(Spatial *p_node, const AABB &p_aabb, int p_room_id);

	Spatial *get_node() { return _node; }
	const AABB &get_aabb() const { return _aabb; }

private:
	// separate flag so we don't have to touch the godot lookup
	bool _show;

	AABB _aabb; // world space

	// new .. can be separated from the scene tree to cull
	Spatial *_node;
	Node *_parent_node;

	int32_t _room_ID;

	// which method we are using to show and hide .. detaching from scene tree or just show / hide through godot
	static bool _detach;
};

// static object
class PStatic : public PHidable {
	AABB _aabb; // world space
};

// dynamic object
class PDob : public PHidable {
public:
	bool _traced_this_frame;
};

} // namespace Lawn
