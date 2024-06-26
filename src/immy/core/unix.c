
#ifdef __unix__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool uDetachFromTerminal() {

    int pid = fork();

    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (pid < 0)
        return false;

    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    return true;
}


#endif

