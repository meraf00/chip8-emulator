#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

// scale factor to scale window size
#define SCALE   10

// display
void drawPixel(SDL_Renderer *renderer, int x, int y);
void renderDisplay(SDL_Renderer *renderer);

byte keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char *argv[])
{       
    CHIP_Initalize();    

    if (argc > 1 && CHIP_LoadProgram(argv[1]) == -1)
    {
        fprintf(stderr, "Unable to open file. %s", SDL_GetError());
        exit(-1);
    }

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)
    {
        fprintf(stderr, "Initialization failed. %s", SDL_GetError());
        exit(-1);
    }
    
    

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    window = SDL_CreateWindow("CHIP8 Emulator : Drag and drop CHIP8 ROM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH*SCALE, HEIGHT*SCALE, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);



    for(;;)
    {   
        CHIP_EmulateCycle();
        
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                break;
            
            if (event.type == SDL_KEYDOWN)
            {   
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    break;

                // Singnal Keyboard press
                for (int i=0; i < 16; i++)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        Keyboard[i] = 1;
                    }
                }
            }

            // Reset released keys to 0
            if (event.type == SDL_KEYUP)
            {
                for (int i=0; i < 16; i++)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        Keyboard[i] = 0;
                    }
                }
            }

            if (event.type == SDL_DROPFILE)
            {   
                CHIP_Initalize();
                CHIP_LoadProgram(event.drop.file);
            }
        }
        else
        {               
            if (drawFlag)
            {   
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                renderDisplay(renderer);                

                SDL_RenderPresent(renderer);

                drawFlag = 0;
            }                

            
            SDL_UpdateWindowSurface(window);
            
            
        }
    }

    SDL_Quit();
    
    return 0;
}

void drawPixel(SDL_Renderer *renderer, int x, int y)
{       
    for (int h=0; h < SCALE; h++)
    {
        for (int w=0; w < SCALE; w++)
        {
            SDL_RenderDrawPoint(renderer,(x * SCALE) + w, (y * SCALE) + h);
        }
    }
    
}

void renderDisplay(SDL_Renderer *renderer)
{
    for (int h = 0; h < HEIGHT; h++)
    {
        for (int w = 0; w < WIDTH; w++)
        {
            if (Display[h * WIDTH + w] == 1)
            {
                drawPixel(renderer, w, h);
            }
        }
    }
}

