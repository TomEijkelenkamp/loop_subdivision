#version 410
// Fragment shader

layout(location = 0) in vec3 vertcoords_fs;
layout(location = 1) in vec3 vertnormal_fs;

uniform int isofrequency;

out vec4 fColor;

void main() {
    vec3 random_vector = normalize(vec3(0.0, 1.0, 0.0));
    float dot_product = dot(vertnormal_fs, vec3(0.0, 1.0, 0.0));
    fColor = mod(dot_product * 50, isofrequency) > isofrequency / 2 ? vec4(1.0) : vec4(0.1, 0.1, 0.1, 1.0);
}
