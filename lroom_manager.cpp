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

#include "lroom_manager.h"
#include "lportal.h"
#include "pconverter.h"
#include "pdebug.h"

LRoomManager::LRoomManager() {
}

void LRoomManager::_bind_methods() {
	// main functions
	ClassDB::bind_method(D_METHOD("rooms_convert"), &LRoomManager::rooms_convert);
	ClassDB::bind_method(D_METHOD("rooms_clear"), &LRoomManager::rooms_clear);

#define LPORTAL_STRINGIFY(x) #x
#define LPORTAL_TOSTRING(x) LPORTAL_STRINGIFY(x)

#define LIMPL_PROPERTY(P_TYPE, P_NAME, P_SET, P_GET)                                                         \
	ClassDB::bind_method(D_METHOD(LPORTAL_TOSTRING(P_SET), LPORTAL_TOSTRING(P_NAME)), &LRoomManager::P_SET); \
	ClassDB::bind_method(D_METHOD(LPORTAL_TOSTRING(P_GET)), &LRoomManager::P_GET);                           \
	ADD_PROPERTY(PropertyInfo(P_TYPE, LPORTAL_TOSTRING(P_NAME)), LPORTAL_TOSTRING(P_SET), LPORTAL_TOSTRING(P_GET));

#define LIMPL_PROPERTY_RANGE(P_TYPE, P_NAME, P_SET, P_GET, P_RANGE_STRING)                                   \
	ClassDB::bind_method(D_METHOD(LPORTAL_TOSTRING(P_SET), LPORTAL_TOSTRING(P_NAME)), &LRoomManager::P_SET); \
	ClassDB::bind_method(D_METHOD(LPORTAL_TOSTRING(P_GET)), &LRoomManager::P_GET);                           \
	ADD_PROPERTY(PropertyInfo(P_TYPE, LPORTAL_TOSTRING(P_NAME), PROPERTY_HINT_RANGE, P_RANGE_STRING), LPORTAL_TOSTRING(P_SET), LPORTAL_TOSTRING(P_GET));

	ADD_GROUP("Main", "");
	LIMPL_PROPERTY(Variant::BOOL, active, rooms_set_active, rooms_get_active);
	LIMPL_PROPERTY(Variant::BOOL, reverse_portals, set_portal_plane_convention, get_portal_plane_convention);
	LIMPL_PROPERTY(Variant::BOOL, show_debug, set_show_debug, get_show_debug);

	ADD_GROUP("Paths", "");
	LIMPL_PROPERTY(Variant::NODE_PATH, rooms, set_rooms_path, get_rooms_path);
	LIMPL_PROPERTY(Variant::NODE_PATH, dobs, set_dobs_path, get_dobs_path);
	LIMPL_PROPERTY(Variant::NODE_PATH, camera, set_camera_path, get_camera_path);

#undef LIMPL_PROPERTY
#undef LIMPL_PROPERTY_RANGE
}

void LRoomManager::set_camera_path(const NodePath &p_path) {
	_rm._settings_path_camera = p_path;

	if (!_rm.resolve_camera_path(this)) {
		//LWARN(0, "Cannot resolve camera path");
		return;
	}
}

void LRoomManager::set_portal_plane_convention(bool p_reversed) {
	LPortal::_portal_plane_convention = p_reversed;
}

bool LRoomManager::get_portal_plane_convention() const {
	return LPortal::_portal_plane_convention;
}

void LRoomManager::set_show_debug(bool p_show) {
	_rm.set_show_debug(p_show);
}

bool LRoomManager::get_show_debug() const {
	return _rm.get_show_debug();
}

void LRoomManager::rooms_set_active(bool p_active) {
	_rm.set_active(p_active);
}

bool LRoomManager::rooms_get_active() const {
	return _rm.get_active();
}

void LRoomManager::rooms_clear() {
	_rm.clear();
}

void LRoomManager::rooms_convert() {
	if (!_rm.resolve_roomlist_path(this)) {
		LWARN(0, "Cannot resolve nodepath");
		return;
	}

	Lawn::PConverter converter;
	converter.convert(_rm);
}

void LRoomManager::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			// turn on process, unless we are in the editor
			if (true)
			//			if (!Engine::get_singleton()->is_editor_hint())
			{
				set_process(true);
				//CreateDebug();
			} else
				set_process(false);
		} break;
			// NOTE!! Must use PROCESS and NOT INTERNAL_PROCESS.
			// This is because all the internal processes are handled before all the processes.
			// If the camera position is set in process, then the culling will be updated BEFORE
			// the camera and we will get objects culled when they should not be.
			// For the same reason, the LRoomManager should be low down in the scene tree
			// so it is updated AFTER the camera.
		case NOTIFICATION_PROCESS: {
			_rm.frame_update(this);
		} break;
	}
}
