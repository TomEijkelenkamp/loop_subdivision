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

# Phong shading
Normal-vector interpolation shading (Phong) uses the formula to compute light intensities: I = k<sub>a</sub> + k<sub>d</sub> (L dot N) + k<sub>s</sub> (R dot V)<sup>α</sup>.  Interpolation of normals shown on the left, vectors used on the right:<br />
<img src='./readme_images/phong_normal_interpolation.png' width='300'><img src='./readme_images/phong.png' width='400'><br />
<br />

# Subdivision shading
The following images show steps involved to redirect all the normals in the mesh to an orientation that is continues relative to its neighbors.<br />
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
Tilt normal in question on axis n<sub>k</sub> ×  ̃n<sup>k+1</sup>:<br />
<img src='./readme_images/tilt_normal.png' width='300'><br />

<img src='./readme_images/results/raster_0subdiv.jpg' width='300'><br />
<img src='./readme_images/results/raster_1subdiv.jpg' width='300'><br />
<img src='./readme_images/results/raster_2subdiv.jpg' width='300'><br />
<br />
<img src='./readme_images/results/standard_phong.jpg' width='300'><br />
<img src='./readme_images/results/subdiv_linear_phong.jpg' width='300'><br />
<br />
<img src='./readme_images/results/standard_normal.jpg' width='300'><br />
<img src='./readme_images/results/subdiv_linear_normal.jpg' width='300'><br />
<br />
<img src='./readme_images/results/standard_isophote.jpg' width='300'><br />
<img src='./readme_images/results/subdiv_linear_isophote.jpg' width='300'><br />
<br />
<br />
<br />
<img src='./readme_images/results/subdiv_linear_blend_phong.jpg' width='300'><br />
<img src='./readme_images/results/subdiv_spherical_phong.jpg' width='300'><br />
<br />
<img src='./readme_images/results/subdiv_linear_blend_normal.jpg' width='300'><br />
<img src='./readme_images/results/subdiv_spherical_normal.jpg' width='300'><br />
<br />
<img src='./readme_images/results/subdiv_linear_blend_isophote.jpg' width='300'><br />
<img src='./readme_images/results/subdiv_spherical_isophote.jpg' width='300'><br />
