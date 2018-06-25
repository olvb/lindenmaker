#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

out vec3 vso_position;
out vec3 vso_normal;
out vec3 vso_color;

uniform mat4 model_view;
uniform mat3 normal_model_view;
uniform mat4 projection;

void main() {
    vec4 model_view_position = model_view * vec4(in_position, 1.0f);
    gl_Position = projection * model_view_position;
    vso_position = vec3(model_view_position);
    vso_normal = normal_model_view * in_normal;
    vso_color = in_color;
}
