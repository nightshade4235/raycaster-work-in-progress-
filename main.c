#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define SC_WIDTH 900
#define SC_HEIGHT 600
#define M_HEIGHT 16
#define M_WIDTH 16

const int Map[M_HEIGHT][M_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,0,1,1,1,0,0,0,1},
    {1,0,1,1,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


int main(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("init fail %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SC_WIDTH, SC_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SC_WIDTH, SC_HEIGHT);

    bool running = true;
    SDL_Event event;

    Uint32 pixels[SC_WIDTH * SC_HEIGHT];

    double posX = 1.5, posY = 1.5;
    double dirX = -1.0, dirY = 0.0;
    double planeX = 0.0, planeY = 0.85;

    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_Q]) running = false;

        double moveSpeed = 0.05;
        double rotSpeed = 0.03;


        if(state[SDL_SCANCODE_W]){
            if(Map[(int)(posX + dirX * moveSpeed)][(int)(posY)] == 0) posX += dirX * moveSpeed;
            if(Map[(int)(posX)][(int)(posY + dirY * moveSpeed)] == 0) posY += dirY * moveSpeed;
        }
        
        if(state[SDL_SCANCODE_S]){
            if(Map[(int)(posX - dirX * moveSpeed)][(int)(posY)] == 0) posX -= dirX * moveSpeed;
            if(Map[(int)(posX)][(int)(posY - dirY * moveSpeed)] == 0) posY -= dirY * moveSpeed;
        }

        if(state[SDL_SCANCODE_D]){
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
    
        if(state[SDL_SCANCODE_A]){
            double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }

        for(int y = 0; y < SC_HEIGHT; y++){
            Uint32 ceilingColor = 0xFF222222;
            Uint32 floorColor = 0xFF555555;
            for(int x = 0; x < SC_WIDTH; x++) {
                pixels[y * SC_WIDTH + x] = (y < SC_HEIGHT / 2) ? ceilingColor : floorColor;
            }
        }

        for(int x = 0; x < SC_WIDTH; x++){
            double cameraX = 2.0 * x/ (double)SC_WIDTH - 1.0; //dat formula for le polarity of looking :)

            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;
        
            int mapX = (int)posX;
            int mapY = (int)posY;
            
            double sideDistX;
            double sideDistY;
            
            double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1.0 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1.0 / rayDirY);
            double perpWallDist;
            
            int stepX;
            int stepY;
            
            int hit = 0;
            int side;
            
            if (rayDirX < 0) {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            } else {
                stepX = 1;  
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }

            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            } else {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }

            while(hit == 0){ //no understand from here
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                
                if (Map[mapX][mapY] > 0) hit = 1;
            }

            if(side == 0) perpWallDist = (sideDistX - deltaDistX);
            else          perpWallDist = (sideDistY - deltaDistY);

            int lineHeight = (int)(SC_HEIGHT / perpWallDist);

            int drawStart = -lineHeight / 2 + SC_HEIGHT / 2;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + SC_HEIGHT / 2;
            if (drawEnd >= SC_HEIGHT) drawEnd = SC_HEIGHT - 1;

            Uint32 wallColor = (side == 1) ? 0xFF0088FF : 0xFF00CCFF;
            if(side == 1) wallColor = (side == 1) ? 0xFF004488 : 0xFF0088CC;
            
            for (int y = drawStart; y < drawEnd; y++) {
                pixels[y * SC_WIDTH + x] = wallColor;
            }
        }

        SDL_UpdateTexture(framebuffer, NULL, pixels, SC_WIDTH * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(framebuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
