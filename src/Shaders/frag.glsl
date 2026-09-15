#version 330

in vec2 vPixelPosition;
in vec2 vMaxPoint;
in vec2 vMinPoint;
in vec2 vPadding;

out vec4 FragColor;

void main ()
{
    // TODO: polish it and use vPadding
    vec2 closest = clamp(vPixelPosition, vMinPoint, vMaxPoint);
    float d = distance(vPixelPosition, closest);

    if (d <= 0.0f) discard;

    float glow = exp(-d * 200.0f);

    FragColor = vec4(0.35f, 0.8f, 1.0f, 0.5f) * glow;
}
