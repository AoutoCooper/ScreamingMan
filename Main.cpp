#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <stdio.h>
#include <SDL_syswm.h>
#include <Windows.h>
#include <iostream>
#include <SDL_Mixer.h>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

using namespace std;
 
bool Initialize();
void Close();
SDL_Texture* LoadTexture(std::string file);
bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey);
void PlayAudio();

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

SDL_Texture* texture = NULL;

int main(int argc, char* args[]) {

	if (!Initialize()) {
		printf("Could not initialize.\n");
		return -1;
	}	

	texture = LoadTexture("skl.png");
	if (texture == NULL) {
		printf("Couldn't load image.");
		return -1;
	}
	
	MakeWindowTransparent(window, RGB(0, 0, 0));

	bool exit = false;
	bool mouseButtonHeld = false;
	int mouseX{ 0 };
	int mouseY{ 0 };

	int windowHeight{ 0 };
	int windowWidth{ 0 };

	SDL_GetWindowSize(window, &windowWidth, &windowHeight);


	SDL_Event e;

	while (!exit) {

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) {
				exit = true;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				mouseButtonHeld = false;
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				mouseButtonHeld = true;
				PlayAudio();
			}

			if (mouseButtonHeld) {
				SDL_GetGlobalMouseState(&mouseX, &mouseY);
				SDL_SetWindowPosition(window, mouseX - windowWidth / 2, mouseY - windowHeight / 2);
			}
		}

		//Clear the renderer
		SDL_RenderClear(renderer);
		//render thge image
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		//show the final render
		SDL_RenderPresent(renderer);
	}
	
	Close();

	return 0;
}

bool Initialize() {
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initialized. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if (Mix_Init(0) < 0) {
		printf("Mixer could not be initialized. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("Screaming Man",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		700,
		517,
		SDL_WINDOW_BORDERLESS);
	
	SDL_SetWindowOpacity(window, 0);

	if (window == NULL) {
		printf("Window couldn't be created.SDL_Error: %s\n", SDL_GetError());
			return false;
	};

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	if (renderer == NULL) {
		printf("Unable to create renderer. SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Chroma key logic. Everything that's blcak becaomes transparent.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	int imgFlags = IMG_INIT_PNG;
	
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		
		printf("SDL image could not be initialized. SDL Error: %s\n", SDL_GetError());
		return false;
	}

	Mix_OpenAudio(441000, MIX_DEFAULT_FORMAT, 1, 1024);

	return true;
}

void Close() {
	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();
}

SDL_Texture* LoadTexture(std::string file) {
	
	SDL_Surface* loadedSurface = IMG_Load(file.c_str());
	SDL_Texture* newTexture = NULL;

	if (loadedSurface == NULL) {
		printf("Unable to load image from the specified path. SDL Error: %s\n", SDL_GetError());
	}
	else {
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		
		if (newTexture == NULL) {
			printf("Unable to crete a texture from the image. SDL Error: %s\n", SDL_GetError());
		}

		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey) {
	// Get window handle (https://stackoverflow.com/a/24118145/3357935)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	// Change window type to layered (https://stackoverflow.com/a/3970218/3357935)
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	// Set transparency color
	return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

void PlayAudio() {
	
	Mix_Chunk* sound = Mix_LoadWAV("sound.wav");
	
	Mix_PlayChannel(-1, sound, 0);
}