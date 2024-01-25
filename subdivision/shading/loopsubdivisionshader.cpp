#include "loopsubdivisionshader.h"
#include <QDebug>

LoopSubdivisionShader::LoopSubdivisionShader() {}

/**
 * @brief LoopSubdivisionShader::normalRefinement Refines the normals based on
 * the Loop subdivision stencils and Subdivision Shading described in the
 * 2008 paper by Marc Alexa and Tamy Boubekeur.
 */
void LoopSubdivisionShader::normalRefinement(Mesh& controlMesh,
                                      Mesh& newMesh) const {
    // Compute normals with angle-weighted average of incident faces normals.
    newMesh.computeBaseNormals();

    // Compute subdivision shading normals with Loop subdivision
    for (int subdivType = LINEAR; subdivType <= SPHERICAL; ++subdivType) {
        SubdivisionShaderType averagingMethod = static_cast<SubdivisionShaderType>(subdivType);

        QVector<Vertex>& vertices = controlMesh.getVertices();
        QVector<HalfEdge>& halfEdges = controlMesh.getHalfEdges();
        QVector<QVector3D>& normals = controlMesh.getVertexSubdivNormals(averagingMethod);
        QVector<QVector3D>& newNormals = newMesh.getVertexSubdivNormals(averagingMethod);

        // Vertex normals
        for (int v = 0; v < controlMesh.numVerts(); v++) {
            newNormals[v] = vertexNormal(vertices[v], normals).normalized();

            if (subdivType == SPHERICAL) {
                newNormals[v] = sphericalAveragingVertex(vertices[v], newNormals[v], normals);
            }
        }

        // Edge normals, i.e. the normals of newly created vertices
        for (int h = 0; h < controlMesh.numHalfEdges(); h++) {
            HalfEdge currentEdge = halfEdges[h];
            if (h > currentEdge.twinIdx()) {
                int v = controlMesh.numVerts() + currentEdge.edgeIdx();
                newNormals[v] = edgeNormal(currentEdge, normals).normalized();

                if (subdivType == SPHERICAL) {
                    QVector3D temp = newNormals[v];
                    newNormals[v] = sphericalAveragingEdge(currentEdge, newNormals[v], normals);
                    qDebug() << temp << " " << newNormals[v];
                }
            }
        }

        // Write new normals to mesh
        newMesh.setSubdividedNormals(averagingMethod, newNormals);
    }
}

QVector3D LoopSubdivisionShader::vertexNormal(const Vertex& vertex, const QVector<QVector3D> normals) const {
    if (vertex.isBoundaryVertex()) {
        int v0 = vertex.index;
        int v1 = vertex.prevBoundaryHalfEdge()->origin->index;
        int v2 = vertex.nextBoundaryHalfEdge()->next->origin->index;

        return (normals[v1] + 6.0 * normals[v0] + normals[v2]).normalized();
    }

    float valence = vertex.valence;
    float beta = (valence == 3.0 ? 3.0 / 16.0 : 3.0 / (8.0 * valence));

    int v0 = vertex.index;

    QVector3D normal = normals[v0] * (1.0 - valence * beta);
    HalfEdge* halfedge = vertex.out->twin;

    do {
        int vNext = halfedge->origin->index;
        normal += normals[vNext] * beta;
        halfedge = halfedge->next->twin;
    } while (halfedge != vertex.out->twin);

    return normal;
}

QVector3D LoopSubdivisionShader::edgeNormal(const HalfEdge& edge, const QVector<QVector3D> normals) const {
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

    return (6.0 * normals[v1] + 6.0 * normals[v2] + 2.0 * normals[v3] + 2.0 * normals[v4]);
}

void LoopSubdivisionShader::blendWeightsRefinement(Mesh& controlMesh,
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
 * @brief LoopSubdivisionShader::vertexBlendWeight Compute blend weight by interpolating over neighbors'
          blend weights using Loop's vertex stencil.
 * @param vertex
 * @param blendWeights
 * @return
 */
float LoopSubdivisionShader::vertexBlendWeight(const Vertex& vertex, const QVector<float> blendWeights) const {
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
float LoopSubdivisionShader::edgeBlendWeight(const HalfEdge& edge, const QVector<float> blendWeights) const {
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

QVector3D LoopSubdivisionShader::sphericalAveragingVertex(const Vertex& vertex,
                                                          QVector3D linearlyAveragedNormal,
                                                          const QVector<QVector3D> normals) const {
    int stopCriterion = 3;

    // Compute beta of Warren's stencil
    float valence = vertex.valence;
    float beta = (valence == 3.0 ? 3.0 / 16.0 : 3.0 / (8.0 * valence));

    // Set nk to the result from step 1., which is the same as the linear weighted average we have done before.
    QVector3D nk = linearlyAveragedNormal;

    for (int i = 0; i < stopCriterion; ++i) {
        QVector3D nk1squiggle;

        if (vertex.isBoundaryVertex()) {
            // 2.  Map all input normals orthogonally to a plane orthogonal to n^k
            QVector3D n0squiggle = createExponentialMap(nk, normals[vertex.index]);
            QVector3D n1squiggle = createExponentialMap(nk, normals[vertex.prevBoundaryHalfEdge()->origin->index]);
            QVector3D n2squiggle = createExponentialMap(nk, normals[vertex.nextBoundaryHalfEdge()->next->origin->index]);

            // 3. Perform linear combination in the exponential map to obtain \~{n}^{k+1}
            nk1squiggle = (n1squiggle + 6.0 * n0squiggle + n2squiggle) / 8.0;
        } else {
            // Step 2 and 3. combined
            QVector3D nk1squiggle = (1 - valence * beta) * createExponentialMap(nk, normals[vertex.index]);
            HalfEdge* halfedge = vertex.out->twin;

            do {
                int vNext = halfedge->origin->index;
                nk1squiggle += beta * createExponentialMap(nk, normals[vNext]);
                halfedge = halfedge->next->twin;
            } while (halfedge != vertex.out->twin);
        }

        // 4. Rotate n^k around n^k x \~{n}^{k+1} with angle ||nk+1squiggle|| to obtain n^{k+1}
        QVector3D nk1 = rotateAroundAxis(nk, nk1squiggle, nk1squiggle.length());

        // 5. Compute the dot product between n^k and n^k+1 to check if the stop criterion has been satisfied
        // stopCriterion = QVector3D::dotProduct(nk, nk1);

        // Set the result
        nk = nk1;
    }

    return nk;
}

QVector3D LoopSubdivisionShader::sphericalAveragingEdge(const HalfEdge& edge,
                                                        QVector3D linearlyAveragedNormal,
                                                        const QVector<QVector3D> normals) const {
    int stopCriterion = 3;

    // Set nk to the result from step 1., which is the same as the linear weighted average we have done before.
    QVector3D nk = linearlyAveragedNormal;

    for (int i = 0; i < stopCriterion; ++i) {
        QVector3D nk1squiggle;

        if (edge.isBoundaryEdge()) {
            // 2.  Map all input normals orthogonally to a plane orthogonal to n^k
            QVector3D n1squiggle = createExponentialMap(nk, normals[edge.origin->index]);
            QVector3D n2squiggle = createExponentialMap(nk, normals[edge.next->origin->index]);

            // 3. Perform linear combination in the exponential map to obtain \~{n}^{k+1}
            nk1squiggle = n1squiggle / 2.0 + n2squiggle / 2.0;
        } else {
            // 2.  Map all input normals orthogonally to a plane orthogonal to n^k
            QVector3D n1squiggle = createExponentialMap(nk, normals[edge.origin->index]);
            QVector3D n2squiggle = createExponentialMap(nk, normals[edge.next->origin->index]);
            QVector3D n3squiggle = createExponentialMap(nk, normals[edge.next->next->origin->index]);
            QVector3D n4squiggle = createExponentialMap(nk, normals[edge.twin->next->next->origin->index]);

            nk1squiggle = (6.0 * n1squiggle + 6.0 * n2squiggle + 2.0 * n3squiggle + 2.0 * n4squiggle) / 16.0;
        }

        // 4. Rotate n^k around n^k x \~{n}^{k+1} with angle ||nk+1squiggle|| to obtain n^{k+1}
        QVector3D nk1 = rotateAroundAxis(nk, nk1squiggle, nk1squiggle.length());

        // 5. Compute the dot product between n^k and n^k+1 to check if the stop criterion has been satisfied
        // stopCriterion = QVector3D::dotProduct(nk, nk1);

        // Set the result
        nk = nk1;
    }

    return nk;
}

/**
 * @brief LoopSubdivisionShader::createExponentialMap Convert n^i to a vector in the exponential map of n^k.
 * This means that the function maps n^i to a plane orthogonal to n^k and scales this projection to have a
 * distance to origin corresponding to the angle to n^k.
 * @param nk
 * @param ni
 * @return
 */
QVector3D LoopSubdivisionShader::createExponentialMap(QVector3D nk, QVector3D ni) const {
    // Project n^k unto n^i
    QVector3D projection = ni - QVector3D::dotProduct(ni, nk) * nk;
    projection.normalize();

    // Find angle between n^k and n^i
    float angle = acos(QVector3D::dotProduct(nk.normalized(), ni.normalized())); // 180.0f / M_PI * ... to convert to degrees

    return angle * projection;
}

/**
 * @brief LoopSubdivisionShader::rotateAroundAxis Rotate a vector around a specified axis
 * using Rodrigues' rotation formula.
 * @param vector
 * @param axis
 * @param angle
 * @return
 */
QVector3D LoopSubdivisionShader::rotateAroundAxis(QVector3D vector, QVector3D secondVector, float angle) const {
    QVector3D axis = QVector3D::crossProduct(vector, secondVector).normalized();

    return vector * cos(angle)
           + QVector3D::crossProduct(axis, vector) * sin(angle)
           + axis * QVector3D::dotProduct(axis, vector) * (1 - cos(angle));
}
