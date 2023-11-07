#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "text.h"

#define FPS 60
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

float msPerFrame = 1000.0f / (float)FPS;

int main(void) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    if(TTF_Init() < 0) return 1;

    SDL_Window* window = SDL_CreateWindow(
        "Browser",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DEFAULT_WIDTH,
        DEFAULT_HEIGHT,
        0
    );

    SDL_Rect inputBox = {
        .w = DEFAULT_WIDTH - 10,
        .h = 30,
        .x = 5,
        .y = 5
    };

    SDL_Rect inputCursor = {
        .w = 2,
        .h = 24,
        .x = 10,
        .y = 8
    };

    SDL_Rect textRect = {
        .x = 10,
        .y = 8
    };

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("assets/RobotoMono-Medium.ttf", 20);
    char inputBuffer[200];
    Text input;
    Text_init(&input, inputBuffer, sizeof(inputBuffer), font);

    SDL_StartTextInput();
    int isInputFocused = 1;
    int cursorFrames = 0;
    
    char addressText_buffer[1000];
    Text responseText;
    Text_init(&responseText, addressText_buffer, sizeof(addressText_buffer), font);
    Text_set(&responseText, "Enter hostname");
    
    int scrollProgress = 0;
    
    int doRequest = 0;
    SDL_Event e;
    int quit = 0;
    while(!quit) {
        Uint64 start = SDL_GetPerformanceCounter();

        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;

                case SDL_MOUSEBUTTONDOWN: {
                    SDL_Point mousePos = { .x = e.button.x, .y = e.button.y };
                    if(SDL_PointInRect(&mousePos, &inputBox)) {
                        SDL_StartTextInput();

                        isInputFocused = 1;
                        cursorFrames = 0;
                    } else {
                        if(isInputFocused) {
                            SDL_StopTextInput();
                        }
                        isInputFocused = 0;
                    }
                    break;
                }

                case SDL_KEYDOWN:
                    if(isInputFocused) {
                        switch(e.key.keysym.sym) {
                            case SDLK_BACKSPACE:
                                Text_backspace(&input);
                                break;
                            
                            case SDLK_RETURN:
                                if(input.length > 0) {
                                    doRequest = 1;
                                    scrollProgress = 0;
                                }
                                break;
                        }
                    }

                    break;

                case SDL_TEXTINPUT: 
                    Text_append(&input, e.text.text, strlen(e.text.text));
                    break;
                
                case SDL_MOUSEWHEEL:
                    if(!isInputFocused) scrollProgress -= e.wheel.y;
                    break;
            }
        }

        if(doRequest) {
            doRequest = 0;
            
            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            struct addrinfo* res;
            if(getaddrinfo(input.buffer, "http", &hints, &res)) {
                Text_set(&responseText, "404");
            } else {
                struct sockaddr_in* serverAddress = (struct sockaddr_in*)res->ai_addr;
                
                int request = socket(AF_INET, SOCK_STREAM, 0);
                
                struct sockaddr_in requestAddress;
                memset(&requestAddress, 0, sizeof(requestAddress));


                requestAddress.sin_family = AF_INET;
                requestAddress.sin_port = serverAddress->sin_port;
                memcpy(&requestAddress.sin_addr.s_addr, &serverAddress->sin_addr.s_addr, sizeof(serverAddress->sin_addr.s_addr));

                if(connect(request, (struct sockaddr*)&requestAddress, sizeof(requestAddress)) < 0) {
                    printf("Failed to connect\n");
                }

                char message[] = 
                    "GET / HTTP/1.1\r\n"
                    "Accept: text/html\r\n"
                    "Accept-Charset: ASCII\r\n"
                    "\r\n";

                send(request, message, sizeof(message), 0);

                Text_set(&responseText, "");

                responseText.length = read(request, responseText.buffer, responseText.maxLength);
                responseText.sdl.requireRerender = 1;

                close(request);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Texture* text = Text_getTexture(&responseText, renderer);
        if(text) {
            SDL_Rect responsePosition = {
                .x = 5,
                .y = 40 - scrollProgress,
                .w = responseText.sdl.width,
                .h = responseText.sdl.height
            };

            SDL_RenderCopy(renderer, text, NULL, &responsePosition);
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &inputBox);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &inputBox);

        inputCursor.x = textRect.x;
        text = Text_getTexture(&input, renderer);
        if(text) {
            textRect.w = input.sdl.width,
            textRect.h = input.sdl.height,
            textRect.y = inputBox.y + inputBox.h / 2 - input.sdl.height / 2;

            SDL_RenderCopy(renderer, text, NULL, &textRect);
            inputCursor.x += textRect.w;
        }

        if(isInputFocused) {
            if(cursorFrames % 50 > 25) {
                SDL_RenderDrawRect(renderer, &inputCursor);
            }
            cursorFrames++;
        }



        SDL_RenderPresent(renderer);

        Uint64 end = SDL_GetPerformanceCounter();

        float timeForFrame = (end - start) * 1000.0f / (float)SDL_GetPerformanceFrequency();
        
        if(timeForFrame < msPerFrame) {
            SDL_Delay(msPerFrame - timeForFrame);
        }
    }
    
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}