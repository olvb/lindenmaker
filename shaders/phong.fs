#version 330 core

in vec3 vso_position;
in vec3 vso_normal;
in vec3 vso_color;

out vec4 fso_color;

uniform vec3 view_position;
uniform vec3 light_position;

vec3 get_post_light_color(vec3 position, vec3 normal, vec3 color) {
    vec3 ambient_color = vec3(1.0f, 1.0f, 1.0f);
    float ambient_amount = 0.1f;
    vec3 ambient = ambient_amount * ambient_color;

    vec3 diffuse_color = vec3(1.0f, 1.0f, 1.0f);
    vec3 light_direction = normalize(light_position - position);
    float diffuse_strength = max(dot(normalize(normal), -light_direction), 0.0f);
    vec3 diffuse = diffuse_strength * diffuse_color;

    vec3 specular_color = vec3(1.0f, 1.0f, 1.0f);
    vec3 view_direction = normalize(view_position - position);
    vec3 reflect_direction = reflect(light_direction, normal);
    float specular_strength = 0.2 * pow(clamp(dot(reflect_direction, -view_direction), 0.0f, 1.0f), 10.0f);
    vec3 specular = specular_strength * specular_color;

    return (ambient + diffuse + specular) * color;
}

void main() {
    vec3 post_light_color = get_post_light_color(vso_position, vso_normal, vso_color);
    fso_color = vec4(post_light_color, 1.0f);
}
