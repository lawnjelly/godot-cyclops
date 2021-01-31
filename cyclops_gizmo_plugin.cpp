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

#include "cyclops_gizmo_plugin.h"
#include "lroom.h"

CyclopsGizmoPlugin::CyclopsGizmoPlugin() {
}

bool CyclopsGizmoPlugin::has_gizmo(Spatial *p_spatial) {
	LRoom *lroom = Object::cast_to<LRoom>(p_spatial);

	if (!lroom)
		return false;

	print_line("gizmo found lroom!");
	return true;
}

String CyclopsGizmoPlugin::get_name() const {
	return "LRoom";
}

int CyclopsGizmoPlugin::get_priority() const {
	return -1;
}

void CyclopsGizmoPlugin::redraw(EditorSpatialGizmo *p_gizmo) {
	if (!_created) {
		//		create_material("hull", Color(0, 0, 1), false, false, true);
		create_material("hull", Color(0.5, 1.0, 0.0), false, true, false);
		_created = true;
	}

	p_gizmo->clear();

	LRoom *lroom = Object::cast_to<LRoom>(p_gizmo->get_spatial_node());
	/*
	if (lroom)
	{
		const Geometry::MeshData &md = lroom->_bound_mesh_data;
		
		if (!md.faces.size())
			return;

		Vector<Vector3> verts;
		Transform tr = lroom->get_global_transform();
		tr.affine_invert();
		
		for (int f=0; f<md.faces.size(); f++)
		{
			const Geometry::MeshData::Face &face = md.faces[f];
			
			for (int c=0; c<face.indices.size()-2; c++)
			{
				int i0 = face.indices[0];
				int i1 = face.indices[c];
				int i2 = face.indices[c+1];
				
				verts.push_back(tr.xform(md.vertices[i0]));
				verts.push_back(tr.xform(md.vertices[i1]));
				verts.push_back(tr.xform(md.vertices[i2]));
			}
		}

		
		Ref<TriangleMesh> tmesh = memnew(TriangleMesh);

		PoolVector<Vector3> pverts;
		pverts.resize(verts.size());
		PoolVector<Vector3>::Write w = pverts.write();
		for (int n=0; n<verts.size(); n++)
		{
			w[n] = verts[n];
		}
		
		tmesh->create(pverts);
		
		p_gizmo->add_collision_triangles(tmesh);
		

//		Vector<Vector3> lines;
//		Transform tr = lroom->get_global_transform();
//		tr.affine_invert();
		
//		Ref<Material> material = get_material("hull", p_gizmo);
//		Color color(1, 1, 1, 1);

//		for (int n=0; n<md.edges.size(); n++)
//		{
//			Vector3 a = md.vertices[md.edges[n].a];
//			Vector3 b = md.vertices[md.edges[n].b];
			
//			// xform
//			a = tr.xform(a);
//			b = tr.xform(b);
					
//			lines.push_back(a);
//			lines.push_back(b);
//		}

//		p_gizmo->add_lines(lines, material, false, color);
		
	} // was lroom
*/

	// as a mesh
	if (lroom) {
		const Geometry::MeshData &md = lroom->_bound_mesh_data;
		if (!md.edges.size())
			return;

		Vector<Vector3> lines;
		Transform tr = lroom->get_global_transform();
		tr.affine_invert();

		Ref<Material> material = get_material("hull", p_gizmo);
		Color color(1, 1, 1, 1);

		for (int n = 0; n < md.edges.size(); n++) {
			Vector3 a = md.vertices[md.edges[n].a];
			Vector3 b = md.vertices[md.edges[n].b];

			// xform
			a = tr.xform(a);
			b = tr.xform(b);

			lines.push_back(a);
			lines.push_back(b);
		}

		p_gizmo->add_lines(lines, material, false, color);

	} // was lroom
}
