
#ifdef _WIN32

#include <stdbool.h>
#include <windows.h>

bool wDetachFromTerminal() {

    FreeConsole();

    return true;
}

#endif

