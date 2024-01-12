#version 410
// Fragment shader

layout(location = 0) in vec3 vertcoords_fs;
layout(location = 1) in vec3 vertnormal_fs;

out vec4 fColor;

void main() {
    fColor = vec4(vertnormal_fs * 0.5 + 0.5, 1.0); // Adjusting from [-1, 1] to [0, 1]
}
