#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ctype.h>
#include "HAPI.H"

#define Write_File(x,y) Write_File_s(x,y,1)

#ifdef WIN32
    #define LIB_PATH ".\\libast.dll"

    void *m_realloc( void *dest__, unsigned old_size, unsigned new_size ) {
        void *temp_ = malloc( new_size );
        memset( temp_, 0, new_size );

        memcpy( temp_, dest__, old_size );

        return temp_;

    }
#else
    #define LIB_PATH "./libast.so"

#endif

#define LS 0
#define CD 1
#define CONET 2
#define CP 3
#define PT 4
#define EML 5
#define FTP_U 6
#define FTP_R 7
#define FTP_D 8
#define REM 9



int talk_progress;
char File_Name[ 20 ];

char *SystemFilesName[] = { "ANYLIS.OSI", "SYSTEM16.DCL", "CLI.A", "ASSEMBLY.A", "8086_COT.OSI" };
int   SystemFilesSize[] = { 61, 23, 18, 33, 19 };
int   SystemNpcIp[][4] = { {198, 192, 6, 2}, {192, 168, 2, 1}, {172, 91, 2, 2} };
char *SystemNpcName[4] = { "http://www.paidu.com", "https://pilipili.com", "Rick's Windows10" };
int   temp_ip[4];

typedef int (*INT_PAR_FUNC)(int);
typedef int (*NORMALRET_FUNC)();
typedef int (*STRING_PAR_FUNC)(char *);
typedef void (*SIMPLE_FUNC)();

Virtual_PC *player;
Virtual_PC *Controller;
Virtual_PC *Servers[3];

void Prompt( Virtual_PC* vpc );

NORMALRET_FUNC nfunc = NULL;
INT_PAR_FUNC ipfunc = NULL;
STRING_PAR_FUNC spfunc = NULL;
SIMPLE_FUNC sfunc = NULL;
void *handle;

int execute( Virtual_PC* vpc, int c ) {
    int i, tot, j;
    File_* start_;
    File_* f_file, *tf;

    switch ( c ) {
        case LS:
            start_ = vpc->fs->HardDriver[0].file_list;
            printf( "NAME\t\tTYPE\tSIZE\n" );
            for ( i = tot = j = 0; start_[i].deleted != 2; i++ ) {
                if ( start_[i].deleted ) { j++; continue; }
                tot += start_[i].u.file.size;
                printf( "%-12s\t%-4s\t%d KB\n", start_[i].u.file.name ? start_[i].u.file.name : "(null)", \ 
                    (char*[]){ "FOLDER", "FILE" }[start_[i].type], start_[i].u.file.size );
            }

            printf( "\n%15d FILE(S)\t%dKB\n\n", i - j, tot );
            break;

        case CD:
            break;
        case CONET:
            for ( i = 0; i < 3; i++ ) {
                if ( temp_ip[0] == SystemNpcIp[i][0] ) {
                    for ( j = 1; j < 4; j++ ) {
                        if ( temp_ip[j] != SystemNpcIp[i][j] )
                            break;
                    }   
                    if ( j == 4 ) {
                        printf( "SUCCESS FOUND SERVICE ON IP %d.%d.%d.%d\n", temp_ip[0], temp_ip[1], temp_ip[2], temp_ip[3] );
                        printf( "HOSTNAME:%s\n", Servers[i]->Computer_name );
                        Controller = Servers[ i ];
                        Prompt( Servers[ i ] );

                        printf( "Stop Controlling\n" );

                        Controller = vpc;

                        return 0;
                    }
                }
            }

            printf( "FAILD:UNKNOW IP OR SERVICE\n" );

            return 1;
        case CP:
            break;
        case PT:
            break;
        case FTP_D: 
            if ( Controller == player ) {
                printf( "NO TARGET FILE TO DOWNLOAD.STOP.\n" );
                return 1;
            }
            start_ = vpc->fs->HardDriver[0].file_list;
            f_file = Find_File( vpc->fs->HardDriver, File_Name );
            if( f_file ) {
                printf( "DOWNLOADING FILE %s...", f_file->u.file.name );
                Write_File(player->fs->HardDriver, f_file);
                printf( "SUCCESS!\n" );
                return 0;
            }

            printf( "CAN NOT FOUND FILE NAMED %s.(E22)\n", File_Name );

            return 1;
        case FTP_U: break;
        case FTP_R: break;
        case REM:
            start_ = Find_File( vpc->fs->HardDriver, File_Name );
            if ( start_ ) {
                Delete_File( vpc->fs->HardDriver, start_ );
                return 0;
            }

            printf( "CAN NOT FIND FILE %s\n", File_Name );
            return 1;

        case EML:
            printf( "NO E-MAILS YOU HAVE RECIVED YET.\n\nTHIS E-MAIL SYSTEM HAS SUPER COW POWER.\n" );
            return 0;
    }

}

int start_PC( Virtual_PC* PC ) {
    File_* start_ = PC->fs->HardDriver[0].file_list;
    if ( Find_File( PC->fs->HardDriver, "BOOT.DCL" ) ) {
        printf( "BOOT SUCCESSFULLY!\n" );
        PC->isOpen = 1;
        return 0;
    }

    printf( "BOOT FAILD!( CAN NOT FOUND BOOT.DCL OR BROKEN BOOT FILE )\n" );
    return 1;    

}

Virtual_PC* New_PC( int ip[4], char *name ) {
    File_ boot_f; 
    Virtual_PC *n_pc = calloc( 1, sizeof( Virtual_PC ) );
    n_pc->isOpen = 0;

    boot_f.type = 1; boot_f.deleted = 0;
    strcpy( boot_f.u.file.name, "BOOT.DCL\0" );

    boot_f.u.file.fs = NULL; boot_f.u.file.size = 172; boot_f.u.file.user_op = 0; //su
    n_pc->fs = malloc( sizeof( FileSystem ) );    
    n_pc->fs->HardDriver = malloc( sizeof( FileNode ) );

    n_pc->fs->HardDriver[0].t_size = 0;
    
    memcpy( n_pc->ip, ip, sizeof( int ) * 4 );
    n_pc->Computer_name = calloc( 1, strlen(name) );
    
    strcpy( n_pc->Computer_name, name );
    
    Write_File_( n_pc->fs->HardDriver, &boot_f );

    for ( int i = 0; i < 5; i++ ) {
        strcpy( boot_f.u.file.name, SystemFilesName[ i ] );
        boot_f.u.file.size = SystemFilesSize[ i ];

        Write_File_( n_pc->fs->HardDriver, &boot_f );
    }

    return n_pc;
    
}

int Delete_Virtual_PC( Virtual_PC* vpc ) {
    free( (*(vpc->fs->HardDriver)).file_list );
    free( vpc->fs->HardDriver );
    free( vpc->fs );
    free( vpc );
    
    return 0;
}

void Prompt( Virtual_PC* vpc ) {
    int ins = 0;
    int cbt = 0;
    int app_level;
    char *error;
    char ic[ 50 ] = { 0, };
    char tc[ 50 ] = { 0, };
    char ac[ 50 ] = { 0, };

    nfunc = dlsym( handle, "level_" );
    if ( nfunc ) app_level = nfunc();
    else {
        dlclose( handle );
        printf( "LOAD THE APPLICATE FAILD.\nHAVE YOU DEFINE THE FUNCTION 'level_'?\n" );
        return ;
    }



    while ( 1 ) {
        printf( "%s(%d.%d.%d.%d) A:/> ", vpc->Computer_name, vpc->ip[ 0 ], vpc->ip[ 1 ], vpc->ip[ 2 ], vpc->ip[ 3 ] );
        fgets( ic, 100, stdin );
        sscanf( ic, "%s%d.%d.%d.%d", tc, temp_ip, temp_ip+1, temp_ip+2, temp_ip+3 );

        if ( !strcmp( tc, "ls" ) ) execute( vpc, LS );
        else if ( !strcmp( tc, "email" ) ) execute( vpc, EML );
        else if ( !strcmp( tc, "stop" ) ) return ;
        else if ( !strcmp( tc, "connect" ) ) execute( vpc, CONET );
        else if ( !strcmp( tc, "remove" ) ) {
            sscanf( ic, "%s%s", tc, File_Name );
            execute( vpc, REM );
        }
        else if ( !strcmp( tc, "ftp" ) ) {
            sscanf( ic, "%s%s%s", tc, ac, File_Name );
            if ( !strcmp( ac, "d" ) ) { sscanf( ic, "%s%s%s", tc, ac, File_Name ); execute( vpc, FTP_D ); }
            else if ( !strcmp( ac, "u" ) ) { sscanf( ic, "%s%s%s%d.%d.%d.%d", tc, ac, File_Name, temp_ip, temp_ip+1, temp_ip+2, temp_ip+3 ); execute( vpc, FTP_U ); }
            else if ( !strcmp( ac, "r" ) ) { sscanf( ic, "%s%s%s%d.%d.%d.%d", tc, ac, File_Name, temp_ip, temp_ip+1, temp_ip+2, temp_ip+3 ); execute( vpc, FTP_R ); }
            else { printf( "UNKNOW OPERATOR '%s'.(E20)\n", ac ); }
        }
        else if ( !strcmp( tc, "fsite" ) ) {
            for ( int i = 0; i < 3; i++ )
                printf( "%s\n", SystemNpcName[i] );
        }
        else if ( !strcmp( tc, "scan" ) ) {
            sscanf( ic, "%s%s", tc, ac );
            for ( int i = 0; i < 3; i++ ) {
                if ( !strcmp( ac, SystemNpcName[ i ] ) ) {
                    cbt = 1;
                    printf( "IP ADDRESS OF %s:%d.%d.%d.%d\n", ac, SystemNpcIp[i][0], SystemNpcIp[i][1], SystemNpcIp[i][2], SystemNpcIp[i][3] );
                    break;
                }
            }
            if ( !cbt )
                printf( "NO INFORMATION ABOUT SITE %s\n", ac );
        }
        else if ( !strcmp( tc, "help" ) ) {
            printf( "ls\nemail\nstop\nconnect [ip address like xxx.xxx.xxx.xxx]\nremove [filename]\nftp [d/u/r] [filename]\nfsite\nscan [website]\n" );
        }

        else {
            if ( !app_level && vpc == player ) {
                if ( isalpha( *tc ) ) {
                    *(void **)(&sfunc) = dlsym( handle, tc );
                    if ( (error = dlerror()) != NULL ) {
                        printf( "UNKNOW APPLICATION OR CLI COMMAND '%s'\n", tc );
                        dlerror();
                    }
                    else {
                        (*sfunc)();
                    }
                }
            }
            else 
                if ( isalpha( *tc ) )
                    printf( "UNKNOW APPLICATION OR CLI COMMAND '%s'\n", tc );
        }

        memset( ic, 0, sizeof( ic ) );
        memset( tc, 0, sizeof( tc ) );
        cbt = 0;
    }
    
    //

}


//int   SystemNpcIp[][4] = { {198, 192, 6, 2}, {192, 168, 2, 1}, {172, 91, 2, 2} };

int Talking( void ) {
    
}

//drecker

int main( void ) {
    char temp[ 20 ];
    
    printf("________  ________  _______   ________  ___  __    _______   ________     \n"
    "|\\   ___ \\|\\   __  \\|\\  ___ \\ |\\   ____\\|\\  \\|\\  \\ |\\  ___ \\ |\\   __  \\    \n"
    " \\ \\  \\_|\\ \\ \\  \\|\\  \\ \\   __/|\\ \\  \\___|\\ \\  \\/  /|\\ \\   __/|\\ \\  \\|\\  \\   \n"
    "  \\ \\  \\ \\\\ \\ \\   _  _\\ \\  \\_|/_\\ \\  \\    \\ \\   ___  \\ \\  \\_|/_\\ \\   _  _\\  \n"
    "   \\ \\  \\_\\\\ \\ \\  \\\\  \\\\ \\  \\_|\\ \\ \\  \\____\\ \\  \\\\ \\  \\ \\  \\_|\\ \\ \\  \\\\  \\| \n"
    "    \\ \\_______\\ \\__\\\\ _\\\\ \\_______\\ \\_______\\ \\__\\\\ \\__\\ \\_______\\ \\__\\\\ _\\ \n"
    "     \\|_______|\\|__|\\|__|\\|_______|\\|_______|\\|__| \\|__|\\|_______|\\|__|\\|__|\n");
                                                                           

    printf( "\nWelcome to Drecker's BIOS system!\ntype 'BOOT' for starting a new game\n     'EXIT' for stooooping the game.\n" );

    printf( "LOAD APPLICATIONS " );

    handle = dlopen( LIB_PATH, RTLD_LAZY );
    if ( !handle ) {
        fprintf( stderr, "%s", dlerror() );
        printf( "FAILD!\n" );
        return 0;
    }

    dlerror();
    printf( "SUCCESS!\n" );

    sfunc = dlsym( handle, "applicate_" );
    if ( !sfunc ) {
        dlclose( handle );
        printf( "LOAD THE APPLICATE FAILD.\nHAVE YOU DEFINE THE FUNCTION 'applicate_' OR MACRO APP_START?\n" );
        return 0;
    }



    while ( 1 ) {
        printf( "> " );
        fgets( temp, 20, stdin );
        if ( !strcmp( temp, "BOOT\n" ) ) { break; }
        if ( !strcmp( temp, "EXIT\n" ) ) { printf( "goodbye\n" ); return 0; }
        if ( *temp == '\n' ) { continue; }
        else { printf( "UNKNOW COMMAND OR PROGRAM %s", temp ); }
    }
    
    sfunc();

    for ( int i = 0; i < 3; i++ )
        Servers[i] = New_PC( SystemNpcIp[i], SystemNpcName[i] );

    player = New_PC( (int[]){112, 98, 1, 2}, "Deck" );
    if ( start_PC( player ) ) {

        Delete_Virtual_PC( player );
        for ( int i = 0; i < 3; i++ )
            Delete_Virtual_PC( Servers[i] );
        return 0;
    }
    Controller = player;

    Prompt( player );

    Delete_Virtual_PC( player );

    for ( int i = 0; i < 3; i++ )
        Delete_Virtual_PC( Servers[i] );

    return 0;
}
