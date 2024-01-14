#ifndef LOOPSUBDIVISIONSHADER_H
#define LOOPSUBDIVISIONSHADER_H

#include "mesh/mesh.h"

class LoopSubdivisionShader
{
public:
    LoopSubdivisionShader();

    void normalRefinement(Mesh& controlMesh, Mesh& newMesh) const;

    QVector3D vertexNormal(const Vertex& vertex, const QVector<QVector3D> normals) const;
    QVector3D edgeNormal(const HalfEdge& edge, const QVector<QVector3D> normals) const;

    QVector3D sphericalAveragingVertex(const Vertex& vertex, QVector3D linearlyAveragedNormal, const QVector<QVector3D> normals) const;
    QVector3D sphericalAveragingEdge(const HalfEdge& edge, QVector3D linearlyAveragedNormal, const QVector<QVector3D> normals) const;

    QVector3D createExponentialMap(QVector3D nk, QVector3D ni) const;
    QVector3D rotateAroundAxis(QVector3D vector, QVector3D secondVector, float angle) const;

    void blendWeightsRefinement(Mesh& controlMesh, Mesh& newMesh) const;
    float interpolatedBlendWeight(const HalfEdge& edge, const QVector<float> blendWeights) const;
};

#endif // LOOPSUBDIVISIONSHADER_H
