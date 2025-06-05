//
// Created by userbrigh on 6/4/25.
//

#ifndef KONSOLA_H
#define KONSOLA_H

class szachy {
    void start();
    void  Wyswietl_Plansze();
    void Wyswietl_Figury();
    std::vector<int> plansza;
    std::vector<std::pair<int, std::pair<int, int>>> figury;
};
#endif //KONSOLA_H
