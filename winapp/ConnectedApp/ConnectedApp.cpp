#include <iostream> // Include the input-output stream library for console I/O
#include <thread> // Include the thread library for multithreading
#include "CommonObject.h" // Include the header for the CommonObjects class
#include "DrawThread.h" // Include the header for the DrawThread class
#include "DownloadThread.h" // Include the header for the DownloadThread class

int main() {
    CommonObjects common;

    DrawThread draw;

    DownloadThread down;

    // Seed the random number generator with the current time
    srand(time(0));

    // Set the URL for the DownloadThread instance
    down.SetUrl("https://api.openbrewerydb.org");

    // Start a thread for the draw operation
    auto draw_th = std::jthread([&] { draw(common); });

    // Start a thread for the download operation
    auto down_th = std::jthread([&] { down(common); });

    std::cout << "running...\n";

    // Wait for the download thread to finish
    down_th.join();

    // Wait for the draw thread to finish
    draw_th.join();

    return 0;
}
