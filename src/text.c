#include "text.h"

#include <string.h>

void Text_init(Text* text, char* buffer, size_t bufferLength, TTF_Font* font) {
    text->buffer = buffer;
    text->font = font;
    text->length = 0;
    text->maxLength = bufferLength;

    text->sdl.texture = NULL;
    text->sdl.requireRerender = 1;

    memset(text->buffer, 0, bufferLength);
}

void Text_set(Text* text, char* value) {
    strncpy(text->buffer, value, text->maxLength);
    text->length = strlen(value);
    text->sdl.requireRerender = 1;
}

void Text_append(Text* text, char* textToAppend, size_t length) {
    if(text->maxLength - length > text->length) {
        strcat(text->buffer, textToAppend);
        text->length += length;

        text->sdl.requireRerender = 1;
    }
}

void Text_backspace(Text* text) {
    if(text->length > 0) {
        text->length--;
        text->buffer[text->length] = '\0';

        text->sdl.requireRerender = 1;
    }
}

SDL_Texture* Text_getTexture(Text* text, SDL_Renderer* renderer) {
    if(text->sdl.requireRerender) {
        text->sdl.requireRerender = 0;
        
        if(text->sdl.texture) {
            SDL_DestroyTexture(text->sdl.texture);
        }

        SDL_Color color = {
            .a = 255,
            .r = 255,
            .g = 255,
            .b = 255,
        };

        SDL_Surface* surface = TTF_RenderText_Solid_Wrapped(text->font, text->buffer, color, 1000);

        if(!surface) {
            text->sdl.width = 0;
            return NULL;
        }

        text->sdl.width = surface->w;
        text->sdl.height = surface->h;
        text->sdl.texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);

    }

    return text->sdl.texture;
}
