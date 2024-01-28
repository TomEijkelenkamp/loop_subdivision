#ifndef BUTTERFLYSUBDIVISIONSHADER_H
#define BUTTERFLYSUBDIVISIONSHADER_H

#include "subdivisionshader.h"

class ButterflySubdivisionShader : public SubdivisionShader
{
public:
    ButterflySubdivisionShader();

    void normalRefinement(Mesh& controlMesh, Mesh& newMesh) const override;

    QVector3D vertexNormal(const Vertex& vertex, const QVector<QVector3D> normals) const override;
    QVector3D edgeNormal(const HalfEdge& edge, const QVector<QVector3D> normals) const override;
};

#endif // BUTTERFLYSUBDIVISIONSHADER_H
