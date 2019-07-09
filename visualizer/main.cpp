#include "my3dpresent.h"
#include "../model/game_model.h"

using namespace T;
const int msize = 60;
int map[msize][msize];


int main() {
    my3d::present::initialize(GetConsoleWindow(), 120, 120, 3);
    GameModel gm(msize, msize);
    gm.set_new_particles(ParticleBrush(vec2(10, 10), 4, ParticleType::Sand));
    while (1) {
        getchar();
        gm.update();
        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                int pos = gm.idx(r, c);
                float p = gm.airstate.p[pos];
                float pc = p  + 100;
                my3d::Color cc = { pc,pc,pc };
                my3d::present::set_pixel(c, r, cc);
            }
        }
        my3d::present::present();
    }
}