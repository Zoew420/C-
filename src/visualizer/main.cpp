#include "my3dpresent.h"
#include "../model/game_model.h"

using namespace Simflow;
const int msize = 100;
int map[msize][msize];

int main() {

    my3d::present::initialize(GetConsoleWindow(), 200, 200, 2);
    GameModel<msize, msize> gm;

    for (int i = 10; i <= 90; i += 2) {
        gm.set_new_particles(ParticleBrush(vec2(40, i), 3, ParticleType::Iron));
        gm.update();
        gm.set_new_particles(ParticleBrush(vec2(60, i), 3, ParticleType::Iron));
        gm.update();
    }

    for (int i = 40; i <= 60; i += 2) {
        gm.set_new_particles(ParticleBrush(vec2(i, 90), 3, ParticleType::Iron));
        gm.update();
    }
    for (int i = 40; i <= 60; i += 2) {
        gm.set_new_particles(ParticleBrush(vec2(i, 88), 3, ParticleType::Iron));
        gm.update();
    }
    for (int i = 40; i <= 60; i += 2) {
        gm.set_new_particles(ParticleBrush(vec2(i, 86), 3, ParticleType::Iron));
        gm.update();
    }

    gm.set_new_particles(ParticleBrush(vec2(50, 10), 10, ParticleType::Water));
    gm.update();
    gm.set_new_particles(ParticleBrush(vec2(50, 20), 10, ParticleType::Water));
    gm.update();
    gm.set_new_particles(ParticleBrush(vec2(50, 30), 10, ParticleType::Water));
    gm.update();
    gm.set_new_particles(ParticleBrush(vec2(50, 40), 10, ParticleType::Water));
    gm.update();
    gm.set_new_particles(ParticleBrush(vec2(50, 50), 10, ParticleType::Water));
    gm.update();
    int i = 0;
    while (1) {
       // Sleep(10);
        gm.update();

        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                my3d::Color cc2 = { 0,0,0 };
                my3d::present::set_pixel(c, r, cc2);
            }
        }
        auto& res = gm.query_particles();
        for (int i = 0; i < res.type.size(); i++) {
            ivec2 pos = f2i(res.position[i]);
            ParticleType type = res.type[i];
            my3d::Color cc2 = { 0,0,0 };
            if (type == ParticleType::Water) {
                cc2[2] = 255;
            }
            else {
                cc2[0] = 255;
            }
            my3d::present::set_pixel(pos.x, pos.y, cc2);
        }

        my3d::present::present();
        i++;
    }
}