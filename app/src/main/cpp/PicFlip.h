//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_PICFLIP_H
#define LIVEWALLPAPER05_PICFLIP_H


class PicFlip : public Simulation{
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
    "uniform mat4 mvp;\n"
    "out vec4 vColor;\n"
    "void main() {\n"
    "    gl_Position = mvp * vec4(pos, 1.0);\n"
    "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    void render() override;

    PicFlip();

    ~PicFlip();

public:

    static void simulate(const vec3& acceleration){
        fluid->simulate(
                fluid->dt, acceleration, fluid->flipRatio, fluid->numPressureIters, fluid->numParticleIters,
                fluid->overRelaxation, fluid->compensateDrift, fluid->separateParticles);
        fluid->frameNr++;
    }

    static void setupScene(){
        int res = 15;

        float tankHeight = 1.0f * simHeight;
        float tankWidth = 1.0f * simWidth;
        float tankDepth = 1.0f * simDepth;
        float h = tankHeight / res;
        float density = 1000.0f;

        float relWaterHeight = 1.0f;
        float relWaterWidth = 0.6f;
        float relWaterDepth = 1.0f;

        // dam break

        // compute number of particles

        float r = 0.3 * h;    // particle radius w.r.t. cell size
        float dx = 2.0 * r;
        float dy = sqrt(3.0) / 2.0 * dx;
        float dz = 2.0 * r;

        int numX = floor((relWaterWidth * tankWidth - 2.0 * h - 2.0 * r) / dx);
        int numY = floor((relWaterHeight * tankHeight - 2.0 * h - 2.0 * r) / dy);
        int numZ = floor((relWaterDepth * tankDepth - 2.0 * h - 2.0 * r) / dz);
        int maxParticles = numX * numY * numZ;

        // create fluid

        fluid = new FlipFluid(density, tankWidth, tankHeight, tankDepth, h, r, maxParticles);

        // create particles

        fluid->numParticles = numX * numY * numZ;
        int p = 0;
        for (int i = 0; i < numX; i++) {
            for (int j = 0; j < numY; j++) {
                for (int k = 0; k < numZ; k++) {
                    fluid->particlePos[p++] = vec3(
                            h + r + dx * i + (j % 2 == 0 ? 0.0 : r),
                            h + r + dy * j,
                            h + dz * k
                    );
                }
            }
        }

        // setup grid cells for tank

        int nx = fluid->fNumX;
        int ny = fluid->fNumY;
        int nz = fluid->fNumZ;

        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    float s = 1.0;    // fluid
                    if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1 || k == 0 || k == nz - 1)
                        s = 0.0;    // solid
                    fluid->s[(i * ny + j) * nz + k] = s;
                }
            }
        }
    }

private:
};


#endif //LIVEWALLPAPER05_PICFLIP_H
