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
void PlayAudio(string file);

SDL_Window* window = nullptr;

SDL_Renderer* renderer = nullptr;


//textures

SDL_Texture* normalFace = nullptr;
SDL_Texture* initSurprised = nullptr;
SDL_Texture* realiaiztion = nullptr;
SDL_Texture* sadOpenEyes = nullptr;
SDL_Texture* sadTalking = nullptr;
SDL_Texture* sadSqueentTalking = nullptr;
SDL_Texture* justSad = nullptr;
SDL_Texture* talkingHappy = nullptr;


Mix_Music* currentFX = nullptr;

Timer* mTimer = nullptr;

bool normal = true;
bool bInitSurprised = false;
bool realizing = false;
bool bSadOpenEyes = false;
bool bSadTalking = false;
bool bSadSqueentTalking = false;
bool bJustSad = false;
bool bTalkingHappy = false;
bool playedGuitar = false;
bool talkedHappily = false;


float currentTime{ 0.0 };
float timeToBeNormal{ 2.0 };
float timeToBeTalkingHappy{ 6.0 };
float timeToBeInitSurprised{ 5.0 };
float timeToBeSadOpenEyes{ 3.0 };
float timeToBeSadTalking{ 2.5 };
float timeToBeSadSqueentTalking{ 3.95 };

int main(int argc, char* args[]) {

	if (!Initialize()) {
		printf("Could not initialize.\n");
		return -1;
	}

	normalFace = LoadTexture("images/normal.png");
	if (normalFace == NULL) {
		printf("Couldn't load image.");
		return -1;
	}
	talkingHappy = LoadTexture("images/talkingHappy.png");
	if (talkingHappy == NULL) {
		printf("Couldn't load image.");
		return -1;
	}

	sadTalking = LoadTexture("images/sadTalking.png");
	if (sadTalking == NULL) {
		printf("Couldn't load image.");
		return -1;
	}
	sadSqueentTalking = LoadTexture("images/sadSqueentTalking.png");
	if (sadSqueentTalking == NULL) {
		printf("Couldn't load image.");
		return -1;
	}

	initSurprised = LoadTexture("images/initSurprise.png");
	if (initSurprised == NULL) {
		printf("Couldn't load image.");
		return -1;
	}

	realiaiztion = LoadTexture("images/realization.png");
	if (realiaiztion == NULL) {
		printf("Couldn't load image.");
		return -1;
	}

	sadOpenEyes = LoadTexture("images/sadOpenEyes.png");
	if (sadOpenEyes == NULL) {
		printf("Couldn't load image.");
		return -1;
	}

	justSad = LoadTexture("images/justSad.png");
	if (justSad == NULL) {
		printf("Couldn't load image.");
		return -1;
	}


	MakeWindowTransparent(window, RGB(0, 255, 0));

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

			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
				exit = true;
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				normal = false;
				if (!bInitSurprised) {
					bInitSurprised = true;
				}
				//then realization 
				mouseButtonHeld = true;
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				mouseButtonHeld = false;
				realizing = false;
				if (!bSadOpenEyes) {
					bSadOpenEyes = true;
				}
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

	currentTime = 0;

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


	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
		printf("Audio could not be opened. SDL_Error: %s\n", SDL_GetError());
	}


	mTimer = Timer::Instance();
	return true;
}

void FixedUpdate() {

	//cout << "init surprised: " << bInitSurprised << endl;
	//cout << "realizing: " << realizing << endl;
	//cout << "normal: " << normal << endl;
	if (normal) {
		SDL_RenderCopy(renderer, normalFace, NULL, NULL);
		currentTime += mTimer->GetDeltaTime();
		
		if (!talkedHappily) {
			if (currentTime >= timeToBeNormal) {
				normal = false;
				currentTime = 0;
				bTalkingHappy = true;
			}
		}
		
	}

	if (bTalkingHappy) {
		SDL_RenderCopy(renderer, talkingHappy, NULL, NULL);
		currentTime += mTimer->GetDeltaTime();
		PlayAudio("sound/happy.wav");

		if (currentTime >= timeToBeTalkingHappy) {
			bTalkingHappy = false;
			currentTime = 0;
			talkedHappily = true;
			normal = true;
		}
	}

	if (bInitSurprised) {
		SDL_RenderCopy(renderer, initSurprised, NULL, NULL);
		currentTime += mTimer->GetDeltaTime();

		if (currentTime >= timeToBeInitSurprised) {
			bInitSurprised = false;
			currentTime = 0;
			realizing = true;
		}
	}

	if (realizing) {
		SDL_RenderCopy(renderer, realiaiztion, NULL, NULL);
		currentTime = 0;
		//this stays like this until mouse gets held up
	}
	

	if (bSadOpenEyes) {
		SDL_RenderCopy(renderer, sadOpenEyes, NULL, NULL);

		currentTime += mTimer->GetDeltaTime();
		if (currentTime >= timeToBeSadOpenEyes) {
			bSadOpenEyes = false;
			currentTime = 0;
			bSadTalking = true;
			PlayAudio("sound/sad.wav");
			//start audio talking
		}
	}
	if (bSadTalking) {
		SDL_RenderCopy(renderer, sadTalking, NULL, NULL);

		currentTime += mTimer->GetDeltaTime();
		if (currentTime >= timeToBeSadTalking) {
			bSadTalking = false;
			currentTime = 0;
			bSadSqueentTalking = true;
			//start audio talking
		}

	}
	if (bSadSqueentTalking) {
		SDL_RenderCopy(renderer, sadSqueentTalking, NULL, NULL);
		currentTime += mTimer->GetDeltaTime();

		if (currentTime >= timeToBeSadSqueentTalking) {
			bSadSqueentTalking = false;
			currentTime = 0;
			bJustSad = true;
		}

	}
	if (bJustSad) {
		SDL_RenderCopy(renderer, justSad, NULL, NULL);
		
		if (!playedGuitar) {
 			PlayAudio("sound/sadGuitar.wav");
			playedGuitar = true;
		}
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
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);

	int imgFlags = IMG_INIT_PNG;

	if (!(IMG_Init(imgFlags) & imgFlags)) {

		printf("SDL image could not be initialized. SDL Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void Close() {

	SDL_DestroyWindow(window);
	SDL_DestroyTexture(initSurprised);
	SDL_DestroyTexture(realiaiztion);
	SDL_DestroyTexture(normalFace);
	SDL_DestroyTexture(sadTalking);
	SDL_DestroyTexture(sadOpenEyes);
	SDL_DestroyTexture(sadSqueentTalking);
	SDL_DestroyTexture(justSad);
	SDL_DestroyTexture(talkingHappy);
	SDL_DestroyRenderer(renderer);

	renderer = nullptr;
	window = nullptr;
	normalFace = nullptr;
	talkingHappy = nullptr;
	initSurprised = nullptr;
	realiaiztion = nullptr;
	sadOpenEyes = nullptr;
	sadTalking = nullptr;
	sadSqueentTalking = nullptr;
	justSad = nullptr;


	Mix_FreeMusic(currentFX);

	currentFX = nullptr;

	mTimer = nullptr;

	Mix_Quit();

	IMG_Quit();

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

void PlayAudio(std::string file) {

	if (!Mix_PlayingMusic()) {
		currentFX = Mix_LoadMUS(file.c_str());
		Mix_PlayMusic(currentFX, 0);
	}
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

