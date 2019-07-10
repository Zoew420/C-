#include "my3dpresent.h"
#include "../model/game_model.h"

using namespace T;
const int msize = 100;
int map[msize][msize];

int main() {

    my3d::present::initialize(GetConsoleWindow(), 200, 200, 2);
    GameModel gm(msize, msize);


    int iii = 0;

    while (1) {
        getchar();

        if (iii == 0) gm.set_new_particles(ParticleBrush(vec2(40, 60), .5, ParticleType::Iron));
        if (iii == 1) gm.set_new_particles(ParticleBrush(vec2(40, 40), .5, ParticleType::Sand));

        gm.update();

        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                int pos = gm.idx(r, c);
                float p = gm.airflow_solver.p[gm.idx_air(r, c)];
                float pc = p * 50 + 100;

                my3d::Color cc = { pc,pc,pc };
                my3d::present::set_pixel(r, c, cc);
                if (!gm.state_cur.map_index[pos].nil()) {
                    my3d::Color cc2 = { 255,0,0 };
                    my3d::present::set_pixel(r, c, cc2);
                }
            }
        }
        my3d::present::present();
        iii++;


    }
}