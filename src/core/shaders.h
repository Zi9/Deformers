#pragma once

static const char* affine_fs = "#version 330\n"
                               "in vec3 fragTexCoord;\n"
                               "uniform sampler2D texture0;\n"
                               "out vec4 finalColor;\n"
                               "void main() {\n"
                               "finalColor = texture(texture0, fragTexCoord.xy/fragTexCoord.z);\n"
                               "}\n";
static const char* affine_vs = "#version 330\n"
                               "in vec4 vertexPosition;\n"
                               "in vec2 vertexTexCoord;\n"
                               "uniform mat4 mvp;\n"
                               "out vec3 fragTexCoord;\n"
                               "void main() {\n"
                               "gl_Position = mvp*vec4(vertexPosition);\n"
                               "fragTexCoord = vec3(vertexTexCoord*gl_Position.z, gl_Position.z);\n"
                               "}\n";
