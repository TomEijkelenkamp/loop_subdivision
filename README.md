# Loop subdivision

The following images show the stencils used to compute the new position in the geometry refinement stage. On the left the stencil is shown, the new vertex position depends on the weights of neighboring vertices in the old topology. The vertex in question to be positions is show on the right in the new topology. There are 4 different cases:<br /><br />

Vertex point that exists both in the new and old topology (β = v!=3 ? 3/8v : 3/16):<br />
<img src='./readme_images/vertex_stencil.png' width='320'><img src='./readme_images/compute_existing_vertex.png' width='300'><br />
<br />
Vertex point that exists both in the new and old topology, located on the boundary of the mesh:<br />
<img src='./readme_images/boundary_vertex_stencil.png' width='330'><img src='./readme_images/compute_existing_boundary_vertex.png' width='300'><br />
<br />
Vertex point that was just created in the topology refinement:<br />
<img src='./readme_images/edge_stencil.png' width='330'><img src='./readme_images/compute_new_vertex.png' width='300'><br />
<br />
Vertex point that was just created in the topology refinement, located on the boundary of the mesh:<br />
<img src='./readme_images/boundary_edge_stencil.png' width='310'><img src='./readme_images/compute_new_boundary_vertex.png' width='300'><br />
<br />

Normal-vector interpolation shading (Phong). Interpolation of normals shown on the left, vectors used on the right:
<img src='./readme_images/phong_normal_interpolation.png' width='300'><img src='./readme_images/phong.png' width='400'><br />
<br />

Applying the loop subdivision stencil to vertex normals:<br />
<img src='./readme_images/normal_weighted_average.png' width='300'><br />
<br />
The orthogonal plane projection in theory comes down to the following. Each neighboring normal is set to a similar origin point. All of these normals are going to be projected on the plane orthogonal to the normal being computed. The projection is essentially the shadow that falls on the plane as an infinitely far away light source shines directly onto the plane:<br />
<img src='./readme_images/orthogonal_plane_projection.png' width='300'><br />
<br />
Compute a weighted average of projected normals:<br />
<img src='./readme_images/weight_projected_normals.png' width='300'><br />
<br />
Tilt normal in question on axis nk ×  ̃nk+1:<br />
<img src='./readme_images/tilt_normal.png' width='300'><br />


Startup view with disabled unused ui elements:
![disabled_ui](./readme_images/disabled_ui.png)

The following images show a Tetrahedron wireframe visual subdivided in zero to 4 steps:
![tetrahedron_wireframe_0](./readme_images/tetrahedron_wireframe_0.png)
![tetrahedron_wireframe_1](./readme_images/tetrahedron_wireframe_1.png)
![tetrahedron_wireframe_2](./readme_images/tetrahedron_wireframe_2.png)
![tetrahedron_wireframe_3](./readme_images/tetrahedron_wireframe_3.png)
![tetrahedron_wireframe_4](./readme_images/tetrahedron_wireframe_4.png)

Tetrahedron after 4 subdivision steps displayed with phong shading:
![tetrahedron_phong_4](./readme_images/tetrahedron_phong_4.png)

Tetrahedron after 4 subdivision steps displayed with isophotes shading:
![tetrahedron_iso_4](./readme_images/tetrahedron_iso_4.png)

Isophotes with higher frequency:
![tetrahedron_isofrequency_4](./readme_images/tetrahedron_isofrequency_4.png)

The following images show a Open Cube wireframe visual subdivided in zero to 4 steps:
![opencube_wireframe_0](./readme_images/opencube_wireframe_0.png)
![opencube_wireframe_1](./readme_images/opencube_wireframe_1.png)
![opencube_wireframe_2](./readme_images/opencube_wireframe_2.png)
![opencube_wireframe_3](./readme_images/opencube_wireframe_3.png)
![opencube_wireframe_4](./readme_images/opencube_wireframe_4.png)

Open Cube with phong shading:
![opencube_phong_4](./readme_images/opencube_phong_4.png)

Isophotes for the Open Cube:
![opencube_iso_4](./readme_images/opencube_iso_4.png)
