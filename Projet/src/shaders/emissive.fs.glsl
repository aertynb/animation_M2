#version 330 core

// Input texture coordinates from the vertex shader.
in vec2 vTexCoords;

// Uniforms for emissive.
uniform sampler2D uEmissiveTexture;
uniform vec3 uEmissiveFactor;

// Output final fragment color.
out vec4 fColor;

void main() {
    // Sample the emissive texture (assumed in linear space) and apply emissive factor.
    vec3 emissive = texture(uEmissiveTexture, vTexCoords).rgb * uEmissiveFactor;
    
    // Convert the emissive color to grayscale using luminance weights.
    float gray = dot(emissive, vec3(0.299, 0.587, 0.114));
    
    // Output grayscale color.
    fColor = vec4(vec3(gray), 1.0);
}