#version 330 core
#define PADDING_VALUE 250.0f

uniform vec2 uScreenResolution;
uniform vec2 uStartPoint;
uniform vec2 uEndPoint;

out vec2 vPixelPosition;
out vec2 vMinPoint;
out vec2 vMaxPoint;
out vec2 vPadding;

const vec2 vertices[6] = vec2[6](
    vec2(0.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f),
    vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f)
);

void main()
{
    vec2 startPoint = uStartPoint / uScreenResolution;
    vec2 endPoint = uEndPoint / uScreenResolution;
    vec2 padding = PADDING_VALUE / uScreenResolution;

    vec2 minPoint = min(startPoint, endPoint);
    vec2 maxPoint = max(startPoint, endPoint);

    vMinPoint = minPoint;
    vMaxPoint = maxPoint;
    vPadding = padding;

    minPoint = minPoint - padding;
    maxPoint = maxPoint + padding;

    vec2 scale = maxPoint - minPoint;
    vec2 pos = vertices[gl_VertexID] * scale + minPoint;

    vPixelPosition = pos;
    pos = pos * 2.0f - vec2(1.0f);

    gl_Position = vec4(pos.x, -pos.y, 0.0f, 1.0f);
}
