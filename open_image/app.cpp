/*
 * Main function of the application
 */

#include <iostream>

// App specific
#include <app.h>

int main(int argc, char **argv)
{
    std::cout << "Hello World!\n";
    if (argc > 1) {
    open_image(argv[1]);
    }
}