#version 330

in vec3 fragTexCoord;

uniform sampler2D texture0;

out vec4 finalColor;

void main() {
    finalColor = texture(texture0, fragTexCoord.xy/fragTexCoord.z);
}