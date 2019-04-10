#define _CRT_SECURE_NO_WARNINGS
#include "AmaApp.h"


int main()
{
    initialize("inventory.txt");
    int status = run();
    cleanup();
    return status;
}
