#include "my3dpresent.h"
#include "../model/game_model.h"

using namespace T;
const int msize = 100;
int map[msize][msize];

int main() {

    my3d::present::initialize(GetConsoleWindow(), 200, 200, 2);
    GameModel gm(msize, msize);

    for (int i = 10; i <= 90; i += 2) {
        gm.set_new_particles(ParticleBrush(vec2(i, 70), 3, ParticleType::Iron));
        gm.update();
    }

    gm.set_new_particles(ParticleBrush(vec2(10, 65), 5, ParticleType::Iron));
    gm.update();        
    gm.set_new_particles(ParticleBrush(vec2(10, 60), 5, ParticleType::Iron));
    gm.update();
    //gm.set_new_particles(ParticleBrush(vec2(10, 55), 5, ParticleType::Iron));
    gm.update();

    gm.set_new_particles(ParticleBrush(vec2(90, 65), 5, ParticleType::Iron));
    gm.update();
    gm.set_new_particles(ParticleBrush(vec2(90, 60), 5, ParticleType::Iron));
    gm.update();
    //gm.set_new_particles(ParticleBrush(vec2(90, 55), 5, ParticleType::Iron));
    gm.update();

    gm.set_new_particles(ParticleBrush(vec2(50, 50), 20, ParticleType::Water));

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