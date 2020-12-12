precision mediump float;
varying vec2 v_texPosition;
uniform sampler2D sampler_y;
uniform sampler2D sampler_u;
uniform sampler2D sampler_v;
void main() {
    float y,u,v;
    y = texture2D(sampler_y,v_texPosition).r;
    // 因为UV的默认值是127，所以我们这里要减去0.5（OpenGLES的Shader中会把内存中0～255的整数数值换算为0.0～1.0的浮点数值）
    u = texture2D(sampler_u,v_texPosition).r- 0.5;
    v = texture2D(sampler_v,v_texPosition).r- 0.5;

    vec3 rgb;
    rgb.r = y + 1.403 * v;
    rgb.g = y - 0.344 * u - 0.714 * v;
    rgb.b = y + 1.770 * u;

    gl_FragColor = vec4(rgb,1);
}
