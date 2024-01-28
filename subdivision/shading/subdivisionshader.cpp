#include "subdivisionshader.h"

/**
 * @brief SubdivisionShader::~SubdivisionShader
 */
SubdivisionShader::~SubdivisionShader() {}

void SubdivisionShader::blendWeightsRefinement(Mesh& controlMesh,
                                               Mesh& newMesh) const {
    QVector<Vertex>& vertices = controlMesh.getVertices();
    QVector<HalfEdge>& halfEdges = controlMesh.getHalfEdges();
    QVector<float> blendWeights = controlMesh.getBlendWeights();
    QVector<float> newBlendWeights;
    newBlendWeights.fill(0.0, newMesh.numVerts());

    // Copy old blend weights to new array
    for (int v = 0; v < controlMesh.numVerts(); v++) {
        newBlendWeights[v] = vertexBlendWeight(vertices[v], blendWeights);
    }

    // Loop over the vertices that have been added and interpolate
    for (int h = 0; h < controlMesh.numHalfEdges(); h++) {
        HalfEdge currentEdge = halfEdges[h];
        if (h > currentEdge.twinIdx()) {
            int v = controlMesh.numVerts() + currentEdge.edgeIdx();
            newBlendWeights[v] = edgeBlendWeight(currentEdge, blendWeights);
        }
    }

    newMesh.setBlendWeights(newBlendWeights);
}

/**
 * @brief SubdivisionShader::vertexBlendWeight Compute blend weight by interpolating over neighbors'
          blend weights using Loop's vertex stencil.
 * @param vertex
 * @param blendWeights
 * @return
 */
float SubdivisionShader::vertexBlendWeight(const Vertex& vertex, const QVector<float> blendWeights) const {
    if (vertex.isBoundaryVertex()) {
        int v0 = vertex.index;
        int v1 = vertex.prevBoundaryHalfEdge()->origin->index;
        int v2 = vertex.nextBoundaryHalfEdge()->next->origin->index;

        return (blendWeights[v1] + 6.0 * blendWeights[v0] + blendWeights[v2]) / 8.0;
    }

    float valence = vertex.valence;
    float beta = (valence == 3.0 ? 3.0 / 16.0 : 3.0 / (8.0 * valence));

    int v0 = vertex.index;

    float blendWeight = blendWeights[v0] * (1.0 - valence * beta);
    HalfEdge* halfedge = vertex.out->twin;

    do {
        int vNext = halfedge->origin->index;
        blendWeight += blendWeights[vNext] * beta;
        halfedge = halfedge->next->twin;
    } while (halfedge != vertex.out->twin);

    return blendWeight;
}

/**
 * @brief interpolatedBlendWeight Compute blend weight by interpolating over neighbors'
          blend weights using Loop's edge stencil.
 * @param edge
 * @param blendWeights
 * @return
 */
float SubdivisionShader::edgeBlendWeight(const HalfEdge& edge, const QVector<float> blendWeights) const {
    if (edge.isBoundaryEdge()) {
        int v1 = edge.origin->index;
        int v2 = edge.next->origin->index;

        return blendWeights[v1] / 2.0 + blendWeights[v2] / 2.0;
    }

    int v1 = edge.origin->index;
    int v2 = edge.next->origin->index;
    int v3 = edge.next->next->origin->index;
    int v4 = edge.twin->next->next->origin->index;

    return (6.0 * blendWeights[v1] + 6.0 * blendWeights[v2] + 2.0 * blendWeights[v3] + 2.0 * blendWeights[v4]) / 16.0;
}
