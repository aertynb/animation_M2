#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec4 aTangent; // Tangent + Handedness in .w

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;
out vec2 vTexCoords;
out vec3 vTangent;
out mat3 vTBN;

uniform mat4 uModelMatrix;
uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix; // Using mat4 instead of mat3

void main()
{
    vViewSpacePosition = vec3(uModelViewMatrix * vec4(aPosition, 1.0));
    vViewSpaceNormal = normalize(vec3(uNormalMatrix * vec4(aNormal, 0.0)));
    vTexCoords = aTexCoords;
    vTangent = aTangent.xyz;
    vec3 T = vec3(uNormalMatrix * vec4(aTangent.xyz, 0.0));
    vec3 N = vViewSpaceNormal;
    vec3 B = cross(N, T) * aTangent.w;
    vTBN = mat3(normalize(T), normalize(B), normalize(N));
    gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
}
