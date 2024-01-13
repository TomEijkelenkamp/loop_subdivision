#ifndef MESH_H
#define MESH_H

#include <QVector>

#include "face.h"
#include "halfedge.h"
#include "vertex.h"

/**
 * @brief The Mesh class Representation of a mesh using the half-edge data
 * structure.
 */
class Mesh {
 public:
  Mesh();
  ~Mesh();

  inline QVector<Vertex>& getVertices() { return vertices; }
  inline QVector<HalfEdge>& getHalfEdges() { return halfEdges; }
  inline QVector<Face>& getFaces() { return faces; }

  inline QVector<QVector3D>& getVertexCoords() { return vertexCoords; }
  inline QVector<QVector3D>& getVertexNorms() { return vertexNormals; }
  inline QVector<QVector3D>& getVertexSubdivNormals() { return vertexNormalsSubdivided; }
  inline QVector<float>& getBlendWeights() { return vertexBlendWeights; }
  inline QVector<unsigned int>& getPolyIndices() { return polyIndices; }

  inline void setSubdividedNormals(QVector<QVector3D>& newNormals) { vertexNormalsSubdivided = newNormals; }
  inline void setBlendWeights(QVector<float>& blendWeights) { vertexBlendWeights = blendWeights; }
  QVector<QVector3D>& getBlendedVertexNormals();

  void extractAttributes();
  void computeBaseNormals();

  int numVerts();
  int numHalfEdges();
  int numFaces();
  int numEdges();

  bool isBaseMesh = false;
  void setBaseMesh(bool value);
  void computeBaseBlendWeights();

 private:
  QVector<QVector3D> vertexCoords;
  QVector<QVector3D> vertexNormals;
  QVector<QVector3D> vertexNormalsSubdivided;
  QVector<QVector3D> blendedNormals;
  QVector<float> vertexBlendWeights;
  QVector<unsigned int> polyIndices;

  QVector<Vertex> vertices;
  QVector<Face> faces;
  QVector<HalfEdge> halfEdges;

  int edgeCount;

  // These classes require access to the private fields to prevent a bunch of
  // function calls.
  friend class MeshInitializer;
  friend class Subdivider;
  friend class LoopSubdivider;
};

#endif  // MESH_H
