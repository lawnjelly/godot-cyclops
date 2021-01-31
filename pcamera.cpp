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

#include "pcamera.h"
#include "scene/3d/camera.h"

using namespace Lawn;

bool PCamera::update(const PRoomManager &p_room_manager) {
	if (_godot_camera_ID == (ObjectID)-1)
		return false;

	Camera *cam = Object::cast_to<Camera>(ObjectDB::get_instance(_godot_camera_ID));
	if (!cam)
		return false;

	// get camera position and direction
	Transform tr = cam->get_global_transform();
	_pos = tr.origin;
	_dir = -tr.basis.get_axis(2);

	_planes.copy_from(cam->get_frustum());

	// alternative : instead of copying straight, we create the first (near) clipping
	// plane manually, at 0 distance from the camera. This ensures that portals will not be
	// missed, while still culling portals and objects behind us. If we use the actual near clipping plane
	// then a portal in front of the camera may not be seen through, giving glitches
	_planes[0] = Plane(_pos, _planes[0].normal);

	// what room is the camera within
	_room_ID = p_room_manager._rooms.find_room_within(_pos, _room_ID);

	return true;
}
