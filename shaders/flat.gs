#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vso_position[];
in vec3 vso_color[];

out vec4 gso_color;

uniform vec3 view_position;
uniform vec3 light_position;
uniform mat3 normal_model_view;

vec3 get_post_light_color(vec3 position, vec3 normal, vec3 color) {
    vec3 ambient_color = vec3(1.0f, 1.0f, 1.0f);
    float ambient_amount = 0.7f;
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

void main(void) {
    if (gl_in.length() == 3) {
        vec3 face_position = (vso_position[0] + vso_position[1] + vso_position[2]) / 3.0f;
        vec3 delta_0 = vso_position[1] - vso_position[0];
        vec3 delta_1 = vso_position[2] - vso_position[1];
        vec3 face_normal = normalize(cross(delta_0, delta_1));
        face_normal = normal_model_view * face_normal;
        vec3 face_color = (vso_color[0] + vso_color[1] + vso_color[2]) / 3.0f;
        vec3 post_light_color = get_post_light_color(face_position, face_normal, face_color);
        gso_color = vec4(post_light_color, 1.0f);

        for(int i = 0; i < gl_in.length(); i++) {
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
