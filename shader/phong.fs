#version 330 core

struct material_t {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float shininess;
};

struct light_t {
    vec3 position;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

in vec3 frag_pos;
in vec3 normal;
in vec2 texture_coordinate;

out vec4 frag_color;

uniform vec3 view_pos;
uniform material_t material;
uniform light_t light;
uniform sampler2D texture_diffuse1;

void main() {
    // ambient
    vec3 ambient = light.ambient_color * material.ambient_color;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light.diffuse_color * material.diffuse_color;

    // specular
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = spec * light.specular_color * material.specular_color;

    vec3 result = (ambient + diffuse + specular) * texture(texture_diffuse1, texture_coordinate).xyz;
    frag_color = vec4(result, 1.0);
}