#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH    (480)
#define SCREEN_HEIGHT   (480)
#define COLOR(a,r,g,b)  (((a) << 24) | ((r) << 16) | ((b)<< 8) | (g))

void full_buffer(SDL_Surface *buff, uint32_t color)
{
    uint32_t *buf = (uint32_t *)buff->pixels;

    for (int i = 0; i < buff->w * buff->h; i ++)
    {
        *buf ++ = color;
    }
}

int main(void)
{
    SDL_Surface *screen = NULL;

    SDL_Init(SDL_INIT_EVERYTHING);

    // 创建一个窗口
    SDL_Window *window = SDL_CreateWindow("show win", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    printf("x:%d  y:%d\n", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED);
    // 渲染层
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // 载入图片来生成材质到渲染器
    SDL_Surface *buffer = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    full_buffer(buffer, COLOR(0x00, 0x00, 0x00, 0xFF));    // arbg
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, buffer);

    // 查看RGB的颜色格式
    printf("RMASK:%x, GMASK:%x, BMASK:%x, AMASK:%x\n", buffer->format->Rmask, buffer->format->Gmask, buffer->format->Bmask, buffer->format->Amask);

    // 复制到渲染器
    SDL_RenderCopy(renderer, texture, NULL, NULL);;

    // 显示
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

    // 清理资源
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // 退出SDL
    SDL_Quit();
    return 0;
}