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

#include "cyclops_editor_plugin.h"
#include "cyclops_gizmo_plugin.h"

void CyclopsEditorPlugin::_bake() {

	if (m_pRoomManager) {
		//LRoomManager->lightmap_bake();
		m_pRoomManager->rooms_convert();

		//#ifdef TOOLS_ENABLED
		//	// FIXME: Hack to refresh editor in order to display new properties and signals. See if there is a better alternative.
		//	if (Engine::get_singleton()->is_editor_hint()) {
		//		EditorNode::get_singleton()->get_inspector()->update_tree();
		//		//NodeDock::singleton->update_lists();
		//	}
		//#endif
	}
}

void CyclopsEditorPlugin::edit(Object *p_object) {

	LRoomManager *s = Object::cast_to<LRoomManager>(p_object);
	if (!s)
		return;

	m_pRoomManager = s;
}

bool CyclopsEditorPlugin::handles(Object *p_object) const {

	return p_object->is_class("LRoomManager");
}

void CyclopsEditorPlugin::make_visible(bool p_visible) {

	if (p_visible) {
		bake->show();
	} else {

		bake->hide();
	}
}

EditorProgress *CyclopsEditorPlugin::tmp_progress = NULL;

void CyclopsEditorPlugin::bake_func_begin(int p_steps) {

	ERR_FAIL_COND(tmp_progress != NULL);

	tmp_progress = memnew(EditorProgress("convert_rooms", TTR("Convert Rooms"), p_steps, true));
}

bool CyclopsEditorPlugin::bake_func_step(int p_step, const String &p_description) {

	ERR_FAIL_COND_V(tmp_progress == NULL, false);

	OS::get_singleton()->delay_usec(1000);

	return tmp_progress->step(p_description, p_step, false);
}

void CyclopsEditorPlugin::bake_func_end() {
	ERR_FAIL_COND(tmp_progress == NULL);
	memdelete(tmp_progress);
	tmp_progress = NULL;
}

void CyclopsEditorPlugin::_bind_methods() {

	ClassDB::bind_method("_bake", &CyclopsEditorPlugin::_bake);
}

CyclopsEditorPlugin::CyclopsEditorPlugin(EditorNode *p_node) {

	editor = p_node;
	bake = memnew(ToolButton);
	bake->set_icon(editor->get_gui_base()->get_icon("Bake", "EditorIcons"));
	bake->set_text(TTR("Convert Rooms"));
	bake->hide();
	bake->connect("pressed", this, "_bake");
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, bake);
	m_pRoomManager = NULL;

	//	LightMapper::bake_begin_function = bake_func_begin;
	//	LightMapper::bake_step_function = bake_func_step;
	//	LightMapper::bake_end_function = bake_func_end;

	Ref<CyclopsGizmoPlugin> gizmo_plugin = Ref<CyclopsGizmoPlugin>(memnew(CyclopsGizmoPlugin));
	SpatialEditor::get_singleton()->add_gizmo_plugin(gizmo_plugin);
}

CyclopsEditorPlugin::~CyclopsEditorPlugin() {
}
