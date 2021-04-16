#version 330

in vec4 vertexPosition;
in vec2 vertexTexCoord;

uniform mat4 mvp;

out vec3 fragTexCoord;

void main() {
    gl_Position = mvp*vec4(vertexPosition);
    fragTexCoord = vec3(vertexTexCoord*gl_Position.z, gl_Position.z);
}