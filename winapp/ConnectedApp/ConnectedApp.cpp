// ConnectedApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include "CommonObject.h"
#include "DrawThread.h"
#include "DownloadThread.h"


int main() {
    CommonObjects common;
    DrawThread draw;
    DownloadThread down;

    down.SetUrl("https://api.openbrewerydb.org");

    auto draw_th = std::jthread([&] { draw(common); });
    auto down_th = std::jthread([&] { down(common); });

    std::cout << "running...\n";

    down_th.join();
    draw_th.join();

    return 0;
}