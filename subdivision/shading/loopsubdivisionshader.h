#ifndef LOOPSUBDIVISIONSHADER_H
#define LOOPSUBDIVISIONSHADER_H

#include "subdivisionshader.h"

class LoopSubdivisionShader : public SubdivisionShader
{
public:
    LoopSubdivisionShader();

    virtual void normalRefinement(Mesh& controlMesh, Mesh& newMesh) const;

    virtual QVector3D vertexNormal(const Vertex& vertex, const QVector<QVector3D> normals) const;
    virtual QVector3D edgeNormal(const HalfEdge& edge, const QVector<QVector3D> normals) const;

    QVector3D sphericalAveragingVertex(const Vertex& vertex, QVector3D linearlyAveragedNormal, const QVector<QVector3D> normals) const;
    QVector3D sphericalAveragingEdge(const HalfEdge& edge, QVector3D linearlyAveragedNormal, const QVector<QVector3D> normals) const;

    QVector3D createExponentialMap(QVector3D nk, QVector3D ni) const;
    QVector3D rotateAroundAxis(QVector3D vector, QVector3D secondVector, float angle) const;
};

#endif // LOOPSUBDIVISIONSHADER_H
