#include "nbsoc.h"
#include "simulatorwindow.h"

#include <QApplication>

#include <iostream>
#include <unistd.h>

void printUsage(char* exe)
{
    std::cout << "Usage:" << exe << "-s <sd card img> -f <flash img> -b <block ram> " << std::endl;
}

int main(int argc, char** argv)
{

    // Parse arguments

    char* blockRamImg = nullptr;
    char* flashImg = nullptr;
    char* sdImg = nullptr;

    char c;

    while ((c = getopt (argc, argv, "b:f:s:")) != -1)
    switch (c)
    {
        case 's':
            sdImg = strdup(optarg);
            break;
        case 'f':
            flashImg = strdup(optarg);
            break;
        case 'b':
            blockRamImg = strdup(optarg);
            break;
        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
    }


    // Create an nbSoC

    nbSoC nanobrain;

    // Configure memory - load block ram image

    if (blockRamImg == nullptr)
    {
        std::cout << "Error: block ram image not specified" << std::endl;
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    nanobrain.configureBlockRam(blockRamImg);

    // Load flash image

    if (flashImg == nullptr)
    {
        std::cout << "Error: flash image not specified" << std::endl;
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    nanobrain.configureFlash(flashImg);

    // Load sd card image

    if (sdImg == nullptr)
    {
        std::cout << "Error: SD card image not specified" << std::endl;
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    nanobrain.configureSDCard(sdImg);

    // Create UI
    // Run GUI

    QApplication a(argc, argv);
    SimulatorWindow w;
    w.setWindowTitle("NanoBrainSoC Simulator");

    w.setOnCloseEvent([&] () { nanobrain.shutDown(); });

    w.show();

    // Set signals.

    nanobrain.onBlitToGfxRam ([&] ()         { w.onBlitToGfxRam(nanobrain); });
    nanobrain.onLedGreenWrite([&] (uint16_t ledGreen) { w.onLedGreenWrite(ledGreen); });
    nanobrain.onLedRedWrite([&]   (uint16_t ledRed)   { w.onLedRedWrite(ledRed); });
    nanobrain.onHexWrite([&]   (int hex, uint16_t val)   { w.onHexWrite(hex, val); });

    w.onResetButtonPressed([&] (bool pressed) { nanobrain.onResetButtonPressed(pressed); });

    // Boot cpu

    nanobrain.start();

    return a.exec();

}
