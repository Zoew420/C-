#include "my3dpresent.h"
#include "../model/game_model.h"

using namespace T;
const int msize = 60;
int map[msize][msize];


int main() {
    my3d::present::initialize(GetConsoleWindow(), 120, 120, 3);
    GameModel gm(msize, msize);
    // gm.set_new_particles(ParticleBrush(vec2(30, 20), 4, ParticleType::Sand));
    while (1) {
        // getchar();
        Sleep(10);
        gm.airflow_solver.cleanBuffer();
        gm.airflow_solver.setVX0(20, 20, 10);
        gm.airflow_solver.addSource();
        gm.update();

        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                int pos = gm.idx(r, c);

        
                //p = gm.airstate.mapv.cur()->map_vy[pos];
                float p = gm.airflow_solver.vx[pos];
                float pc = p * 100 + 100;

                my3d::Color cc = { pc,pc,pc };
                my3d::present::set_pixel(c, r, cc);
                if (!gm.state_cur.map_index[pos].nil()) {
                    my3d::Color cc2 = { 255,0,0 };
                    my3d::present::set_pixel(c, r, cc2);
                }

                //if (r == 20 && c == 20) {
                //    gm.airstate.mapv.cur()->map_vx[pos] = 0.01;
                //}

            }
        }
        my3d::present::present();

        

    }
}