#include "test.h"
#include "../model/game_model.h"
#include <iostream>

TEST_CASE(placeholder) {
    expect(1 == 1, "some msg");
}

TEST_CASE(model_simplistic_test) {
    return;

    using namespace T;
    GameModel gm(10, 10);
    int map[10][10];

    gm.set_new_particles(ParticleBrush(vec2(2, 2), .5, ParticleType::Sand));
    cout << endl;
    while (1) {
        getchar();
        gm.update();
        auto res = gm.query_particles();
        for (int r = 0; r < 10; r++) {
            for (int c = 0; c < 10; c++) {
                map[r][c] = -1;
            }
        }
        for (int i = 0; i < res.position.size(); i++) {
            ivec2 ip = f2i(res.position[i]);
            if (ip.x < 10 && ip.x >= 0 && ip.y < 10 && ip.y >= 0) {
                map[ip.y][ip.x] = 1;
            }
        }
        for (int r = 0; r < 10; r++) {
            for (int c = 0; c < 10; c++) {
                if (map[r][c] == -1) {
                    cout << '#';
                }
                else {
                    cout << map[r][c] % 10;
                }
            }
            cout << endl;
        }
    }
}