/*
===============================================================================
 Name        : DrawLine.c
 Author      : $Tarun Chawla
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>



/* Be careful with the port number and location number, because

some of the location may not exist in that port. */

#define PORT_NUM 0

#define LCD_DATA 21
#define LCD_RESET 22
#define LCD_CS 16

uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29



#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

// defining color values

#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x3366ff
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define ORANGE 0XFF4500
#define OLIVE 0X008000
#define CERE 0X156B49
#define BROWN 0X8B4513
#define WOOD 0XDEB887
#define SKY 0X000066
#define YELLOW 0Xffff00
#define PINK 0Xff0066

int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;


void spiwrite(uint8_t c)

{

 int pnum = 0;

 src_addr[0] = c;

 SSP_SSELToggle( pnum, 0 );

 SSPSend( pnum, (uint8_t *)src_addr, 1 );

 SSP_SSELToggle( pnum, 1 );

}



void writecommand(uint8_t c)

{

 LPC_GPIO0->FIOCLR |= (0x1<<LCD_DATA);

 spiwrite(c);

}



void writedata(uint8_t c)

{

 LPC_GPIO0->FIOSET |= (0x1<<LCD_DATA);

 spiwrite(c);

}



void writeword(uint16_t c)

{

 uint8_t d;

 d = c >> 8;

 writedata(d);

 d = c & 0xFF;

 writedata(d);

}



void write888(uint32_t color, uint32_t repeat)

{

 uint8_t red, green, blue;

 int i;

 red = (color >> 16);

 green = (color >> 8) & 0xFF;

 blue = color & 0xFF;

 for (i = 0; i< repeat; i++) {

  writedata(red);

  writedata(green);

  writedata(blue);

 }

}



void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)

{

 writecommand(ST7735_CASET);

 writeword(x0);

 writeword(x1);

 writecommand(ST7735_RASET);

 writeword(y0);

 writeword(y1);

}


void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

 int16_t i;

 int16_t width, height;

 width = x1-x0+1;

 height = y1-y0+1;

 setAddrWindow(x0,y0,x1,y1);

 writecommand(ST7735_RAMWR);

 write888(color,width*height);

}



void lcddelay(int ms)

{

 int count = 24000;
 int i;

 for ( i = count*ms; i--; i > 0);

}



void lcd_init()

{

 int i;
 printf("LCD Demo Begins!!!\n");
 // Set pins P0.LCD_CS, P0.LCD_DATA, P0.LCD_RESET as output
 LPC_GPIO0->FIODIR |= (0x1<<LCD_CS);

 LPC_GPIO0->FIODIR |= (0x1<<LCD_DATA);

 LPC_GPIO0->FIODIR |= (0x1<<LCD_RESET);

 // Hardware Reset Sequence
 LPC_GPIO0->FIOSET |= (0x1<<LCD_RESET);
 lcddelay(500);

 LPC_GPIO0->FIOCLR |= (0x1<<LCD_RESET);
 lcddelay(500);

 LPC_GPIO0->FIOSET |= (0x1<<LCD_RESET);
 lcddelay(500);

 // initialize buffers
 for ( i = 0; i < SSP_BUFSIZE; i++ )
 {

   src_addr[i] = 0;
   dest_addr[i] = 0;
 }

 // Take LCD display out of sleep mode
 writecommand(ST7735_SLPOUT);
 lcddelay(200);

 // Turn LCD display on
 writecommand(ST7735_DISPON);
 lcddelay(200);

}




void drawPixel(int16_t x, int16_t y, uint32_t color)

{

 if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))

 return;

 setAddrWindow(x, y, x + 1, y + 1);

 writecommand(ST7735_RAMWR);

 write888(color, 1);

}



/***************************


** Descriptions:        Draw line function

**

** parameters:           Starting point (x0,y0), Ending point(x1,y1) and color

** Returned value:        None

**

***************************/


void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

	int16_t slope = abs(y1 - y0) > abs(x1 - x0);

 if (slope) {

  swap(x0, y0);

  swap(x1, y1);

 }

 if (x0 > x1) {

  swap(x0, x1);

  swap(y0, y1);

 }

 int16_t dx, dy;

 dx = x1 - x0;

 dy = abs(y1 - y0);

 int16_t err = dx / 2;

 int16_t ystep;

 if (y0 < y1) {

  ystep = 1;

 }

 else {

  ystep = -1;

 }

 for (; x0 <= x1; x0++) {

  if (slope) {

   drawPixel(y0, x0, color);

  }

  else {

   drawPixel(x0, y0, color);

  }

  err -= dy;

  if (err < 0) {

   y0 += ystep;

   err += dx;

  }

 }

}



void draw_sqr(uint32_t color,float val_lam, int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4, int innersquares)
{

	int x [innersquares][4];
	int y [innersquares][4];


	// X Co-ordinates for the initial square
		 x[0][0] = x1;
		 x[0][1] = x2;
		 x[0][2] = x3;
		 x[0][3] = x4;
		 float lambda = val_lam;
		 for(int m=1; m<innersquares; m++)
		    {
		       for(int n=0;n<4;n++)
		       {
		    	   if(n>2)
		    	   {
		    		   x[m][n]= x[m-1][n]+ lambda*(x[m-1][0]-x[m-1][n]);
		    	   }
		    	   else
		    	   {
		    		   x[m][n]= x[m-1][n]+ lambda*(x[m-1][n+1]-x[m-1][n]);
		    	   }
		       }
		    }

	// Y Co-ordinates for the initial square
		     y[0][0] = y1;
		 	 y[0][1] = y2;
		 	 y[0][2] = y3;
		 	 y[0][3] = y4;

		 	 for(int m=1; m<innersquares; m++)
		 	    {
		 	       for(int n=0;n<4;n++)
		 	       {
		 	    	   if(n>2)
		 	    	   {
		 	    		   y[m][n]= y[m-1][n]+ lambda*(y[m-1][0]-y[m-1][n]);
		 	    	   }
		 	    	   else
		 	    	   {
		 	    		   y[m][n]= y[m-1][n]+ lambda*(y[m-1][n+1]-y[m-1][n]);
		 	    	   }
		 	       }
		 	    }

		 	for(int m=0; m<innersquares; m++) {
		 	      for(int n=0;n<4;n++) {
		 	    	  if(n>2)
		 	    	  {
		 	    		 drawLine(x[m][n],y[m][n],x[m][0],y[m][0],color);
		 	    	  }
		 	    	  else
		 	    	  {
		 	    		 drawLine(x[m][n],y[m][n],x[m][n+1],y[m][n+1],color);
		 	    	  }

		 	         }
		 	      }
		 	   }

rotate_point(int *x2,int *y2,int x1, int y1,int angle)
{int x_temp_diff,y_temp_diff,xro,yro;
float cosine,sine,ra;
ra = angle*(3.14159265/180);
x_temp_diff = *x2 -x1;
y_temp_diff = *y2 -y1;
cosine = cos(ra);
sine = sin(ra);
xro = x_temp_diff * cosine -y_temp_diff * sine;
yro = x_temp_diff * sine +y_temp_diff * cosine;
*x2=xro+x1;
*y2=yro+y1;
return;
}


void draw_screensaver(int no_of_colours){

	 int a1,a2,a4,a3,b4,b1,b2,b3,l;
    float alpha;
	 	uint32_t colour;
	    for(int g=0;g<no_of_colours;g++)
	    {
	 	 if(g==0)
	 	 {
	 		 colour=ORANGE;
	 		 a3=50+rand()%20;
	 	     b3=30+rand()%30;
	 		 l=80;
	 		 alpha=0.2;
	 	 }
	 	 if(g==1)
	 	 {
	 		 colour=RED;
	 		 a3=60+rand()%20;
	 		 b3=125+rand()%20;
	 		 l=140;
	 		 alpha=0.8;
	 	 }
	 	 if(g==2)
	 	 {
	 		 colour=YELLOW;
	 		 a3=75+rand()%20;
	 		 b3=130+rand()%20;
	 		 l=50;
	 		 alpha=0.2;
	      }
	 	 if(g==3)
	 	 {
	 		 colour=LIGHTBLUE;
	 		 a3=110+rand()%20;
	 		 b3=85+rand()%20;
	 		 l=120;
	 		 alpha=0.8;
	 	 }
	 	 if(g==4)
	 	 {
	 		 colour=GREEN;
	 		 a3= 60+rand()%20;
	 		 b3= 95+rand()%20;
	 		 l= 80;
	 		 alpha=0.2;
	 	 }
	 	 if(g==5)
	 	 {
	 		colour=PURPLE;
	 		a3= 98+rand()%20;
	 		b3= 44+rand()%20;
	    	l= 45;
	    	alpha=0.8;
	 	 }
	 	 if(g==6)
	 	 {
	 		colour=RED;
	 		a3= 10+rand()%20;
	 		b3= 62+rand()%20;
	 		l= 25;
	 		alpha=0.2;
	 	  }
	 	 if(g==7)
	 	 {
	 		colour=MAGENTA;
	 		a3= 60+rand()%20;
	 		b3= 110+rand()%20;
	 		l= 50;
	 		alpha=0.8;
	 	 }
	 	 if(g==8)
	 	  {
	 		colour=BLACK;
	 		a3= 115+rand()%20;
	 		b3= 75+rand()%20;
	 		 l= 30;
	 		 alpha=0.2;
	 	  }
	 	 if(g==9)
	 	  {
	 		colour=WHITE;//
	 		a3= 70+rand()%20;
	 		b3= 65+rand()%30;
	 		l= 80;
	 		alpha=0.8;
	 	   }

	 	 a4 = a3-l;
	 	 b4 = b3;
	 	 a2 = a3;
	 	 b2 = b3-l;
	 	 a1 = a3 -l;
	 	 b1 = b3-l;
	 	draw_sqr(colour,alpha,a1,a2,a3,a4,b1,b2,b3,b4,12);
	 	lcddelay(500);
	}
}

void draw_tree(int  x1,  int  y1,  int  x2,  int  y2,uint32_t  color,  int num , int ang)
{
	int c=0;
	int x_temp,y_temp;
    int angle = ang;
	if(num <=0)
		return;

	x_temp = x1;
	y_temp = y1;
	x1 = x2;
	x2 = x2+ (x1 - x_temp)*0.8;
	y1 = y2;
	y2 = y2+ (y1 - y_temp)*0.8;


	c=num/2;
	rotate_point(&x2,&y2,x1,y1,angle);
    for (int n=(c*(-1)); n<=c ; n=n+1 )
	drawLine(x1+n,y1,x2+n,y2,color);
    draw_tree(x1,y1,x2,y2,GREEN,num-1,angle);


	c=num/2;
	rotate_point(&x2,&y2,x1,y1,360-angle);
	for (int n=(c*(-1)); n<=c ; n=n+1 )
    drawLine(x1+n,y1,x2+n,y2,color);
	draw_tree(x1,y1,x2,y2,OLIVE,num-1,angle);

	c=num/2;
	rotate_point(&x2,&y2,x1,y1,360-angle);
	for (int n=-1*c; n<=c ; n=n+1 )
    drawLine(x1+n,y1,x2+n,y2,color);
	draw_tree(x1,y1,x2,y2,ORANGE,num-1,angle);

}


int main (void)


{

	 uint32_t pnum = PORT_NUM;
	 time_t t;
	 srand((unsigned) time(&t));
	 pnum = 0 ;

	 if ( pnum == 0 )
		 SSP0Init();

	 else
		 printf("Port number is not correct");

	 lcd_init();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLUE);
	 // for resetting the screen to blue

	 draw_screensaver(10);

	lcddelay(750);
	lcddelay(2750);

	fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLACK);

    int y1=25 ;
    int x1=25+rand()%5;
    int x2=x1;
    int y2= y1+28;


    for (int n=-4; n<=+4 ; n++ )
    drawLine(x1+n,y1,x2+n,y2,BROWN);
    //The number of times a loop is executed denotes the thickness of the tree


    draw_tree(x1,y1,x2,y2,BROWN,10,30);

     y1= rand()%15;
     x1=10+rand()%25;
     x2=x1-10;
     y2= y1+20;

     for (int n=-3; n<=3 ; n++ )
     drawLine(x1+n,y1,x2+n,y2,BROWN);
     draw_tree(x1,y1,x2,y2,BROWN,8,30);

    y1= rand()%4 +10;
    x1=75+rand()%27;
    x2=x1;
    y2= y1+30;

    for (int n=-4; n<=4 ; n++ )
    drawLine(x1+n,y1,x2+n,y2,BROWN);
    draw_tree(x1,y1,x2,y2,BROWN,8,25);

	return 0;

}


