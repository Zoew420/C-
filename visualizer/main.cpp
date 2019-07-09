#include "my3dpresent.h"
#include "../model/game_model.h"

using namespace T;
const int msize = 400;
int map[msize][msize];


int main() {
    my3d::present::initialize(GetConsoleWindow(), 800, 800, 2);
    GameModel gm(msize, msize);
    gm.set_new_particles(ParticleBrush(vec2(120, 20), 10, ParticleType::Sand));
    
    //getchar();
    //cout << "start";
    //for (int i = 0; i < 1000; i++) {
    //    gm.update();
    //}
    //cout << endl;
    
    while (1) {

        gm.update();

        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                int pos = gm.idx(r, c);
                float p = gm.airflow_solver.p[pos];
                float pc = p * 50 + 100;

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