/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_ttf.h>
#include <string>
#include <SDL_image.h>
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const unsigned int frameRate = 60;
const enum DIRECTION { RIGHT, DOWN, UP, LEFT };

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

SDL_Renderer* renderer;
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The texture we will load and show on the screen
TTF_Font *gFont = NULL, *goFont = NULL;



class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font *font);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
LTexture gScoreTexture, gOTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font *font)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			if (TTF_Init() == -1) {
				printf("TTF_init :%s\n", TTF_GetError());
				exit(2);
			}
			else {
				//Get window surface
				gScreenSurface = SDL_GetWindowSurface(gWindow);
				renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load splash image
	gFont = TTF_OpenFont("../Assets/Fonts/lazy.ttf", 12);
	goFont = TTF_OpenFont("../Assets/Fonts/lazy.ttf", 28);
	if (gFont == NULL)
	{
		printf("Unable to load font %s! SDL Error: %s\n", "lazy.ttf", SDL_GetError());
		success = false;
	}
	else {
		SDL_Color textColor = { 0,255,0 };
		int score = 0;
		std::string text = "Score: " + std::to_string(score);
		printf("Score %d" , 0);
		if (!gScoreTexture.loadFromRenderedText(text, textColor,gFont)||!gOTexture.loadFromRenderedText("GameOver", textColor,goFont))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}

	}

	return success;
}

int random(int max) {

	int number = rand()  %max;

	
	return (number / 10) * 10;

}

void bound(SDL_Rect &head) {
	if (head.x > SCREEN_WIDTH) 
		head.x = 0;	
	if (head.x < 0)
		head.x = SCREEN_WIDTH - 10;
	if (head.y > SCREEN_HEIGHT) 
		head.y = 0;	
	if (head.y < 0)
		head.y = SCREEN_HEIGHT - 10;

}
void close()
{
	//Free loaded images
	gScoreTexture.free();

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window    
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			int tickTime = SDL_GetTicks();
			int lastKey = SDL_GetTicks();
			int length = 2;
			int velocity = RIGHT;
			bool GAMEOVER = false;
			SDL_Color textColor = { 0,255,0 };
			int score = 0;
			SDL_Rect rect;

			SDL_Rect pickup;
			pickup.x = 50;
			pickup.y = 40;
			pickup.w = 10;
			pickup.h = 10;

			SDL_Rect* body = new SDL_Rect[1000];
			rect.x = 0;
			rect.y = 0;
			rect.w = 10;
			rect.h = 10;
			bool quit = false;
		
			SDL_Event e;

			while (!quit) {

				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type = SDL_KEYDOWN) {
						if (SDL_GetTicks() > lastKey) {
							lastKey = SDL_GetTicks();
							switch (e.key.keysym.sym) {
							case SDLK_UP:
								velocity = UP;
								break;
							case SDLK_DOWN:
								velocity = DOWN;
								break;
							case SDLK_RIGHT:
								velocity = RIGHT;
								break;
							case SDLK_LEFT:
								velocity = LEFT;
								break;
							case SDLK_SPACE:
								if (GAMEOVER) {
									tickTime = SDL_GetTicks();
									lastKey = SDL_GetTicks();
									length = 2;
									velocity = RIGHT;
									GAMEOVER = false;

									score = 0;

									pickup.x = 50;
									pickup.y = 40;
									pickup.w = 10;
									pickup.h = 10;

									body = new SDL_Rect[1000];
									rect.x = 0;
									rect.y = 0;
									rect.w = 10;
									rect.h = 10;
								}
								break;

							default:
								break;
							}
						}
					}
				}
				//Apply the image
				//SDL_BlitScaled(gHelloWorld, NULL, gScreenSurface, &rect);

				if (SDL_GetTicks() > tickTime + frameRate) {
					tickTime = SDL_GetTicks();
					body[0] = rect;
					for (int i = length; i > 0; i--) {
						if (rect.x == body[i].x && rect.y == body[i].y)
							GAMEOVER = true;
						if (rect.x == pickup.x && rect.y == pickup.y) {
							pickup.x = random(SCREEN_WIDTH);
							pickup.y = random(SCREEN_HEIGHT);
							score++;
							printf("New pick up location (%d ,%d)", pickup.x, pickup.y);
							length++;
						}
						body[i] = body[i - 1];
					}
					if (!GAMEOVER) {
						switch (velocity) {
						case RIGHT:
							rect.x += 10;
							break;
						case UP:
							rect.y -= 10;
							break;
						case DOWN:
							rect.y += 10;
							break;
						case LEFT:
							rect.x -= 10;
							break;
						default:
							printf("Whoops velocity is wrong");
						}
						bound(rect);
						//clear screen
						SDL_SetRenderDrawColor(renderer, 0, 0,0, 255);
						SDL_RenderClear(renderer);
						//draw ead
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
						SDL_RenderFillRect(renderer, &rect);
						//draw body
						SDL_RenderFillRects(renderer, body, length);
						//draw pickup
						SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
						SDL_RenderFillRect(renderer, &pickup);
						std::string  text = "Score: " + std::to_string(score);
						
						gScoreTexture.loadFromRenderedText(text, textColor,gFont);
						
						gScoreTexture.render(0, 0);
						//update 
						
					}
					else {
						gOTexture.loadFromRenderedText("Game Over", textColor, goFont);
						gOTexture.render((SCREEN_WIDTH - gOTexture.getWidth()) / 2, (SCREEN_HEIGHT - gOTexture.getHeight()) / 2);
					}
					SDL_RenderPresent(renderer);
					//Update the surface
					//SDL_UpdateWindowSurface(gWindow);

				}
				//Wait two seconds

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}