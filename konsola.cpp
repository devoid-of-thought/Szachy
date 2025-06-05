#include <vector>
#include <iostream>
#include "konsola.h"
using namespace  std;


void szachy::start() {
    vector<int> fig_start = {5, 2, 3, 9, 10, 3, 2, 5};
    for(int i = 0; i<8;i++) {
        plansza.push_back(fig_start[i]*-1);
        figury.push_back({fig_start[i]*-1, {i, 0}});
    }
    for(int i = 0; i<8;i++) {
        plansza.push_back(-1);
        figury.push_back({-1, {i, 1}});
    }
    for(int i = 0; i<32;i++) {
        plansza.push_back(0);
    }
    for(int i = 0; i<8;i++) {
        plansza.push_back(1);
        figury.push_back({1, {i, 6}});
    }
    for(int i = 0; i<8;i++) {
        plansza.push_back(fig_start[i]);
        figury.push_back({fig_start[i], {i, 7}});
    }
}

void szachy::Wyswietl_Plansze() {
    for(int i = 0; i<8;i++) {
        for(int j = 0; j<8;j++) {
            cout << plansza[i*8+j] << " ";
        }
        cout <<endl;
    }
}
void szachy::Wyswietl_Figury() {
    for(int i = 0; i<figury.size();i++) {
        cout << figury[i].first << " " << figury[i].second.first << " " << figury[i].second.second << endl;
    }
}