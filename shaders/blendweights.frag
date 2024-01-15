#version 410
// Fragment shader

layout(location = 0) in vec3 vertcoords_fs;
layout(location = 1) in vec3 vertnormal_fs;
layout(location = 2) in float blendweight_fs;

out vec4 fColor;

void main() {
    fColor = vec4(blendweight_fs, 0.0, (1 - blendweight_fs), 1.0);
}
