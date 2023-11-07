#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#if !defined(TEXT_H)
#define TEXT_H

typedef struct Text {
    char* buffer;
    size_t length;
    size_t maxLength;
    TTF_Font* font;
    struct {
        int requireRerender;
        int width;
        int height;
        SDL_Texture* texture;
    } sdl;
} Text;

void Text_init(Text* text, char* buffer, size_t bufferLength, TTF_Font* font);
void Text_set(Text* text, char* value);
void Text_append(Text* text, char* textToAppend, size_t length);
void Text_backspace(Text* text);
SDL_Texture* Text_getTexture(Text* text, SDL_Renderer* renderer);

#endif // TEXT_H
