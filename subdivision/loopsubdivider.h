#ifndef LOOP_SUBDIVIDER_H
#define LOOP_SUBDIVIDER_H

#include "mesh/mesh.h"
#include "subdivider.h"

/**
 * @brief The LoopSubdivider class is a subdivider class that performs Loop
 * subdivision on triangle meshes.
 */
class LoopSubdivider : public Subdivider {
 public:
  LoopSubdivider();
  Mesh subdivide(Mesh& controlMesh) const override;

 private:
  void reserveSizes(Mesh& controlMesh, Mesh& newMesh) const;
  void geometryRefinement(Mesh& controlMesh, Mesh& newMesh) const;
  void topologyRefinement(Mesh& controlMesh, Mesh& newMesh) const;
  void normalRefinement(Mesh& controlMesh, Mesh& newMesh) const;

  void setHalfEdgeData(Mesh& newMesh, int h, int edgeIdx, int vertIdx,
                       int twinIdx) const;

  QVector3D vertexPoint(const Vertex& vertex) const;
  QVector3D edgePoint(const HalfEdge& edge) const;

  QVector3D vertexNormal(const Vertex& vertex, const QVector<QVector3D> normals) const;
  QVector3D edgeNormal(const HalfEdge& edge, const QVector<QVector3D> normals) const;
};

#endif  // LOOP_SUBDIVIDER_H
