#include "test.h"
#include "../model/game_model.h"
#include <iostream>

TEST_CASE(placeholder) {
    expect(1 == 1, "some msg");
}

const int msize = 32;
int map[msize][msize];

TEST_CASE(model_simplistic_test) {
    //return;


    using namespace T;
    GameModel gm(msize, msize);


    gm.set_new_particles(ParticleBrush(vec2(10, 15), 4, ParticleType::Sand));
    //getchar();
    //cout << endl;
    //for (int i = 0; i < 30000; i++) {
    //    gm.update();
    //}
    //cout << "done" << endl;
    //for (int i = 0; i < 3000; i++) {
    //    gm.update();
    //}
    //cout << "done" << endl;

    while (1) {
        getchar();
        gm.update();
        auto res = gm.query_particles();
        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                map[r][c] = -1;
            }
        }
        for (int i = 0; i < res.position.size(); i++) {
            ivec2 ip = f2i(res.position[i]);
            if (ip.x < msize && ip.x >= 0 && ip.y < msize && ip.y >= 0) {
                map[ip.y][ip.x] = 1;
            }
        }
        //for (int r = 0; r < msize; r++) {
        //    for (int c = 0; c < msize; c++) {
        //        if (map[r][c] == -1) {
        //            cout << '#';
        //        }
        //        else {
        //            cout << map[r][c] % 10;
        //        }
        //    }
        //    cout << endl;
        //}

        for (int r = 0; r < msize; r++) {
            for (int c = 0; c < msize; c++) {
                cout << gm.airstate.p[gm.idx(r, c)];
            }
            cout << endl;
        }
    }
}