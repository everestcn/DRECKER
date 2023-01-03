#include "HAPI.H"

APP_START {
    printf( "Hello, this is my first dll application.\nfor more information type 'help_app' after boot\n" );
}

INT_FUNC(level_) {
    return ONLY_PLAYER;
}

VOID_FUNC(hello) {
    printf( "running my first program here!\n" );
}

VOID_FUNC(help_app) {
    printf( "hello - my first program\nhelp_app - help\n" );
}
