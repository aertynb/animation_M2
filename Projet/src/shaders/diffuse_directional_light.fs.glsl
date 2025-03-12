#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

uniform vec3 uLightDirection;
uniform vec3 uLightIntensity;

out vec3 fColor;

void main()
{
  // Need another normalization because interpolation of vertex attributes does
  // not maintain unit length
  vec3 viewSpaceNormal = normalize(vViewSpaceNormal);
  vec3 viewDirection = -vViewSpacePosition;
  float theta = dot(vViewSpaceNormal, normalize(uLightDirection));
  fColor = vec3(1 / 3.14, 1 / 3.14, 1 / 3.14) * uLightIntensity * cos(theta);
}