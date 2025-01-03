#include "mesh.h"

#include <assert.h>
#include <math.h>

#include <QDebug>

/**
 * @brief Mesh::Mesh Initializes an empty mesh.
 */
Mesh::Mesh() {}

/**
 * @brief Mesh::~Mesh Deconstructor. Clears all the data of the half-edge data.
 */
Mesh::~Mesh() {
    vertices.clear();
    vertices.squeeze();
    halfEdges.clear();
    halfEdges.squeeze();
    faces.clear();
    faces.squeeze();
}

/**
 * @brief Mesh::setBaseMesh
 * @param value
 */
void Mesh::setBaseMesh(bool value) {
    if (value) {
        isBaseMesh = value;

        // Set the subdivision normals to base normals
        computeBaseNormals();

        for (int subdivType = LINEAR; subdivType <= BUTTERFLY; ++subdivType) {
            setSubdividedNormals(static_cast<SubdivisionShaderType>(subdivType), getVertexNorms());
        }

        // Set the blend weights
        computeBaseBlendWeights();
    }
}

/**
 * @brief Mesh::computeBaseNormals Computes the face and vertex normals with an
 * angle-weighted average of incident faces normals.
 */
void Mesh::computeBaseNormals() {
    for (int f = 0; f < numFaces(); f++) {
        faces[f].recalculateNormal();
    }

    vertexNormals.clear();
    vertexNormals.fill({0, 0, 0}, numVerts());

    // normal computation
    for (int h = 0; h < numHalfEdges(); ++h) {
        HalfEdge* edge = &halfEdges[h];
        QVector3D pPrev = edge->prev->origin->coords;
        QVector3D pCur = edge->origin->coords;
        QVector3D pNext = edge->next->origin->coords;

        QVector3D edgeA = (pPrev - pCur);
        QVector3D edgeB = (pNext - pCur);

        double edgeLengths = edgeA.length() * edgeB.length();
        double edgeDot = QVector3D::dotProduct(edgeA, edgeB) / edgeLengths;
        double angle = sqrt(1 - edgeDot * edgeDot);

        vertexNormals[edge->origin->index] +=
            (angle * edge->face->normal) / edgeLengths;
    }

    for (int v = 0; v < numVerts(); ++v) {
        vertexNormals[v].normalize();
    }
}

void Mesh::computeBaseBlendWeights() {
    QVector<Vertex> vertices = getVertices();

    vertexBlendWeights.clear();
    vertexBlendWeights.fill(0.0, numVerts());

    for (int v = 0; v < numVerts(); ++v) {
        if (vertices[v].valence != 6) {
            vertexBlendWeights[v] = 1.0;
        }
    }
}

QVector<QVector3D>& Mesh::getBlendedVertexNormals(SubdivisionShaderType subdivType) {
    blendedNormals.clear();
    blendedNormals.fill({0.0, 0.0, 0.0}, numVerts());

    for (int v = 0; v < numVerts(); ++v) {
        blendedNormals[v] = vertexBlendWeights[v] * vertexNormalsSubdivided[subdivType][v]
                            + (1.0 - vertexBlendWeights[v]) * vertexNormals[v];
    }

    return blendedNormals;
}

/**
 * @brief Mesh::extractAttributes Extracts the normals, vertex coordinates and
 * indices into easy-to-access buffers.
 */
void Mesh::extractAttributes() {
    computeBaseNormals();

    vertexCoords.clear();
    vertexCoords.reserve(vertices.size());
    for (int v = 0; v < vertices.size(); v++) {
        vertexCoords.append(vertices[v].coords);
    }

    polyIndices.clear();
    polyIndices.reserve(halfEdges.size() + faces.size());
    for (int f = 0; f < faces.size(); f++) {
        HalfEdge* currentEdge = faces[f].side;
        for (int m = 0; m < faces[f].valence; m++) {
            polyIndices.append(currentEdge->origin->index);
            currentEdge = currentEdge->next;
        }
    }
}

/**
 * @brief Mesh::numVerts Retrieves the number of vertices.
 * @return The number of vertices.
 */
int Mesh::numVerts() { return vertices.size(); }

/**
 * @brief Mesh::numHalfEdges Retrieves the number of half-edges.
 * @return The number of half-edges.
 */
int Mesh::numHalfEdges() { return halfEdges.size(); }

/**
 * @brief Mesh::numFaces Retrieves the number of faces.
 * @return The number of faces.
 */
int Mesh::numFaces() { return faces.size(); }

/**
 * @brief Mesh::numEdges Retrieves the number of edges.
 * @return The number of edges.
 */
int Mesh::numEdges() { return edgeCount; }
