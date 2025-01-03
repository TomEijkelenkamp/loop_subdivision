#include "loopsubdivider.h"

#include <QDebug>

/**
 * @brief LoopSubdivider::LoopSubdivider Creates a new empty Loop subdivider.
 */
LoopSubdivider::LoopSubdivider() {}

/**
 * @brief LoopSubdivider::subdivide Subdivides the provided control mesh and
 * returns the subdivided mesh. Performs just a single subdivision step. The
 * subdivision follows the indexing rules of this paper:
 * https://diglib.eg.org/bitstream/handle/10.2312/egs20221028/041-044.pdf?sequence=1&isAllowed=y
 * @param controlMesh The mesh to be subdivided.
 * @return The mesh resulting of applying a single subdivision step on the
 * control mesh.
 */
Mesh LoopSubdivider::subdivide(Mesh& controlMesh) const {
    Mesh newMesh;
    reserveSizes(controlMesh, newMesh);
    geometryRefinement(controlMesh, newMesh);
    topologyRefinement(controlMesh, newMesh);

    subdivisionShaderLoop.normalRefinement(controlMesh, newMesh);
    subdivisionShaderLoop.blendWeightsRefinement(controlMesh, newMesh);
    subdivisionShaderButterfly.normalRefinement(controlMesh, newMesh);
    subdivisionShaderButterfly.blendWeightsRefinement(controlMesh, newMesh);

    return newMesh;
}

/**
 * @brief LoopSubdivider::reserveSizes Resizes the vertex, half-edge and face
 * vectors. Aslo recalculates the edge count.
 * @param controlMesh The control mesh.
 * @param newMesh The new mesh. At this point, the mesh is fully empty.
 */
void LoopSubdivider::reserveSizes(Mesh& controlMesh, Mesh& newMesh) const {
    int newNumEdges = 2 * controlMesh.numEdges() + 3 * controlMesh.numFaces();
    int newNumFaces = controlMesh.numFaces() * 4;
    int newNumHalfEdges = controlMesh.numHalfEdges() * 4;
    int newNumVerts = controlMesh.numVerts() + controlMesh.numEdges();

    newMesh.getVertices().resize(newNumVerts);
    newMesh.getHalfEdges().resize(newNumHalfEdges);
    newMesh.getFaces().resize(newNumFaces);

    for (int subdivType = LINEAR; subdivType <= BUTTERFLY; ++subdivType) {
        newMesh.getVertexSubdivNormals(static_cast<SubdivisionShaderType>(subdivType)).resize(newNumVerts);
    }

    newMesh.edgeCount = newNumEdges;
}

/**
 * @brief LoopSubdivider::geometryRefinement Performs the geometry refinement.
 * In other words, it calculates the coordinates of the vertex and edge points.
 * @param controlMesh The control mesh.
 * @param newMesh The new mesh. At the start of this function, the only
 * guarantee you have of this newMesh is that the vertex, half-edge and face
 * vectors have the correct sizes.
 */
void LoopSubdivider::geometryRefinement(Mesh& controlMesh,
                                        Mesh& newMesh) const {
    QVector<Vertex>& newVertices = newMesh.getVertices();
    QVector<Vertex>& vertices = controlMesh.getVertices();

    // Vertex Points
    for (int v = 0; v < controlMesh.numVerts(); v++) {
        QVector3D coords = vertexPoint(vertices[v]);
        Vertex vertPoint(coords, nullptr, vertices[v].valence, v);
        newVertices[v] = vertPoint;
    }

    // Edge Points
    QVector<HalfEdge>& halfEdges = controlMesh.getHalfEdges();
    for (int h = 0; h < controlMesh.numHalfEdges(); h++) {
        HalfEdge currentEdge = halfEdges[h];
        // Only create a new vertex per set of halfEdges (i.e. once per undirected
        // edge)
        if (h > currentEdge.twinIdx()) {
            QVector3D coords = edgePoint(currentEdge);
            int v = controlMesh.numVerts() + currentEdge.edgeIdx();
            int valence = currentEdge.isBoundaryEdge() ? 4 : 6;
            Vertex edgePointVert = Vertex(coords, nullptr, valence, v);
            newVertices[v] = edgePointVert;
        }
    }
}

/**
 * @brief LoopSubdivider::vertexPoint Calculates the new position of the
 * provided vertex.
 * @param vertex The vertex to calculate the new position of. Note that this
 * vertex is the vertex from the control mesh.
 * @return The coordinates of the new vertex point.
 */
QVector3D LoopSubdivider::vertexPoint(const Vertex& vertex) const {
    QVector3D coords;
    HalfEdge* halfedge;

    float beta;
    if (vertex.isBoundaryVertex()) {
        return (vertex.prevBoundaryHalfEdge()->origin->coords + 6 * vertex.coords + vertex.nextBoundaryHalfEdge()->next->origin->coords) / 8;
    }

    if (vertex.valence == 6) {
        // We use beta to do normalized weighting immediately
        beta = 1.0 / (10.0 + vertex.valence);
        // Weight mid vertex
        coords = vertex.coords * 10.0 * beta;
    } else {
        beta = vertex.valence == 3.0 ? 3.0/16.0 : 3.0/(8.0*vertex.valence);
        coords = vertex.coords * (1.0 - vertex.valence * beta);
    }

    halfedge = vertex.out->twin;
    do {
        coords += halfedge->origin->coords * beta;
        halfedge = halfedge->next->twin;
    } while (halfedge != vertex.out->twin);

    return coords;
}

/**
 * @brief LoopSubdivider::edgePoint Calculates the position of the edge point.
 * @param edge One of the half-edges that lives on the edge to calculate
 * the edge point. Note that this half-edge is the half-edge from the control
 * mesh.
 * @return The coordinates of the new edge point.
 */
QVector3D LoopSubdivider::edgePoint(const HalfEdge& edge) const {
    if (edge.isBoundaryEdge()) {
        return edge.origin->coords / 2.0 + edge.next->origin->coords / 2.0;
    }

    QVector3D edgePt = edge.origin->coords * 6.0;
    edgePt += edge.next->origin->coords * 6.0;
    edgePt += edge.next->next->origin->coords * 2.0;
    edgePt += edge.twin->next->next->origin->coords * 2.0;
    return edgePt /= 16.0;
}

/**
 * @brief LoopSubdivider::topologyRefinement Performs the topology refinement.
 * Already takes into consideration the boundaries, so you do not need to alter
 * the geometry refinement for this assignment.
 * @param controlMesh The control mesh.
 * @param newMesh The new mesh.
 */
void LoopSubdivider::topologyRefinement(Mesh& controlMesh,
                                        Mesh& newMesh) const {
    for (int f = 0; f < newMesh.numFaces(); ++f) {
        newMesh.faces[f].index = f;
        // Loop subdivision generates only triangles
        newMesh.faces[f].valence = 3;
    }

    // Split halfedges
    for (int h = 0; h < controlMesh.numHalfEdges(); ++h) {
        HalfEdge* edge = &controlMesh.halfEdges[h];

        int h1 = 3 * h;
        int h2 = 3 * h + 1;
        int h3 = 3 * h + 2;
        int h4 = 3 * controlMesh.numHalfEdges() + h;

        int twinIdx1 = edge->twinIdx() < 0 ? -1 : 3 * edge->twin->next->index + 2;
        int twinIdx2 = 3 * controlMesh.numHalfEdges() + h;
        int twinIdx3 = 3 * edge->prev->twinIdx();
        int twinIdx4 = 3 * h + 1;

        int vertIdx1 = edge->origin->index;
        int vertIdx2 = controlMesh.numVerts() + edge->edgeIndex;
        int vertIdx3 = controlMesh.numVerts() + edge->prev->edgeIndex;
        int vertIdx4 = vertIdx3;

        int edgeIdx1 = 2 * edge->edgeIndex + (h > edge->twinIdx() ? 0 : 1);
        int edgeIdx2 = 2 * controlMesh.numEdges() + h;
        int edgeIdx3 = 2 * edge->prev->edgeIndex +
                       (edge->prevIdx() > edge->prev->twinIdx() ? 1 : 0);
        int edgeIdx4 = 2 * controlMesh.numEdges() + h;

        setHalfEdgeData(newMesh, h1, edgeIdx1, vertIdx1, twinIdx1);
        setHalfEdgeData(newMesh, h2, edgeIdx2, vertIdx2, twinIdx2);
        setHalfEdgeData(newMesh, h3, edgeIdx3, vertIdx3, twinIdx3);
        setHalfEdgeData(newMesh, h4, edgeIdx4, vertIdx4, twinIdx4);
    }
}

/**
 * @brief LoopSubdivider::setHalfEdgeData Sets the data of a single half-edge
 * (and the corresponding vertex and face)
 * @param newMesh The new mesh this half-edge will live in.
 * @param h Index of the half-edge.
 * @param edgeIdx Index of the (undirected) edge this half-edge will belong to.
 * @param vertIdx Index of the vertex that this half-edge will originate from.
 * @param twinIdx Index of the twin of this half-edge. -1 if the half-edge lies
 * on a boundary.
 */
void LoopSubdivider::setHalfEdgeData(Mesh& newMesh, int h, int edgeIdx,
                                     int vertIdx, int twinIdx) const {
    HalfEdge* halfEdge = &newMesh.halfEdges[h];

    halfEdge->edgeIndex = edgeIdx;
    halfEdge->index = h;
    halfEdge->origin = &newMesh.vertices[vertIdx];
    halfEdge->face = &newMesh.faces[halfEdge->faceIdx()];
    halfEdge->next = &newMesh.halfEdges[halfEdge->nextIdx()];
    halfEdge->prev = &newMesh.halfEdges[halfEdge->prevIdx()];
    halfEdge->twin = twinIdx < 0 ? nullptr : &newMesh.halfEdges[twinIdx];

    halfEdge->origin->out = halfEdge;
    halfEdge->origin->index = vertIdx;
    halfEdge->face->side = halfEdge;
}
