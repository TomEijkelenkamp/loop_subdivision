#include "butterflysubdivisionshader.h"
#include <QDebug>

ButterflySubdivisionShader::ButterflySubdivisionShader() {}

/**
 * @brief ButterflySubdivisionShader::normalRefinement Refines the normals based on
 * the Loop subdivision stencils and Subdivision Shading described in the
 * 2008 paper by Marc Alexa and Tamy Boubekeur.
 */
void ButterflySubdivisionShader::normalRefinement(Mesh& controlMesh,
                                             Mesh& newMesh) const {
    // Compute normals with angle-weighted average of incident faces normals.
    newMesh.computeBaseNormals();

    // Compute subdivision shading normals with Butterfly subdivision
    for (int subdivType = BUTTERFLY; subdivType <= BUTTERFLY; ++subdivType) {
        SubdivisionShaderType averagingMethod = static_cast<SubdivisionShaderType>(subdivType);

        QVector<Vertex>& vertices = controlMesh.getVertices();
        QVector<HalfEdge>& halfEdges = controlMesh.getHalfEdges();
        QVector<QVector3D>& normals = controlMesh.getVertexSubdivNormals(averagingMethod);
        QVector<QVector3D>& newNormals = newMesh.getVertexSubdivNormals(averagingMethod);

        // Vertex normals
        for (int v = 0; v < controlMesh.numVerts(); v++) {
            newNormals[v] = normals[v].normalized();
        }

        // Edge normals, i.e. the normals of newly created vertices
        for (int h = 0; h < controlMesh.numHalfEdges(); h++) {
            HalfEdge currentEdge = halfEdges[h];
            if (h > currentEdge.twinIdx()) {
                int v = controlMesh.numVerts() + currentEdge.edgeIdx();
                newNormals[v] = edgeNormal(currentEdge, normals).normalized();
            }
        }

        // Write new normals to mesh
        newMesh.setSubdividedNormals(averagingMethod, newNormals);
    }
}

/**
 * @brief ButterflySubdivisionShader::vertexNormal In Butterfly subdivision, simply return the original coordinates
 * (i.e. normal in this case) when the vertex existed in the previous subdivision step.
 * @param vertex
 * @param normals
 * @return
 */
QVector3D ButterflySubdivisionShader::vertexNormal(const Vertex& vertex, const QVector<QVector3D> normals) const {
    return normals[vertex.index];
}

QVector3D ButterflySubdivisionShader::edgeNormal(const HalfEdge& edge, const QVector<QVector3D> normals) const {
    // Define the tension parameter w
    float w = 1.0 / 16.0;

    if (edge.isBoundaryEdge()) {
        int v1 = edge.origin->index;
        int v2 = edge.next->origin->index;

        QVector3D newNormal = normals[v1] / 2.0 + normals[v2] / 2.0;
        return newNormal.normalized();
    }

    int v1 = edge.origin->index;
    int v2 = edge.next->origin->index;
    int v3 = edge.next->next->origin->index;
    int v4 = edge.twin->next->next->origin->index;

    // Check if the 'butterfly' vertices exist
    QVector3D v5, v6, v7, v8 = { 0.0, 0.0, 0.0};

    if (edge.prev->twin != nullptr) {
        v5 = normals[edge.prev->twin->prev->origin->index];
    }

    if (edge.twin->next->twin != nullptr) {
        v6 = normals[edge.twin->next->twin->prev->origin->index];
    }

    if (edge.twin->prev->twin != nullptr) {
        v7 = normals[edge.twin->prev->twin->prev->origin->index];
    }

    if (edge.next->twin != nullptr) {
        v8 = normals[edge.next->twin->prev->origin->index];
    }

    return (normals[v1] + normals[v2]) / 2.0 + 2.0 * w * (normals[v3] + normals[v4]) - w * (v5 + v6 + v7 + v8);
}
