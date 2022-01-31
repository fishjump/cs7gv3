#version 330 core
layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 texture_coordinate_in;

out vec3 frag_pos;
out vec3 normal;
out vec2 texture_coordinate;

uniform mat4 model_uni;
uniform mat4 view_uni;
uniform mat4 projection_uni;

void main() {
    texture_coordinate = texture_coordinate_in;
    frag_pos = vec3(model_uni * vec4(pos_in, 1.0));
    normal = mat3(transpose(inverse(model_uni))) * normal_in;

    gl_Position = projection_uni * view_uni * vec4(frag_pos, 1.0);
}