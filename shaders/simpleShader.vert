#version 450

layout(location = 0) in vec3 position;  // Changed from vec2 to vec3
layout(location = 1) in vec3 color;

layout(push_constant) uniform Push {
  mat4 transform;
  vec3 color;
} push;

layout(location = 0) out vec3 fragColor;

void main() {
  gl_Position = push.transform * vec4(position, 1.0);  // Use 1.0 for w component
  fragColor = color;
}