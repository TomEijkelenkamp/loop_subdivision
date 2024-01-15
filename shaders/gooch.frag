#version 410
// Fragment shader

layout(location = 0) in vec3 vertcoords_fs;
layout(location = 1) in vec3 vertnormal_fs;

out vec4 fColor;

const vec3 lightPos = vec3(3.0, 0.0, 2.0);
const vec3 baseColour = vec3(1.0, 1.0, 1.0);
const vec3 lightBlue = vec3(0.0, 0.0, 1.0);
const vec3 lightYellow = vec3(1.0, 0.65, 0.0);
const vec3 camerapos = vec3(0.0);

const float a = 0.85;
const float alpha = 0.15;
const float b = 0.85;
const float beta = 0.15;

// Gooch shading, based on the 1998 paper "A Non-Photorealistic Lighting Model
// for Automatic Technical Illustration".
vec3 goochShading(vec3 colourCool, vec3 colourWarm, vec3 coords, vec3 normal) {
  vec3 surfToLight = normalize(lightPos - coords);
  vec3 surfToCamera = normalize(camerapos - coords);

  if (!gl_FrontFacing) {
    // Make the inside a darker shade.
    normal *= -1;
    colourCool *= 0.4;
    colourWarm *= 0.4;
  }

  float diffuse = dot(normal, surfToLight) * 0.5 + 0.5;
  return mix(colourCool, colourWarm, diffuse);

  // float coeff = dot(normal, surfToLight);
  // float t = (coeff + 1) / 2;
  // float specular = pow((dot(normal, surfToCamera) + 1) / 2, 2);

  // // return (1 + coeff) / 2.0 * colourWarm + (1 - coeff / 2.0) * colourCool;
  // return specular + (1 - specular) * t * colourWarm + (1 - t) * colourCool;
}

void main() {
  vec3 colourCool = a * lightBlue + alpha * baseColour;
  vec3 colourWarm = b * lightYellow + beta * baseColour;

  vec3 col = goochShading(colourCool, colourWarm, vertcoords_fs, vertnormal_fs);
  fColor = vec4(col, 1.0);
}
