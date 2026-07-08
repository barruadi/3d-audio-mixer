#version 410 core

out vec4 FragColor;

uniform vec3 uColor;
uniform float uAlpha;
uniform int uIsPoint;

void main()
{
    // gl_PointCoord is only defined for point primitives, so the circular
    // cutout must not run when drawing lines (grid)
    if (uIsPoint == 1)
    {
        vec2 p = gl_PointCoord * 2.0 - 1.0;
        if (dot(p, p) > 1.0) discard;
    }
    FragColor = vec4(uColor, uAlpha);
}
