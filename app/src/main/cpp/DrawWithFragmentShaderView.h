//
// Created by Immanuel Bierer on 12/17/2023.
//

#ifndef LIVEWALLPAPER05_DRAWWITHFRAGMENTSHADERVIEW_H
#define LIVEWALLPAPER05_DRAWWITHFRAGMENTSHADERVIEW_H


#include "View.h"
#include "Texture.h"
#include "FBO.h"

class DrawWithFragmentShaderView : public View {
public:

    GLuint mPlanesProgram;

    Texture texture;

    FBO fbo;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D image;\n"
            "uniform int WIDTH;\n"
            "uniform int HEIGHT;\n"
            "out vec4 outColor;\n"
            "\n"
            "vec3 fetchFromSpectrum(float value) {\n"
            "    float index = value - floor(value);\n"
            "    if(index < 1.0f / 6.0f){\n"
            "        return vec3(\n"
            "                1.0f,\n"
            "                6.0f * index,\n"
            "                0.0f);\n"
            "    }else if(index < 2.0f / 6.0f){\n"
            "        return vec3(\n"
            "                2.0f - 6.0f * index,\n"
            "                1.0f,\n"
            "                0.0f);\n"
            "    }else if(index < 3.0f / 6.0f){\n"
            "        return vec3(\n"
            "                0.0f,\n"
            "                1.0f,\n"
            "                6.0f * index - 2.0f);\n"
            "    }else if(index < 4.0f / 6.0f){\n"
            "        return vec3(\n"
            "                0.0f,\n"
            "                4.0f - 6.0f * index,\n"
            "                1.0f);\n"
            "    }else if(index < 5.0f / 6.0f){\n"
            "        return vec3(\n"
            "                6.0f * index - 4.0f,\n"
            "                0.0f,\n"
            "                1.0f);\n"
            "    }else{\n"
            "        return vec3(\n"
            "                1.0f,\n"
            "                0.0f,\n"
            "                6.0f * (1.0f - index));\n"
            "    }\n"
            "}\n"
            "void main() {\n"
            "   // Maximum number of iterations for Mandelbrot algorithm\n"
            "   const int MAX_ITERATIONS = 1536;\n"
            "   float width = float(WIDTH);\n"
            "   float height = float(HEIGHT);\n"
            "   \n"
            "   float y = gl_FragCoord.y;\n"
            "   float x = gl_FragCoord.x;\n"
            "   // Map pixel coordinates to the complex plane\n"
            "   float minX = -2.0;\n"
            "   float maxX = 2.0;\n"
            "   float minY = -1.5;\n"
            "   float maxY = 1.5;\n"
            "   float cr = minX + x * (maxX - minX) / (width - 1.0f);\n"
            "   float ci = minY + y * (maxY - minY) / (height - 1.0f);\n"
            "   float zr = 0.0f;\n"
            "   float zi = 0.0f;\n"
            "   \n"
            "   int iterations = 0;\n"
            "   \n"
            "   // Iterate to check if the point is in the Mandelbrot set\n"
            "   while (iterations < MAX_ITERATIONS && (zr * zr + zi * zi) < 4.0f) {\n"
            "       float temp = zr * zr - zi * zi + cr;\n"
            "       zi = 2.0 * zr * zi + ci;\n"
            "       zr = temp;\n"
            "       \n"
            "       iterations++;\n"
            "   }\n"
            "   \n"
            "   if(iterations == MAX_ITERATIONS){\n"
            "       outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
            "   }else{\n"
            "       outColor = vec4(fetchFromSpectrum(0.005f * float(iterations)), 1.0f);\n"
            "   }\n"
            "}\n";

    const string PLANES_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "out vec3 position;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    position = pos;\n"
            "}\n";

    const string PLANES_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D image;\n"
            "in vec3 position;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(image, position.xy); \n"
            "}\n";

    DrawWithFragmentShaderView();

    ~DrawWithFragmentShaderView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_DRAWWITHFRAGMENTSHADERVIEW_H
