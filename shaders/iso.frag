#version 410
// Fragment shader

layout(location = 0) in vec3 vertcoords_fs;
layout(location = 1) in vec3 vertnormal_fs;

uniform int isofrequency;
uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

out vec4 fColor;

const vec4 color_1 = vec4(1.0);
const vec4 color_2 = vec4(0.1, 0.1, 0.1, 1.0);

void main() {
    vec3 random_vector = vec3(0.0, 1.0, 0.0);
    float dot_product = dot(vertnormal_fs, random_vector);

    fColor = mod(dot_product * 50, isofrequency*2) > isofrequency ? color_1 : color_2;
}
