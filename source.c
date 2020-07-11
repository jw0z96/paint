#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <math.h>

//DRAW POINT IN AREA
void drawPoint(Uint32 *pixels, int x, int y, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, Uint32 c)
{
	if (x > 10 && x < 11+IMG_WIDTH)
	{
		if (y > 110 && y < SCREEN_HEIGHT-10)
		{
			pixels[y * SCREEN_WIDTH + x] = c;
		}
	}
	if (x >= 324 && x < 394)
	{
		if (y >= 20 && y < 90)
		{
			pixels[y * SCREEN_WIDTH + x] = c;
		}
	}	
}

//SQUARE BRUSH
void pencilTool(Uint32 *pixels, int x, int y, int radius, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, Uint32 clr)
{
	for(int i = x-radius; i < x+radius; i++)
	{
		for(int j = y-radius; j < y+radius; j++)
		{
			drawPoint(pixels, i, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, clr);
		}
	}
}

//BRUSH TOOL
void brushTool(Uint32 *pixels, int x, int y, int radius, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, Uint32 clr)
{
	for(int i = 0; i < radius; i++ )
	{
		int j = (int) sqrt((double) (radius * radius) - (i * i));
		for(int fill = 0; fill < j; fill++)
		{
			drawPoint(pixels, x + fill, y + i, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, clr);
			if(i > 0)
				drawPoint(pixels, x + fill, y - i, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, clr);
			if(fill > 0)
			{
				drawPoint(pixels, x - fill, y + i, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, clr);
				if(i > 0)
					drawPoint(pixels, x - fill, y - i, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, clr);
			}
		}
	}
}

//ITERATIVE FLOOD FILL WHERE SRC IS THE COLOUR OF THE PIXEL THE USER CLICKED ON, AND DST IS THE COLOUR THEY WANT TO FILL WITH
void fill(Uint32 *pixels, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, int x,int y,Uint32 src, Uint32 dst)
{
	//SET EMPTY QUEUE
	int * queue = (int*)malloc(sizeof(int)*(SCREEN_WIDTH * SCREEN_HEIGHT));
	memset(queue, -1, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int));
	
	if(src != pixels[(y*SCREEN_WIDTH)+x]) return;
	
	//APPEND CURRENT POSITION TO QUEUE
	queue[0] = (SCREEN_WIDTH*y)+x;
	
	Uint32 index, i, j, counter;
	
	index = 0;
	counter = 1;
	
	while(1)
	{
		if(queue[index] == -1) break;
		
		i = queue[index]%SCREEN_WIDTH;		//finding the x coordinate from the integer position
		j = (queue[index]-1)/SCREEN_WIDTH;	//finding the y coordinate from the integer position
		
		drawPoint(pixels, i, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, dst);	//draw a point at the current position
		
		if(((i+1>11) && (i-1< IMG_WIDTH+10)) && ((j+1>111) && (j-1< IMG_HEIGHT+111)))
		{
			if(pixels[(j*SCREEN_WIDTH)+(i-1)] == src)
			{ 
				queue[counter] = (SCREEN_WIDTH*j)+(i-1);	//append x-1 to QUEUE
				drawPoint(pixels, i-1, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, dst);
				counter++;	//increment queue index counter
			}
			if(pixels[(j*SCREEN_WIDTH)+(i+1)] == src)
			{
				queue[counter] = (SCREEN_WIDTH*j)+(i+1);	//append x+1 to QUEUE
				drawPoint(pixels, i+1, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, dst);
				counter++;
			}
			if(pixels[((j-1)*SCREEN_WIDTH)+i] == src)
			{
				queue[counter] = (SCREEN_WIDTH*(j-1))+(i);//append y+1 to QUEU
				drawPoint(pixels, i, j-1, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, dst);
				counter++;
			}
			if(pixels[((j+1)*SCREEN_WIDTH)+i] == src)
			{
				queue[counter] = (SCREEN_WIDTH*(j+1))+(i);	//append y-1 to QUEUE
				drawPoint(pixels, i, j+1, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, dst);
				counter++;
			}
		}
		index++;
	}
	free(queue);
}

//LINE TOOL
/*	COPIED CODE FROM EIKE ANDERSON WEEK 20 LECTURE SLIDES STARTS HERE	*/
void line_tool_new(Uint32 *pixels, int radius, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, int x0, int y0, int x1, int y1, Uint32 c)
{
	int dx = abs(x1-x0); 
	int sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0); 
	int sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;
 
	while(1)
	{
		brushTool(pixels, x0, y0, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) 
		{ 
			err -= dy; x0 += sx; 
		}
		if (e2 < dy) 
		{
			err += dx; y0 += sy; 
		}
	}
}
/*	COPIED CODE FROM EIKE ANDERSON WEEK 20 LECTURE SLIDES ENDS HERE	*/

//ELLIPSE (BRESENHAM) 
/* CODE FROM http://members.chello.at/easyfilter/bresenham.html STARTS HERE */
void ellipseToolOutlined(Uint32 *pixels, int radius, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, int x0, int y0, int x1, int y1, Uint32 c)
{
	long a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; 
	long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; 
	long err = dx+dy+b1*a*a, e2; 

	if (x0 > x1) 
	{
		 x0 = x1; x1 += a; 
	}  
	
	if (y0 > y1) y0 = y1; 
	y0 += (b+1)/2; y1 = y0-b1;   
	a *= 8*a; b1 = 8*b*b;

	while (x0 <= x1)
	{
		brushTool(pixels, x1, y0, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		brushTool(pixels, x0, y0, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		brushTool(pixels, x0, y1, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		brushTool(pixels, x1, y1, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);

		e2 = 2*err;
		if (e2 <= dy) { y0++; y1--; err += dy += a; }  
		if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } 
	} 
}
/* CODE FROM http://members.chello.at/easyfilter/bresenham.html ENDS HERE */

//ELLIPSE (BRESENHAM) MODIFIED FROM THE PREVIOUS FUNCTION
void ellipseToolFilled(Uint32 *pixels, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, int x0, int y0, int x1, int y1, Uint32 c)
{
	long width = abs(x1-x0), height = abs(y1-y0), b1 = height&1; 
	long dx = 4*(1-width)*height*height, dy = 4*(b1+1)*width*width; 
	long err = dx+dy+b1*width*width, e2; 

	if (x0 > x1) 
	{
		 x0 = x1; x1 += width; 
	} 
	
	if (y0 > y1) y0 = y1; 
	y0 += (height+1)/2; y1 = y0-b1;  
	width *= 8*width; b1 = 8*height*height;
	
	while (x0 <= x1)
	{
		for(int i = x0; i<=x1; i++)
		{
			drawPoint(pixels, i, y0, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		}
		for(int j = x0; j<=x1; j++)
		{
			drawPoint(pixels, j, y1, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		}
		
		e2 = 2*err;
		
		if (e2 <= dy) 
		{ 
			y0++; y1--; 
			err += dy += width; 
		} 
		
		if (e2 >= dx || 2*err > dy) 
		{ 
			x0++; x1--; 
			err += dx += b1; 
		}
	} 
}

//BOX
void boxTool(Uint32 *pixels, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, int startX, int startY, int x, int y, Uint32 c)
{
	if(startX<x)
	{
		for(int i = startX; i<x; i++)
		{
			if(startY<y)
			{
				for(int j = startY; j<y; j++)
					drawPoint(pixels, i, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			}
			else
			{
				for(int j = y; j<startY; j++)
				{
					drawPoint(pixels, i, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
				}
			}
		}
	}
	else
	{
		for(int i = x; i<startX; i++)
		{
			if(startY<y)
			{
				for(int j = startY; j<y; j++)
					drawPoint(pixels, i, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			}
			else
			{
				for(int j = y; j<startY; j++)
					drawPoint(pixels, i, j, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			}
		}
	}
}

//BOX OUTLINE
void boxOutlineTool(Uint32 *pixels, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, int radius, int startX, int startY, int x, int y, Uint32 c)
{
	if(startX<x)
	{
		for(int i = startX; i<x; i++)
		{
			brushTool(pixels, i, startY, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			brushTool(pixels, i, y, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		}
		if(startY<y)
		{
			for(int j = startY; j<y; j++)
			{
				brushTool(pixels, startX, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
				brushTool(pixels, x, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			}
		}
		else if(startY>y)
		{
			for(int j = y; j<startY; j++)
			{
				brushTool(pixels, startX, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
				brushTool(pixels, x, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			}
		}
	}
	else
	{
		for(int i = x; i<startX; i++)
		{
			brushTool(pixels, i, startY, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			brushTool(pixels, i, y, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
		}
		if(startY<y)
		{
			for(int j = startY; j<y; j++)
			{
				brushTool(pixels, startX, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
				brushTool(pixels, x, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			}
		}
		else if(startY>y)
		{
			for(int j = y; j<startY; j++)
			{
				brushTool(pixels, startX, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
				brushTool(pixels, x, j, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			}
		}
	}
}

//BRUSH HUD
void toolUI(Uint32 *pixels, int tool, int radius, int IMG_WIDTH, int IMG_HEIGHT, int SCREEN_WIDTH, int SCREEN_HEIGHT, Uint32 c, Uint32 erasure)
{
	if(radius>31)	//ensure radius doesnt exceed viewport size
	{
		radius = 31;
	}
	if (tool == 2 || tool == 4 || tool == 9)	//set these tools to filled squares
	{
		radius = 31;
		tool = 0;
	}
	if (tool == 5)	//set to large circle
	{
		radius = 29;
		tool = 1;
	}
	memset(pixels, 255, SCREEN_WIDTH * 100 * sizeof(Uint32));

	switch(tool)
	{
		case 0:
			pencilTool(pixels,359, 54,radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			break;
		case 1:
			brushTool(pixels,359, 54,radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			break;
		case 3:
		case 6:
		case 7:
			for(int i=0;i<=20;i++)	//draw diagonal line
				pencilTool(pixels,349+i,44+i,2, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, c);
			break;
	}

}

//UI BACKDROP AND SHADOWS
void fancyTing(SDL_Renderer *renderer, int width, int height, int IMG_WIDTH, int IMG_HEIGHT)
{
	SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
	for(int i = 0; i < height; i = i + 2 )
	{
		for(int j = i%4; j < width; j = j + 4)
		{
			SDL_RenderDrawPoint(renderer,j,i);
			SDL_RenderDrawPoint(renderer,j+1,i);
			SDL_RenderDrawPoint(renderer,j,i+1);
			SDL_RenderDrawPoint(renderer,j+1,i+1);
		}
	}
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
	SDL_Rect drop = {15, 115, IMG_WIDTH+2, IMG_HEIGHT+2};
	SDL_RenderFillRect(renderer, &drop);
	
	SDL_Rect dropUI = {15, 15, 780, 90};
	SDL_RenderFillRect(renderer, &dropUI);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect Canvas = {10, 110, IMG_WIDTH+2, IMG_HEIGHT+2};
	SDL_RenderFillRect(renderer, &Canvas);
}

//MAIN FUNCTION
int viewPort(char *filePath, int IMG_WIDTH, int IMG_HEIGHT)
{
	bool quit = false;
	bool leftMouseButtonDown = false;
	
	SDL_Event event;

	SDL_Init(SDL_INIT_EVERYTHING);
	
	int startX, startY;
	
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT = IMG_HEIGHT+122;
	
	if(IMG_WIDTH<778)
		SCREEN_WIDTH = 800;
	else
		SCREEN_WIDTH = IMG_WIDTH+22;
	
	SDL_Window * screen = SDL_CreateWindow("Paint Program",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	
	SDL_Renderer * renderer = SDL_CreateRenderer(screen, -1, 0);   
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	SDL_Texture * canvasTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	Uint32 * canvasPixels = (Uint32*)malloc(sizeof(Uint32)*(SCREEN_WIDTH * SCREEN_HEIGHT));
	memset(canvasPixels, 16777215, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
	
	Uint32 * banding = (Uint32*)malloc(sizeof(Uint32)*(SCREEN_WIDTH * SCREEN_HEIGHT));	//EMPTY ARRAY FOR RUBBER BANDING
	
	fancyTing(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, IMG_WIDTH, IMG_HEIGHT);
	
	SDL_Rect canvasPos = {11,111, IMG_WIDTH, IMG_HEIGHT};	//CANVAS POSITION
	
	SDL_Surface * image1 = SDL_LoadBMP("assets/palette_full.bmp");
	SDL_Surface * image = SDL_ConvertSurfaceFormat(image1, SDL_PIXELFORMAT_RGB888, 0);
	SDL_FreeSurface(image1);
	Uint32 *palettePixels = (Uint32 *)image->pixels;
	
	SDL_Texture * paletteTex = SDL_CreateTextureFromSurface(renderer, image);
	SDL_Rect imgpl = {10, 10, 780, 90};	//POS OF UI
	SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
	
	SDL_Rect brushUI = {328, 24, 62, 60}; //POS OF BRUSH HUD
	
	char new[3] = "NEW";
	
	if(strcmp(filePath, new) != 0)	//IF USER HAS SPECIFIED A FILE TO LOAD
	{
		SDL_Surface * hlp = SDL_CreateRGBSurfaceFrom(canvasPixels, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SCREEN_WIDTH*sizeof(Uint32), 0, 0, 0, 0);	//EMPTY HELPER SURFACE
		SDL_Surface * bgimg = IMG_Load(filePath);	//LOAD THE IMAGE AS A SURFACE
		SDL_BlitScaled(bgimg, NULL, hlp, &canvasPos);	//BLIT IT INTO THE CANVAS AREA ONTO THE HELPER SURFACE
		Uint32 *imgpixels = (Uint32 *)hlp->pixels;	//CREATE AN ARRAY FROM THE HELPER SURFACE
		canvasPixels = imgpixels;	//ASSIGN CANVAS PIXELS TO IMGPIXELS
		SDL_FreeSurface(bgimg);	//FREE BOTH SURFACES
		SDL_FreeSurface(hlp);
	}
	
	int radius = 10;
	int currentTool = 1;
	Uint32 erasure = 16777215;
	Uint32 blc = 0;
	Uint32 currentClr = 0;
	toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);

	SDL_RenderPresent(renderer);
	
	while (!quit)
	{
		SDL_UpdateTexture(canvasTexture, NULL, canvasPixels, SCREEN_WIDTH * sizeof(Uint32));
		SDL_WaitEvent(&event);
		switch (event.type)
		{
			case SDL_MOUSEWHEEL:
				if (event.wheel.y > 0)
					if (radius < 100)
						radius++;
						toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
				if (event.wheel.y < 0)
					if (radius > 1)
						radius--;
						toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					leftMouseButtonDown = true;
					if (event.motion.x > 10 && event.motion.x < 10+IMG_WIDTH)	
					{
						if (event.motion.y > 110 && event.motion.y < SCREEN_HEIGHT-10)
							if(currentTool == 2)	//FLOOD FILL
							{
								Uint32 src = canvasPixels[(event.motion.y*SCREEN_WIDTH)+event.motion.x];
								if(src != currentClr)
								{
									fill(canvasPixels, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, event.motion.x, event.motion.y, src, currentClr);
								}
							}
							if(currentTool == 3 || currentTool == 4 || currentTool == 5 || currentTool == 6 || currentTool == 7)	//LINE
							{
								startX = event.motion.x;
								startY = event.motion.y;
								memcpy(banding,canvasPixels,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
							}
						}
					if (event.motion.x > 404 && event.motion.x < 434)
					{
						if (event.motion.y > 20 && event.motion.y < 50)
						{
							currentTool = 0;	//SET PENCIL
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect pencilShadow = {404,20,30,30};
							SDL_RenderFillRect(renderer, &pencilShadow);							
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
						if (event.motion.y > 60 && event.motion.y < 90) 
						{
							currentTool = 1;	//SET BRUSH
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect brushShadow = {404,60,30,30};
							SDL_RenderFillRect(renderer, &brushShadow);	
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
					}
					if (event.motion.x > 444 && event.motion.x < 474)
					{
						if (event.motion.y > 20 && event.motion.y < 50)  //SET FLOOD FILL
						{
							currentTool = 2;
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect floodShadow = {444,20,30,30};
							SDL_RenderFillRect(renderer, &floodShadow);	
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}	
						if (event.motion.y > 60 && event.motion.y < 90)  //LINE
						{
							currentTool = 3;
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect lineShadow = {444,60,30,30};
							SDL_RenderFillRect(renderer, &lineShadow);	
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
					}
					if (event.motion.x > 484 && event.motion.x < 514)
					{
						if (event.motion.y > 20 && event.motion.y < 50)	//SQUARE
						{
							currentTool=4;
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect sqShadow = {484,20,30,30};
							SDL_RenderFillRect(renderer, &sqShadow);	
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
						if (event.motion.y > 60 && event.motion.y < 90)	//ELLIPSE
						{
							currentTool=5;
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect ellipseShadow = {484,60,30,30};
							SDL_RenderFillRect(renderer, &ellipseShadow);
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
					}
					if (event.motion.x > 524 && event.motion.x < 554)
					{
						if (event.motion.y > 20 && event.motion.y < 50)	//OUTLINED SQUARE
						{
							currentTool=6;
							radius=3;
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect sqShadow = {524,20,30,30};
							SDL_RenderFillRect(renderer, &sqShadow);	
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
						if (event.motion.y > 60 && event.motion.y < 90)	//OUTLINED ELLIPSE
						{
							currentTool=7;
							radius=3;
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect ellipseShadow = {524,60,30,30};
							SDL_RenderFillRect(renderer, &ellipseShadow);
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
					}
					if (event.motion.x > 564 && event.motion.x < 594)
					{
						if (event.motion.y > 20 && event.motion.y < 50)  //SET ERASER
						{
							currentTool = 8;	//SET PENCILERASER
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect pencilShadow = {564,20,30,30};
							SDL_RenderFillRect(renderer, &pencilShadow);	
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
						if (event.motion.y > 60 && event.motion.y < 90)  //CLEAR CANVAS
						{
							memset(canvasPixels, 16777215, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
						}
					}
					if (event.motion.x > 604 && event.motion.x < 634)
					{
						if (event.motion.y > 20 && event.motion.y < 50)  //LOAD
						{
							SDL_HideWindow(screen);
							free(canvasPixels);;
							SDL_FreeSurface(image);
							SDL_DestroyTexture(paletteTex);
							SDL_DestroyTexture(canvasTexture);
							SDL_DestroyRenderer(renderer);
							SDL_DestroyWindow(screen);
							SDL_Quit();
							return 1;
						}
						if (event.motion.y > 60 && event.motion.y < 90)  //NEW
						{
							SDL_HideWindow(screen);
							memset(canvasPixels, 16777215, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
							free(canvasPixels);
							SDL_FreeSurface(image);
							SDL_DestroyTexture(paletteTex);
							SDL_DestroyTexture(canvasTexture);
							SDL_DestroyRenderer(renderer);
							SDL_DestroyWindow(screen);
							SDL_Quit();
							return 2;
						}
					}
					if (event.motion.x > 644 && event.motion.x < 674)
					{
						if (event.motion.y > 20 && event.motion.y < 50)  //SAVE
						{
							leftMouseButtonDown = false;
							char fileName[10];
							SDL_HideWindow(screen);
							printf("\nEnter a file name, including extension .bmp or .png:\n");
							scanf("%s", fileName);
							fflush(stdin);
							SDL_Surface * hlp = SDL_CreateRGBSurfaceFrom(canvasPixels, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SCREEN_WIDTH*sizeof(Uint32), 0, 0, 0, 0);
							SDL_Surface * save = SDL_CreateRGBSurface(0, IMG_WIDTH, IMG_HEIGHT, 32, 0, 0, 0, 0);
							SDL_BlitScaled(hlp, &canvasPos, save, NULL);
							int work = SDL_SaveBMP(save,fileName);
							SDL_ShowWindow(screen);
							SDL_HideWindow(screen);
							if(work == 0){
								printf("\nSaved succesfully!\n");
							}
							else if(work != 0){
								printf("\nsomething went wrong\n");
							}
							SDL_FreeSurface(hlp);
							SDL_FreeSurface(save);
							SDL_ShowWindow(screen);
							fancyTing(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, IMG_WIDTH, IMG_HEIGHT);
							SDL_RenderCopy(renderer, paletteTex, NULL, &imgpl);
							SDL_RenderCopy(renderer, canvasTexture, &brushUI, &brushUI);
							SDL_RenderCopy(renderer, canvasTexture, &canvasPos, &canvasPos);
						}
						if (event.motion.y > 60 && event.motion.y < 90)  //SPICE
						{							
							currentTool=9;
							SDL_RenderCopy(renderer,paletteTex, NULL, &imgpl);
							SDL_Rect clrPicker = {644,60,30,30};
							SDL_RenderFillRect(renderer, &clrPicker);
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
							
						}
					}
				}
			case SDL_MOUSEMOTION:
				if (leftMouseButtonDown)
				{
					if (event.motion.x > 14 && event.motion.x < 294)
					{
						if (event.motion.y > 14 && event.motion.y < 94)
						{
							currentClr = palettePixels[((event.motion.y-10)*image->w)+(event.motion.x-10)];
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
					}
					if (event.motion.x > 301 && event.motion.x < 310)
					{
						if (event.motion.y > 14 && event.motion.y < 94)
						{
							currentClr = palettePixels[((event.motion.y-10)*image->w)+(event.motion.x-10)];
							toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
						}
					}
					if (event.motion.x > 10 && event.motion.x < 10+IMG_WIDTH)
					{
						if (event.motion.y > 110 && event.motion.y < SCREEN_HEIGHT-10)
							switch(currentTool)
							{
								case 0:
									pencilTool(canvasPixels,event.motion.x, event.motion.y,radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr);
									break;
								case 1:
									brushTool(canvasPixels,event.motion.x, event.motion.y,radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr);
									break;
								case 2:
									break;
								case 3:
									memcpy(canvasPixels,banding,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
									line_tool_new(canvasPixels, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, startX, startY, event.motion.x, event.motion.y, currentClr);	
									break;
								case 4:
									memcpy(canvasPixels,banding,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
									boxTool(canvasPixels, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, startX, startY, event.motion.x, event.motion.y, currentClr);	
									break;
								case 5:
									memcpy(canvasPixels,banding,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
									ellipseToolFilled(canvasPixels, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, startX, startY, event.motion.x, event.motion.y, currentClr);	
									break;
								case 6:
									memcpy(canvasPixels,banding,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
									boxOutlineTool(canvasPixels, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, radius, startX, startY, event.motion.x, event.motion.y, currentClr);
									break;
								case 7:
									memcpy(canvasPixels,banding,SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
									ellipseToolOutlined(canvasPixels, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, startX, startY, event.motion.x, event.motion.y, currentClr);	
									break;
								case 8:
									pencilTool(canvasPixels,event.motion.x, event.motion.y,radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, erasure);
									break;
								case 9:
									currentClr = canvasPixels[(event.motion.y*SCREEN_WIDTH)+event.motion.x];
									break;
							}
						}
						toolUI(canvasPixels, currentTool, radius, IMG_WIDTH, IMG_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, currentClr, erasure);
					}
				break;
			case SDL_QUIT:
				quit = true;
				break;
		}
		SDL_RenderCopy(renderer, canvasTexture, &brushUI, &brushUI);
		SDL_RenderCopy(renderer, canvasTexture, &canvasPos, &canvasPos);
		SDL_RenderPresent(renderer);
	}
	free(canvasPixels);
	SDL_FreeSurface(image);
	SDL_DestroyTexture(paletteTex);
	SDL_DestroyTexture(canvasTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	SDL_Quit();
	return 0;
}

//MAIN FUNCTION HOLDS ALL USER INPUT INTO TERMINAL
int main(void)
{
	char new[3] = "NEW";
	char filePath[30];
	int width, height, progress;
		
	while(1)
	{
		printf("Specify a bmp or png file to open, type 'NEW' to create a new blank canvas, or 'q' to quit:");
		scanf("%s", filePath);
		fflush(stdin);
		
		if(filePath[0]=='q')
		{
			printf("\nGoodbye!\n");
			return 0;
		}
		
		else if(strcmp(filePath, new) != 0)
		{
			SDL_Surface * bgimg = IMG_Load(filePath);
			if(bgimg != NULL)
			{
				progress = viewPort(filePath, bgimg->w, bgimg->h);
				break;
			}
			else
			{
				printf("\nImage '%s' does not exist or is not a bitmap or png \n", filePath);
			}
			SDL_FreeSurface(bgimg);
		}
		else if(strcmp(filePath, new) == 0)
		{
			printf("Specify canvas width:");
			scanf("%d", &width);
			fflush(stdin);
			printf("Specify canvas height:");
			scanf("%d", &height);
			fflush(stdin);
			progress = viewPort(filePath, width, height);
			break;
		}
	}
	while(progress !=0)
	{
		while(progress == 1)
		{
			printf("Specify a file to load:");
			scanf("%s", filePath);
			fflush(stdin);
			SDL_Surface * bgimg = IMG_Load(filePath);
			if(bgimg != NULL)
			{
				progress = viewPort(filePath, bgimg->w, bgimg->h);
			}
			else
			{
				printf("\nImage '%s' does not exist or is not a bitmap or png \n", filePath);
			}
			SDL_FreeSurface(bgimg);
		}
		while(progress == 2)
		{
			strcpy(filePath, new);
			printf("Specify canvas width:");
			scanf("%d", &width);
			fflush(stdin);
			printf("Specify canvas height:");
			scanf("%d", &height);
			fflush(stdin);
			progress = viewPort(filePath, width, height);
		}
		if(progress == -1)
		{
			printf("something went wrong");
			break;
		}
	}
	return 0;
}
