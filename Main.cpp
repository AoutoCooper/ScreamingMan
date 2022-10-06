#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <stdio.h>
#include <SDL_syswm.h>
#include <Windows.h>
#include <iostream>
#include <SDL_Mixer.h>
#include "Timer.h"

const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 200;

const int MAX_FRAME_RATE = 60;
using namespace std;

bool Initialize();
bool InitWindow();
void Close();
void FixedUpdate();
SDL_Texture* LoadTexture(std::string file);
bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey);
void PlayAudio();

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

SDL_Texture* moonSmug = NULL;

SDL_Texture* moonScared = NULL;

Timer* mTimer;

bool scared = false;

float currentTime{ 0.0 };
float timeToBeSad{ 2.0 };

int main(int argc, char* args[]) {

	if (!Initialize()) {
		printf("Could not initialize.\n");
		return -1;
	}

	moonSmug = LoadTexture("images/moonSmug.png");
	if (moonSmug == NULL) {
		printf("Couldn't load image.");
		return -1;
	}

	moonScared = LoadTexture("images/moonScared.png");
	if (moonSmug == NULL) {
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

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				mouseButtonHeld = true;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				mouseButtonHeld = false;
				scared = true;
				//wait for finished something, then play audio
				PlayAudio();
			}

			if (mouseButtonHeld) {
				SDL_GetGlobalMouseState(&mouseX, &mouseY);
				SDL_SetWindowPosition(window, mouseX - windowWidth / 2, mouseY - windowHeight / 2);
			}


		}

		mTimer->Update();
		//framerate independet update
		if (mTimer->GetDeltaTime() >= 1.0f / MAX_FRAME_RATE) {
			//render the image
			FixedUpdate();
			mTimer->Reset();
		}
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
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_BORDERLESS);

	if (!InitWindow()) {
		printf("Could not create window.");
	}


	Mix_OpenAudio(192000, MIX_DEFAULT_FORMAT, 1, 2048);

	mTimer = Timer::Instance();
	return true;
}

void FixedUpdate() {
	if (scared) {
		SDL_RenderCopy(renderer, moonScared, NULL, NULL);
		currentTime += mTimer->GetDeltaTime();
		
		if (currentTime >= timeToBeSad) {
			scared = false;
			currentTime = 0;
		}
	}
	else {
		SDL_RenderCopy(renderer, moonSmug, NULL, NULL);
	}
	//show the final render
	SDL_RenderPresent(renderer);

	//Clear the renderer
	SDL_RenderClear(renderer);
}

bool InitWindow() {

	bool retflag = true;
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

	Mix_Chunk* sound = Mix_LoadWAV("sound/sadGuitar.wav");

	Mix_PlayChannel(-1, sound, 0);
}