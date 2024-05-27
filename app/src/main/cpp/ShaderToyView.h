//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_SHADERTOYVIEW_H
#define LIVEWALLPAPER05_SHADERTOYVIEW_H


#include "ShaderToyView.h"

class ShaderToyView : public View {
public:

    VertexArrayObject environmentTriangleVAO;

    SphereMap sphereMap;

    GLuint sphereMapProgram;

    const string SPHERE_MAP_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    // Found at https://www.shadertoy.com/view/XsfXDr
    /*const string SPHERE_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "uniform vec2 iResolution; // Pass the resolution of your rendering surface\n"
            "uniform float iTime;      // Pass the elapsed time since the start of the application\n"
            "uniform vec2 iMouse;      // Pass the current mouse position\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main(){\n"
            "   float ratio = iResolution.x / iResolution.y;\n"
            "   vec2 uv = vec2(ratio, 1.0f) * (2.0f * gl_FragCoord.xy / iResolution.xy - 1.0f);\n"
            "   \n"
            "   vec3 n = vec3(uv, sqrt(1.0f - clamp(dot(uv, uv), 0.0f, 1.0f)));\n"
            "   \n"
            "   vec3 color = 0.5f + 0.5f * n;\n"
            "   color = mix(vec3(0.5f), color, smoothstep(1.01f, 1.0f, dot(uv, uv)));\n"
            "   outColor = vec4(color, 1.0f);\n"
            "}\n";*/

    // Found at https://www.shadertoy.com/view/tsBXW3
    /*const string SPHERE_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "const int AA = 2;               //change to 1 to increase performance\n"
            "const float _Speed = 3.0f;      //disk rotation speed\n"
            "const float _Steps = 12.0f;     //disk texture layers\n"
            "const float _Size = 0.3f;       //size of BH\n"
            "uniform sampler2D iChannel0;\n"
            "uniform vec2 iResolution; // Pass the resolution of your rendering surface\n"
            "uniform float iTime;      // Pass the elapsed time since the start of the application\n"
            "uniform vec2 iMouse;      // Pass the current mouse position\n"
            "out vec4 outColor;\n"
            "\n"
            "float hash(float x) { return fract(sin(x) * 152754.742); }\n"
            "float hash(vec2 x) { return hash(x.x + hash(x.y)); }\n"
            "\n"
            "float value(vec2 p, float f) {\n"
            "    float bl = hash(floor(p * f + vec2(0., 0.)));\n"
            "    float br = hash(floor(p * f + vec2(1., 0.)));\n"
            "    float tl = hash(floor(p * f + vec2(0., 1.)));\n"
            "    float tr = hash(floor(p * f + vec2(1., 1.)));\n"
            "\n"
            "    vec2 fr = fract(p * f);\n"
            "    fr = (3. - 2. * fr) * fr * fr;\n"
            "    float b = mix(bl, br, fr.x);\n"
            "    float t = mix(tl, tr, fr.x);\n"
            "    return mix(b, t, fr.y);\n"
            "}\n"
            "\n"
            "vec4 background(vec3 ray) {\n"
            "    vec2 uv = ray.xy;\n"
            "    if (abs(ray.x) > 0.5)\n"
            "        uv.x = ray.z;\n"
            "    else if (abs(ray.y) > 0.5)\n"
            "        uv.y = ray.z;\n"
            "    float brightness = value(uv * 3., 100.);\n"
            "    float color = value(uv * 2., 20.);\n"
            "    brightness = pow(brightness, 256.);\n"
            "    brightness = brightness * 100.;\n"
            "    brightness = clamp(brightness, 0., 1.);\n"
            "    vec3 stars = brightness * mix(vec3(1., .6, .2), vec3(.2, .6, 1), color);\n"
            "    vec4 nebulae = texture(iChannel0, (uv * 1.5));\n"
            "    nebulae.xyz += nebulae.xxx + nebulae.yyy + nebulae.zzz;\n"
            "    nebulae.xyz *= 0.25;\n"
            "    nebulae *= nebulae;\n"
            "    nebulae *= nebulae;\n"
            "    nebulae *= nebulae;\n"
            "    nebulae *= nebulae;\n"
            "    nebulae.xyz += stars;\n"
            "    return nebulae;\n"
            "}\n"
            "\n"
            "vec4 raymarchDisk(vec3 ray, vec3 zeroPos) {\n"
            "    vec3 position = zeroPos;\n"
            "    float lengthPos = length(position.xz);\n"
            "    float dist = min(1., lengthPos * (1. / _Size) * 0.5) * _Size * 0.4 * (1. / _Steps) / (abs(ray.y));\n"
            "    position += dist * _Steps * ray * 0.5;\n"
            "    vec2 deltaPos;\n"
            "    deltaPos.x = -zeroPos.z * 0.01 + zeroPos.x;\n"
            "    deltaPos.y = zeroPos.x * 0.01 + zeroPos.z;\n"
            "    deltaPos = normalize(deltaPos - zeroPos.xz);\n"
            "    float parallel = dot(ray.xz, deltaPos);\n"
            "    parallel /= sqrt(lengthPos);\n"
            "    parallel *= 0.5;\n"
            "    float redShift = parallel + 0.3;\n"
            "    redShift *= redShift;\n"
            "    redShift = clamp(redShift, 0., 1.);\n"
            "    float disMix = clamp((lengthPos - _Size * 2.) * (1. / _Size) * 0.24, 0., 1.);\n"
            "    vec3 insideCol = mix(vec3(1.0, 0.8, 0.0), vec3(0.5, 0.13, 0.02) * 0.2, disMix);\n"
            "    insideCol *= mix(vec3(0.4, 0.2, 0.1), vec3(1.6, 2.4, 4.0), redShift);\n"
            "    insideCol *= 1.25;\n"
            "    redShift += 0.12;\n"
            "    redShift *= redShift;\n"
            "    vec4 o = vec4(0.);\n"
            "    for (float i = 0.; i < _Steps; i++) {\n"
            "        position -= dist * ray;\n"
            "        float intensity = clamp(1. - abs((i - 0.8) * (1. / _Steps) * 2.), 0., 1.);\n"
            "        float lengthPos = length(position.xz);\n"
            "        float distMult = 1.;\n"
            "        distMult *= clamp((lengthPos - _Size * 0.75) * (1. / _Size) * 1.5, 0., 1.);\n"
            "        distMult *= clamp((_Size * 10. - lengthPos) * (1. / _Size) * 0.20, 0., 1.);\n"
            "        distMult *= distMult;\n"
            "        float u = lengthPos + iTime * _Size * 0.3 + intensity * _Size * 0.2;\n"
            "        vec2 xy;\n"
            "        float rot = mod(iTime * _Speed, 8192.);\n"
            "        xy.x = -position.z * sin(rot) + position.x * cos(rot);\n"
            "        xy.y = position.x * sin(rot) + position.z * cos(rot);\n"
            "        float x = abs(xy.x / (xy.y));\n"
            "        float angle = 0.02 * atan(x);\n"
            "        const float f = 70.;\n"
            "        float noise = value(vec2(angle, u * (1. / _Size) * 0.05), f);\n"
            "        noise = noise * 0.66 + 0.33 * value(vec2(angle, u * (1. / _Size) * 0.05), f * 2.);\n"
            "        float extraWidth = noise * 1. * (1. - clamp(i * (1. / _Steps) * 2. - 1., 0., 1.));\n"
            "        float alpha = clamp(noise * (intensity + extraWidth) * ((1. / _Size) * 10. + 0.01) * dist * distMult, 0., 1.);\n"
            "        vec3 col = 2. * mix(vec3(0.3, 0.2, 0.15) * insideCol, insideCol, min(1., intensity * 2.));\n"
            "        o = clamp(vec4(col * alpha + o.rgb * (1. - alpha), o.a * (1. - alpha) + alpha), vec4(0.), vec4(1.));\n"
            "        lengthPos *= (1. / _Size);\n"
            "        o.rgb += redShift * (intensity * 1. + 0.5) * (1. / _Steps) * 100. * distMult / (lengthPos * lengthPos);\n"
            "    }\n"
            "    o.rgb = clamp(o.rgb - 0.005, 0., 1.);\n"
            "    return o;\n"
            "}\n"
            "\n"
            "void Rotate(inout vec3 vector, vec2 angle) {\n"
            "    vector.yz = cos(angle.y) * vector.yz + sin(angle.y) * vec2(-1, 1) * vector.zy;\n"
            "    vector.xz = cos(angle.x) * vector.xz + sin(angle.x) * vec2(-1, 1) * vector.zx;\n"
            "}\n"
            "\n"
            "void main() {\n"
            "    outColor = vec4(0.);\n"
            "    vec2 fragCoordRot;\n"
            "    fragCoordRot.x = gl_FragCoord.x * 0.985 + gl_FragCoord.y * 0.174;\n"
            "    fragCoordRot.y = gl_FragCoord.y * 0.985 - gl_FragCoord.x * 0.174;\n"
            "    fragCoordRot += vec2(-0.06, 0.12) * iResolution.xy;\n"
            "    for (int j = 0; j < AA; j++) {\n"
            "        for (int i = 0; i < AA; i++) {\n"
            "            vec3 ray = normalize(vec3((fragCoordRot - iResolution.xy * .5 + vec2(i, j) / float(AA)) / iResolution.x, 1));\n"
            "            vec3 pos = vec3(0., 0.05, -(20. * iMouse.xy / iResolution.y - 10.) * (20. * iMouse.xy / iResolution.y - 10.) * .05);\n"
            "            vec2 angle = vec2(iTime * 0.1, .2);\n"
            "            angle.y = (2. * iMouse.y / iResolution.y) * 3.14 + 0.1 + 3.14;\n"
            "            float dist = length(pos);\n"
            "            Rotate(pos, angle);\n"
            "            angle.xy -= min(.3 / dist, 3.14) * vec2(1, 0.5);\n"
            "            Rotate(ray, angle);\n"
            "            vec4 col = vec4(0.);\n"
            "            vec4 glow = vec4(0.);\n"
            "            vec4 outCol = vec4(100.);\n"
            "            for (int disks = 0; disks < 20; disks++) {\n"
            "                for (int h = 0; h < 6; h++) {\n"
            "                    float dotpos = dot(pos, pos);\n"
            "                    float invDist = inversesqrt(dotpos);\n"
            "                    float centDist = dotpos * invDist;\n"
            "                    float stepDist = 0.92 * abs(pos.y / (ray.y));\n"
            "                    float farLimit = centDist * 0.5;\n"
            "                    float closeLimit = centDist * 0.1 + 0.05 * centDist * centDist * (1. / _Size);\n"
            "                    stepDist = min(stepDist, min(farLimit, closeLimit));\n"
            "                    float invDistSqr = invDist * invDist;\n"
            "                    float bendForce = stepDist * invDistSqr * _Size * 0.625;\n"
            "                    ray = normalize(ray - (bendForce * invDist) * pos);\n"
            "                    pos += stepDist * ray;\n"
            "                    glow += vec4(1.2, 1.1, 1, 1.0) * (0.01 * stepDist * invDistSqr * invDistSqr * clamp(centDist * (2.) - 1.2, 0., 1.));\n"
            "                }\n"
            "                float dist2 = length(pos);\n"
            "                if (dist2 < _Size * 0.1)\n"
            "                    outCol = vec4(col.rgb * col.a + glow.rgb * (1. - col.a), 1.);\n"
            "                else if (dist2 > _Size * 1000.) {\n"
            "                    vec4 bg = background(ray);\n"
            "                    outCol = vec4(col.rgb * col.a + bg.rgb * (1. - col.a) + glow.rgb * (1. - col.a), 1.);\n"
            "                }\n"
            "                else if (abs(pos.y) <= _Size * 0.002) {\n"
            "                    vec4 diskCol = raymarchDisk(ray, pos);\n"
            "                    pos.y = 0.;\n"
            "                    pos += abs(_Size * 0.001 / ray.y) * ray;\n"
            "                    col = vec4(diskCol.rgb * (1. - col.a) + col.rgb, col.a + diskCol.a * (1. - col.a));\n"
            "                }\n"
            "            }\n"
            "            if (outCol.r == 100.)\n"
            "                outCol = vec4(col.rgb + glow.rgb * (col.a + glow.a), 1.);\n"
            "            col = outCol;\n"
            "            col.rgb = pow(col.rgb, vec3(0.6));\n"
            "            outColor += col / float(AA * AA);\n"
            "        }\n"
            "    }\n"
            "}\n";*/

    // Found at https://www.shadertoy.com/view/XsfXDr
    const string SPHERE_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "uniform vec2 iResolution;     // Pass the resolution of your rendering surface\n"
            "uniform float iTime;          // Pass the elapsed time since the start of the application\n"
            "uniform vec2 iMouse;          // Pass the current mouse position\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            "void main(){\n"
            "   outColor = Texture(environmentTexture, direction);\n"
            "}\n";

    ShaderToyView();

    ~ShaderToyView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SHADERTOYVIEW_H