#include <windows.h>
#include "argcargv.h"
#include "init.h"

int main(int, char **);

void mainCRTStartup(void)
{
    int mainret, argc;

    argc = _ConvertCommandLineToArgcArgv();

    __init();

    mainret = main(argc, _ppszArgv);

    __exit();

    ExitProcess(mainret);
}
