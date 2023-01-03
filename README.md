# DRECKER
A Simple Hacker Simulator with an unix-like system

V0.25
更新了软件系统，可以通过HAPI.H来编写在这个系统中属于你自己的程序
其中，必须包括两个函数
void applicate_()（APP_START为它的宏，可添加虚拟系统初始化时的要输出的内容）
int level_()（跟虚拟操作系统沟通其作用域）

编译应用指令:gcc [file] -fPIC -shared -o libast.[so/dll]并用其替换原有的libast.so/dll
编译主程序指令（必须一步一步来）:
gcc fs_system.c -fPIC -shared -o libfssys.so/dll
gcc game_v2.c -lfssys -L. -o DRECKER[.EXE] -ldl
