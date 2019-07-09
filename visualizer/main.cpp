#include "my3dpresent.h"
#include "../model/game_model.h"

using namespace T;
const int msize = 60;
int map[msize][msize];


int main() {
    my3d::present::initialize(GetConsoleWindow(), 120, 120, 3);
    GameModel gm(msize, msize);
    gm.set_new_particles(ParticleBrush(vec2(40, 20), 4, ParticleType::Sand));
    while (1) {
       // getchar();
        gm.update();
        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                int pos = gm.idx(r, c);
                float p = gm.airstate.p[pos];
                float pc = p / 100000  + 100;
                my3d::Color cc = { pc,pc,pc };
                my3d::present::set_pixel(c, r, cc);
                if (!gm.state_cur.map_index[pos].nil()) {
                    my3d::Color cc2 = { 255,0,0 };
                    my3d::present::set_pixel(c, r, cc2);
                }
            }
        }
        my3d::present::present();
    }
}