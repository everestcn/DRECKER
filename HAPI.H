#ifndef HAPI_H_
#define HAPI_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define APP_START void applicate_()
#define VOID_FUNC(x) void x()
#define INT_FUNC(x)  int x()
#define INT_INT_PARA_FUNC(x,y) int x( int y )
#define INT_STRING_PARA_FUNC(x,y) int x( char *y )

#define ONLY_PLAYER 0
#define ALL_SERVER 1
#define EXCEPT_PLAYER 2

typedef struct File_ File_;
typedef struct FileNode FileNode;
typedef struct FileSystem FileSystem;
typedef struct Virtual_PC Virtual_PC;

struct FileNode { int user_op; unsigned t_size; File_* file_list; };

struct File_ {
    bool type; int deleted;
    union {
        struct { char name[20]; char *fs; unsigned size; int user_op; } file;
        FileNode f;
    } u;  
};

struct FileSystem { FileNode* HardDriver; };
struct Virtual_PC { int ip[ 4 ]; int isOpen; char *Computer_name; FileSystem* fs; };

int Write_File_( FileNode* hd, File_* f );
int Write_File_s( FileNode* hd, File_* f, int alert );
int Delete_File( FileNode* hd, File_* f );
File_* Find_File( FileNode* hd, char *name );

#endif