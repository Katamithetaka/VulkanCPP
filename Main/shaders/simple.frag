#version 450

layout(location = 0) out vec4 outColor;
layout (location = 0) in vec2 fragTexCoord;

float thickness = 1.0;
float fade = 0.005;

vec3 color0 = vec3(1.0, 0.81, 0.86);
vec3 color1 = vec3(0.68, 0.77, 0.81);



void main() {

    outColor = vec4(mix(color0, color1, fragTexCoord.x + fragTexCoord.y - 2 * fragTexCoord.x * fragTexCoord.y), 1.0);
    vec2 uv = fragTexCoord.xy * 2.0 - 1.0;
    float distance = 1 - length(uv);
    float circleAlpha = smoothstep(0.0, fade, distance);
    circleAlpha *= smoothstep(thickness + fade, thickness, distance);
    outColor.a = circleAlpha;

}