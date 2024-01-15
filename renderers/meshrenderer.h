#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <QOpenGLShaderProgram>

#include "../mesh/mesh.h"
#include "renderer.h"

/**
 * @brief The MeshRenderer class is responsible for rendering a mesh. Only
 * renders triangle meshes.
 */
class MeshRenderer : public Renderer {
 public:
  MeshRenderer();
  ~MeshRenderer() override;

  void updateUniforms();
  void updateBuffers(Mesh& m);
  void draw();

 protected:
  void initShaders() override;
  void initBuffers() override;

 private:
  GLuint vao;
  GLuint meshCoordsBO, meshNormalsBO, meshBlendWeightsBO, meshIndexBO;
  int meshIBOSize;

  // Uniforms
  GLint uniModelViewMatrix, uniProjectionMatrix, uniNormalMatrix, uniIsoFrequency;
};

#endif  // MESHRENDERER_H
