#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMatrix4x4>

#include "shadertypes.h"
#include "subdivisionshadertypes.h"

/**
 * Struct that contains all the settings of the program. Initialised with a
 * number of default values.
 */
typedef struct Settings {
  bool modelLoaded = false;
  bool wireframeMode = true;

  float FoV = 80;
  float dispRatio = 16.0f / 9.0f;
  float rotAngle = 0.0f;

  bool uniformUpdateRequired = true;

  ShaderType currentShader = ShaderType::PHONG;
  SubdivisionShaderType currentSubdivShadingAvgMethod = SubdivisionShaderType::LINEAR;

  QMatrix4x4 modelViewMatrix, projectionMatrix;
  QMatrix3x3 normalMatrix;

  bool subdivisionShading = false;
  bool blendNormals = false;
  int isoFrequency;
} Settings;

#endif  // SETTINGS_H
