#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#define GAME_STATE 		0b00
#define MENU_STATE 		0b01
#define PAUSED_STATE 	0b10
#define CREDIT_STATE 	0b11


using namespace std;

const int WIDTH  = 800;
const int HEIGHT = 600;

int STATE = MENU_STATE;

SDL_Renderer * gRenderer = NULL;
SDL_Window * gWindow = NULL;
SDL_Event gEvent;
bool running = false;


// walls
SDL_Rect walls[4];
SDL_Rect player[2];

SDL_Rect play,exitRect;

int playerSpeed = 2;

bool checkCollision(SDL_Rect a,SDL_Rect b);


class Ball
{

	public:
		int WIDTH = 20;
		int HEIGHT = 20;

		Ball();
		
		void move();
		void render()
		{
			SDL_Rect rect = {(int)mPosX,(int)mPosY,WIDTH,HEIGHT};
			SDL_SetRenderDrawColor(gRenderer,0xcc,0xcc,0xcc,0xff);
			SDL_RenderFillRect(gRenderer,&rect);
		}

		float mPosX,mPosY;
		float mVelX,mVelY;
		SDL_Rect mCollider;
};

Ball::Ball()
{
	mPosY = 280;
	mPosX = 480;
	mVelX = 4;
	mVelY = 1;

	// set collision box dimensions
	mCollider.w = WIDTH;
	mCollider.h = HEIGHT;
	mCollider.x = (int)mPosX;
	mCollider.y = (int)mPosY;
}


void Ball::move()
{
	this->mPosX += this->mVelX;
	this->mCollider.x = (int)this->mPosX;
	this->mPosY += this->mVelY;
	this->mCollider.y = (int)this->mPosY;
}

bool checkCollision(SDL_Rect a,SDL_Rect b)
{
	int leftA, leftB;
	int rightA,rightB;
	int topA,topB;
	int bottomA, bottomB;

	// calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	// calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y+ b.h;

	// if any of the sides from A are outside of B
	if (bottomA <= topB || topA >= bottomB)
		return false;
	if (rightA <= leftB || leftA >= rightB)
		return false;
	return true;
}

Ball ball;


void handleClicks()
{
	int x,y;
	SDL_GetMouseState(&x,&y);
	
	if(STATE == MENU_STATE)
	{
		// check is the play button was clicked
		if(x >= play.x && x <= play.x + play.w)
		{
			if(y >= play.y && y <= play.y + play.h)
			{
				STATE = GAME_STATE;
			}
		}

		// check if the exit button was clicked
		if(x >= exitRect.x && x <= exitRect.x + exitRect.w)
		{
			if(y >= exitRect.y && y <= exitRect.y + exitRect.h)
			{
				cout << "Exitting"<<endl;
				running = false;
			}
		}
	}

}

void renderBoard()
{

	SDL_SetRenderDrawColor(gRenderer,0xaa,0xaa,0xaa,0xff);	

	for(int i = 0; i< 4; i++)
	{
		SDL_RenderFillRect(gRenderer,&walls[i]);
	}

	// middle line
	SDL_Rect line;
	line.w = 20;
	line.h = HEIGHT;
	line.x = (WIDTH - line.w)/2;
	line.y = 0;
	SDL_RenderFillRect(gRenderer,&line);


}

void render(void)
{
	SDL_SetRenderDrawColor(gRenderer,0x0,0x0,0x0,0xff);	
	SDL_RenderClear(gRenderer);
	renderBoard();
	ball.render();
	SDL_RenderFillRect(gRenderer,&player[0]);
	SDL_RenderFillRect(gRenderer,&player[1]);

	SDL_RenderPresent(gRenderer);

}


void player1UP()
{
	if(player[1].y > 20)
		player[1].y -= playerSpeed;
}

void player1DOWN()
{
	if(player[1].y < HEIGHT - 20)
		player[1].y += playerSpeed;	
}


void handleEvents(void)
{
	while(SDL_PollEvent(&gEvent) != 0)
	{
		if(gEvent.type == SDL_QUIT)
			running = false;

		// mouse click event
		if(gEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			handleClicks();
		}

		if(gEvent.type == SDL_KEYDOWN)
		{
			if(gEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				STATE = MENU_STATE;
			}
		}

	}

	// fast way to get key input
	const Uint8 * keystate = SDL_GetKeyboardState(NULL);
	if(keystate[SDL_SCANCODE_UP])
		player1UP();
	if(keystate[SDL_SCANCODE_DOWN])
		player1DOWN();
}


void aiLogic()
{
		// ridiculous right?
		player[0].y = ball.mPosY - 30;
}

void update(void)
{
	for(int i = 0; i < 4; i++)
	{

		// check collision for the players
		// they are only two players so i placed this condition here
		if(i < 2)
		{
			if(checkCollision(ball.mCollider,player[i]))
			{
				ball.mVelX *= -1;
			}
		}


		if(checkCollision(ball.mCollider,walls[i]))
		{
			// if it hits the top or bottom wall: 0 - top, 1 - bottom
			if(i < 2)
			{
				ball.mVelY *= -1;
			}
			else{
				// it hits the sides;
				ball.mVelX *= -1;
			}
			break;
		}
	}
	ball.move();
	aiLogic();
}

int main()
{

	// Init SDL2
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cerr << "Unable to init SDL! ERROR: "<<SDL_GetError()<<endl;
		return -1;
	}

	// Create window
	SDL_Window * lWindow = SDL_CreateWindow("Pong",0,0,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
	SDL_Renderer * lRenderer = NULL;
	if(lWindow)
	{
		// Create Renderer
		lRenderer = SDL_CreateRenderer(lWindow,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(!lRenderer)
		{
			cerr << "Unable to create Renderer! ERROR: "<<SDL_GetError() <<endl;
			return -1;
		}
	}
	else{
		cerr << "Unable to create window! ERROR: "<<SDL_GetError()<<endl;
		return -1;
	}

	// Copy window and Renderer to global scope
	gWindow = lWindow;
	gRenderer = lRenderer;



	// define walls 

	// top
	walls[0].x = 0;
	walls[0].y = 0;
	walls[0].h = 20;
	walls[0].w = WIDTH;

	// bottom
	walls[1].x = 0;
	walls[1].y = HEIGHT - 20;
	walls[1].h = 20;
	walls[1].w = WIDTH;

	// left
	walls[2].x = 0;
	walls[2].y = 0;
	walls[2].h = HEIGHT;
	walls[2].w = 20;

	// right
	walls[3].x = WIDTH - 20;
	walls[3].y = 0;
	walls[3].h = HEIGHT;
	walls[3].w = 20;


	// define player rects
	player[0].w = player[1].w = 20;
	player[0].h = player[1].h = 60;

	player[0].x = 100;
	player[0].y = (HEIGHT - 60)/2;
	player[1].x = WIDTH - 100;
	player[1].y = player[0].y;

	running = true;


	// setting up menu buttons and labels
	SDL_Surface * image = IMG_Load("title.png");
	if(!image)
		printf("Unable to load image: %s\n",SDL_GetError());
	
	SDL_Texture * title = SDL_CreateTextureFromSurface(gRenderer,image);
	if(!title)
	{
		printf("unable to create texture from surface\n");
		return -1;
	}

	SDL_FreeSurface(image);
	image = IMG_Load("play.png");
	SDL_Texture * playTexture = SDL_CreateTextureFromSurface(gRenderer,image);
	

	SDL_FreeSurface(image);
	image = IMG_Load("exit.png");
	SDL_Texture * exitRectTexture = SDL_CreateTextureFromSurface(gRenderer,image);
	
	// SDL_FreeSurface(image);
	// image = IMG_Load("credits.png");
	// SDL_Texture * creditTexture = SDL_CreateTextureFromSurface(gRenderer,image);
	


	SDL_Rect dest;
	dest.w = 300;
	dest.h = 80;
	dest.x = (WIDTH - dest.w)/2;
	dest.y = -200+ (HEIGHT - dest.h)/2;

	play.w = 200;
	play.h = 40;
	play.x = (WIDTH - play.w)/2;
	play.y = (HEIGHT - play.h)/2;

	exitRect.w = 200;
	exitRect.h = 40;
	exitRect.x = (WIDTH - exitRect.w)/2;
	exitRect.y = 100+ (HEIGHT - exitRect.h)/2;


	while(running)
	{
		// Normal game player mode
		while(STATE == GAME_STATE && running)
		{
			handleEvents();
			update();
			render();
		}

		// menu
		while(STATE == MENU_STATE && running)
		{
			handleEvents();
			SDL_SetRenderDrawColor(gRenderer,0x0,0x0,0x0,0xff);
			SDL_RenderClear(gRenderer);
			SDL_RenderCopy(gRenderer,title,NULL,&dest);
			SDL_RenderCopy(gRenderer,playTexture,NULL,&play);
			SDL_RenderCopy(gRenderer,exitRectTexture,NULL,&exitRect);
			SDL_RenderPresent(gRenderer);

		}



		// while(STATE == CREDIT_STATE && running)
		// {
		// 	handleEvents();
		// 	SDL_RenderClear(gRenderer);
		// 	SDL_RenderCopy(gRenderer,credits,NULL,&dest);
		// 	SDL_RenderCopy(gRenderer,exitRectTexture,NULL,&exitRect);
		// 	SDL_RenderPresent(gRenderer);

		// }

	}
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}