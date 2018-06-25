#version 330 core

in vec4 gso_color;

out vec4 fso_color;

void main() {
    fso_color = gso_color;
}
