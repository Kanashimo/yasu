#version 330

uniform vec2 uScreenResolution;
uniform float uStrength;

in vec2 vPixelPosition;
in vec2 vMaxPoint;
in vec2 vMinPoint;
in vec2 vPadding;

out vec4 FragColor;

void main ()
{
    // TODO: use vPadding, optimize everything
    vec2 closest = clamp(vPixelPosition, vMinPoint, vMaxPoint);
    vec2 delta = vPixelPosition - closest;

    delta.x *= uScreenResolution.x / 1.5f;
    delta.y *= uScreenResolution.y;

    float d = length(delta);

    if (d <= 0.0f) discard;

    // float glow = uStrength * exp(-d * 0.075f);
    // float glow = uStrength * exp(-d * 0.075f) * exp(-2.4f * max(0, d - 22.5));
    float glow = uStrength * exp(-d * 0.075f) * exp(-10.0f * max(0, d - 22.5));

    FragColor = vec4(0.35f, 0.8f, 1.0f, 0.5f) * glow;
}
