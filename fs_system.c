#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "HAPI.H"

int   Pos_;
int   Pos_DeletedFile[100];

File_* Find_File( FileNode* hd, char *name ) {
    if( !hd ) return NULL;
    File_* start_ = hd[0].file_list;
    for ( int i = 0; start_[i].deleted != 2; i++ ) {
        if ( start_[i].type && start_[i].u.file.name && !start_[i].deleted ) {
            if ( !strcmp( start_[i].u.file.name, name ) ) {
                return start_ + i;
            }
        }
    }

    return NULL;
}

int Delete_File( FileNode* hd, File_* f ) {
    if ( !hd ) return 1;
    File_* start_ = hd[0].file_list;
    for ( int i = 0; start_[i].deleted != 2; i++ ) {
        if ( start_[i].type && start_[i].u.file.name && !start_[i].deleted ) {
            if ( start_ + i == f ) {
                Pos_DeletedFile[ Pos_++ ] = i;
                f->deleted = 1;

                return 0;
            }
        }
    }
    return 1;

}

int Write_File_s( FileNode* hd, File_* f, int alert ) {
    char ch;
    char new_name[ 20 ];
    File_* tf = Find_File( hd, f->u.file.name );
    if ( tf ) {
        if ( alert ) {
            printf( "WARNING:YOU ARE TRYING TO REPLACE A SAME NAMED FILE.\n" );
            printf( "CHOOSE YOUR OPERATION:\nr:rename\tp:replace\n" );
            ch = getchar();
            switch ( ch ) {
                case 'r':
                    printf( "ENTER THE NEW NAME:" ); 
                    scanf( "%20s", new_name ); 
                    strcpy( f->u.file.name, new_name );
                    Write_File_( hd, f );
                    return 1;

                case 'p':
                    *tf = *f;
                    return 2;

                default: printf( "WRONG OPERATION.STOP\n" ); return 3;
            }
        }

        *tf = *f;
        printf( "WARNING:YOU HAVE REPLACED A SAME NAMED FILE\n" );
    }

    else {
        Write_File_( hd, f );
    }

    return 0;
}

int Write_File_( FileNode* hd, File_* f ) {
    if ( !hd ) return 1;
    File_* fl = hd->file_list;

    if ( !Pos_ ) {
    #ifdef WIN32
        fl = m_realloc( fl, (hd->t_size++ + 1) * sizeof( File_ ), (hd->t_size + 1) * sizeof( File_ ) );
    #else
        fl = (File_ *)realloc( fl, (++hd->t_size) * sizeof( File_ ) );
    #endif
        fl[ hd->t_size-1 ] = *f;

        fl[ hd->t_size ].deleted = 2;
    }
    else {
        fl[ Pos_DeletedFile[ --Pos_ ] ] = *f;
        //printf( "Pos_:%d\npos:%d\nf.name:%s\n", Pos_, Pos_DeletedFile[ Pos_ ], f->u.file.name );
        //fl[ Pos_DeletedFile[ Pos_ ] ].deleted = 0;
    }

    hd->file_list = fl;
    
    return 0;
}