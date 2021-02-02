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

#include "register_types.h"

#include "core/class_db.h"
#include "cyclops_editor_plugin.h"
#include "cyclops_gizmo_plugin.h"
#include "lportal.h"
#include "lroom.h"
#include "lroom_manager.h"

void register_cyclops_types() {

	ClassDB::register_class<LRoomManager>();
	ClassDB::register_class<LRoom>();
	ClassDB::register_class<LPortal>();

#ifdef TOOLS_ENABLED
	EditorPlugins::add_by_type<CyclopsEditorPlugin>();
	ClassDB::register_class<CyclopsGizmoPlugin>();
#endif
}

void unregister_cyclops_types() {
	//nothing to do here
}
