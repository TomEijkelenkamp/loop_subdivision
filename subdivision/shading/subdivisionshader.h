#ifndef SUBDIVISIONSHADER_H
#define SUBDIVISIONSHADER_H

#include "mesh/mesh.h"

class SubdivisionShader
{
public:
    virtual ~SubdivisionShader();

    virtual void normalRefinement(Mesh& controlMesh, Mesh& newMesh) const = 0;

    virtual QVector3D vertexNormal(const Vertex& vertex, const QVector<QVector3D> normals) const = 0;
    virtual QVector3D edgeNormal(const HalfEdge& edge, const QVector<QVector3D> normals) const = 0;

    void blendWeightsRefinement(Mesh& controlMesh, Mesh& newMesh) const;
    float vertexBlendWeight(const Vertex& vertex, const QVector<float> blendWeights) const;
    float edgeBlendWeight(const HalfEdge& edge, const QVector<float> blendWeights) const;
};

#endif // SUBDIVISIONSHADER_H
