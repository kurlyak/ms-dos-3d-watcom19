//===============================================================
//      Ed Kurlyak 2020 Software Rendering
//===============================================================

#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include "Mesh.h"
#include <time.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 200

#define VGA_INPUT_STATUS_1   0x3DA
#define VGA_VSYNC_MASK 0x08

void Wait_For_Vsync(void)
{
    while (inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK)
    {
    }
    while (! (inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK) )
    {
    }
}

int main ()
{

    _asm {
            mov ax, 0x4f02
            mov bx, 0x101 //set resolution 640x480
            int 10h
        }

    unsigned char * my_video_buffer;
    my_video_buffer = new unsigned char[307200]; //640 * 480 = 307200

    char * video_buffer =(char*) 0xA0000L;

    int back_lpitch = 640;

    CMeshManager MeshManager;
    MeshManager.InitCube(my_video_buffer, back_lpitch);


    while(!kbhit())
    {

        //Wait_For_Vsync();
        memset(my_video_buffer,0,307200);

        MeshManager.CalculateCube();
        MeshManager.DrawCube () ;

        Wait_For_Vsync();
                
        for ( short int i = 0; i < 5; i++)
        {
            if(i < 4)
            {
                _asm {
                        mov ax, 0x4f05  //функцию установки видеостраницы
                        mov dx, i   //номер видео страницы от 0 до 4
                        mov bx, 0x0 
                        int 10h
                    }
                    
                memcpy((unsigned char*)video_buffer, my_video_buffer + i * 65536, 65536);
            }
            
            if(i == 4)
            {
                _asm {
                        mov ax, 0x4f05  //функцию установки видеостраницы
                        mov dx, 4   //номер видео страницы от 0 до 4
                        mov bx, 0x0 
                        int 10h
                    }

                memcpy((unsigned char*)video_buffer, my_video_buffer + 4 * 65536, 45056);
            }
        } //for ( short int i = 0; i < 5; i++)
    }

    _asm {
            mov ah, 0x0;
            mov al,0x03; //set text video mode
            int 10h;  //set video mode call interrupt
        }

    return 0;
}
