/*************************************************************************************************
					HEADER SECTION
*************************************************************************************************/
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <graphics.h>
#include <stdlib.h>
#include <dos.h>
/*************************************************************************************************
				GLOBAL VARIABLE DECLARATION 
*************************************************************************************************/
union REGS i, o;
int playertype=0, gamemode=0;
int tpos[36][2], tn=99, ttn;	//tn=tiles no., ttn=total tiles no of the board
int tstatus[36][3];     	//[][0] for no of imagefile, [][1] for loaded, [][2] for matched
int cli=0;      // currently loaded image. cannot be more than 2
int p1p=0;	//player1 point
char username[21];
int m=0, mm=0, bonus=0; 	//m=matches, mm=misses
int playtime;			//time used to complete the game
//structure declaration for reading score data from file
struct score{   char name[21];   int point;   };
//relative path of image file used in game
char imagefile[18][100] =
{
	"Graphics\\Sun.txt",		"Graphics\\Flag.txt",
	"Graphics\\Bomb.txt",		"Graphics\\Busy.txt",
	"Graphics\\Yin.txt",		"Graphics\\Comp.txt",
	"Graphics\\Cross.txt",		"Graphics\\Fist.txt",
	"Graphics\\Guitar.txt",		"Graphics\\Heart.txt",
	"Graphics\\Trash.txt",		"Graphics\\Tick.txt",
	"Graphics\\Diamond.txt",	"Graphics\\Moon01.txt",
	"Graphics\\Skull_02.txt",	"Graphics\\OneEye.txt",
	"Graphics\\Pen.txt",		"Graphics\\Seagate.txt",
};

//(done)
#define INTR 0x09
#ifdef __cplusplus
   #define __CPPARGS ...
#else
   #define __CPPARGS
#endif
void int_open(void);
void int_close(void);
void anitext(int x, int y, char text[500]);

void interrupt (*oldhandler) (__CPPARGS);
void interrupt handler(__CPPARGS);
void interrupt handler(__CPPARGS)
{
	int n;
	disable();
	n=inport(0x60);
	if((n&0x00ff)==0x0041)
		exit(1);
	enable();
	oldhandler();
}

void pclose(void)
{
	closegraph();
	enable();
	oldhandler();
	int_close();
	delay(2000);
	anitext(11, 10, "Thank you for using this software. Hope you have enjoyed it.");
	delay(500);
}
#pragma exit pclose


//Prototyping of mouse functions
int initmouse(void);
void getmousepos(int *button, int *x, int *y);
void showmouseptr(void);
void hidemouseptr(void);

//Prototyping of functions
void round_tiles(int x, int y);
void board(int type);
void loadimage(void);
void set_tn(void);
void inittiles(int tno);
void round_rectangle(int x, int y, int w, int h, int round);
void menu_button(int x, int y, int w, int h);
void image(int x, int y, char filename[100]);
void credit_title(int y, char text[100]);
void credit_text(int y, char text[100]);
void pturn(int turn);
int quit(void);
void music(int type);
void instext(int i);

void score_read(int mode, score tempscore[10])
{
	int i;
	char temp[4];      //for atoi function
	FILE *f;

	if(mode==1)
		f = fopen("Easy.scf", "r");
	else if(mode==2)
		f = fopen("Normal.scf", "r");
	else if(mode==3)
		f = fopen("Hard.scf", "r");

	for (i=0; i<10; i++)
	{
		fgets(temp, 2, f);
		fgets(tempscore[i].name, 21, f);
		fgets(temp, 4, f);
		tempscore[i].point = atoi(temp);
	}
	fclose(f);
}
void score_write(int mode, score tempscore[10])
{
	int i;
	FILE *f;

	if(mode==1)
		f = fopen("Easy.scf", "w");
	else if(mode==2)
		f = fopen("Normal.scf", "w");
	else if(mode==3)
		f = fopen("Hard.scf", "w");

	fprintf(f, "\n");
	for (i=0; i<10; i++)
	{
		fprintf(f, "%s", tempscore[i].name);
		fprintf(f, "%3d", tempscore[i].point);
		fprintf(f, "\n");
	}
	fclose(f);
}

int score_update(void)
{
	int i, j; //for looping
	int t1point, t2point;
	char t1name[21], t2name[21];
	score temp[10];  //structure

	score_read(gamemode, temp);

	if (p1p>temp[9].point)
	{
		//drawing graphics for taking username as input
		cleardevice();
		hidemouseptr();

		//draw background
		setbkcolor(LIGHTBLUE);
		setfillstyle(11, BLUE);
		bar(0, 0, getmaxx(), getmaxy());
		setfillstyle(1, BLUE);
		bar(159, 139, 479, 339);
		setcolor(WHITE);
		rectangle(159+7, 139+7, 479-7, 339-7);
		rectangle(159+9, 139+9, 479-9, 339-9);
			
		int txtw;	//for textwidth() function

		settextstyle(4, 0, 0);
		txtw = textwidth("Congratulations");
		outtextxy(159+(320-txtw)/2, 150, "Congratulations");
		settextstyle(2, 0, 6);
		outtextxy(180, 190, "You have the highest score.");
		outtextxy(180, 210, "Please enter your name.");
		settextstyle(2, 0, 6);
		outtextxy(180, 240, "Your name here:");

		username[0] = '\0';
		//textbox control
		setfillstyle(1, LIGHTBLUE);
		bar(180, 260, 180+220, 260+20);
		rectangle(180, 260, 180+220, 260+20);
		outtextxy(190, 260, strcat(username, "_"));

		//draws menu button with OK text
		menu_button(270, 290, 100, 25);
		setcolor(RED);
		settextstyle(8, 0, 1);
		txtw = textwidth("OK");
		outtextxy(270+(100-txtw)/2, 287, "OK");

		showmouseptr();
		int mb, mx, my, cc=0;  //cc = charcounter
		char ch;

		music(2);
		while(1)
		{
			getmousepos(&mb, &mx, &my);
			if((mb &1)==1)
			{
				if (mx>=270 && mx<=270+100  && my>=290  && my<=290+25 && cc>0)
					{  music(4);  break;  }
			}

			if (kbhit())
			{
				ch = getch();
				if (ch>31 && ch<127 && cc<20)
				{
					music(4);
					hidemouseptr();
					username[cc] = ch;
					cc++;
					//textbox control
					setcolor(WHITE);
					setfillstyle(1, LIGHTBLUE);
					bar(180, 260, 180+220, 260+20);
					rectangle(180, 260, 180+220, 260+20);
					settextstyle(2, 0, 6);
					outtextxy(190, 260, strcat(username, "_"));
					showmouseptr();
				}
				else if (ch==8 && cc!=0)
				{
					music(4);
					hidemouseptr();
					username[cc-1] = '\0';
					cc--;
					//textbox control
					setcolor(WHITE);
					setfillstyle(1, LIGHTBLUE);
					bar(180, 260, 180+220, 260+20);
					rectangle(180, 260, 180+220, 260+20);
					settextstyle(2, 0, 6);
					outtextxy(190, 260, strcat(username, "_"));
					showmouseptr();
				}
				else if (ch==13 && cc>0)
					{  music(4);  break;  }
			}

		}
		//this code fills the username arrey with space
		if (cc<20)
		{
			for(i=cc; i<20; i++)
				username[i] = ' ';
		}
		username[20] = '\0';

		for(i=0; i<10; i++)
		{
			if(p1p>temp[i].point)
			{
				//code for update the structure
				int flag1 = 1;
				for (j=i; j<10; j++)
				{
					if(!flag1)
					{
						//saving ith  data to temp2
						strcpy(t2name, temp[j].name);
						t2point = temp[j].point;

						//Overwriting ith data with (i-1)th data
						strcpy(temp[j].name, t1name);
						temp[j].point = t1point;

						strcpy(t1name, t2name);
						t1point = t2point;
					}
					if(flag1)
					{
						strcpy(t1name, temp[j].name);
						t1point = temp[j].point;

						strcpy(temp[j].name, username);
						temp[j].point = p1p;

						flag1 = 0;
					}
				}
			break;
			}
		}
		score_write(gamemode, temp);
		return 3;
	}
	else
		return 0;
}

void screen0_fn1(void);
void screen1_fn1(void);
void screen3_fn1(void);
void screen3_fn2(int button);
void screen5_fn1(void);
void screen6_fn1(void);
void screen6_fn2(void);

int sp_game(void)
{
	cleardevice();
	hidemouseptr();

	setbkcolor(LIGHTBLUE);
	setfillstyle(11, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	board(gamemode);    //int value depends on the gamemode variable
	inittiles(gamemode);

	//draw the curret game status
	round_rectangle(20, 190, 110, 100, 10);
	setfillstyle(1, LIGHTBLUE);
	floodfill(20, 190, WHITE);

	int txtwidth;
	char ch[10], ch1;
	setfillstyle(1, RED);
	bar(20, 190, 130, 220);
	settextstyle(8, 0, 1);
	txtwidth = textwidth("Player 1");
	outtextxy(20+(110-txtwidth)/2, 190, "Player 1");

	settextstyle(2, 0, 0);
	outtextxy( 30, 230, "Matches");
	outtextxy(110, 230, itoa(0, ch, 10));
	outtextxy( 30, 245, "Misses");
	outtextxy(110, 245, itoa(0, ch, 10));
	outtextxy( 30, 260, "Bonus");
	outtextxy(110, 260, itoa(0, ch, 10));
	outtextxy( 30, 275, "Time (sec)");
	outtextxy(110, 275, itoa(0, ch, 10));

	showmouseptr();

	int mb, mx, my;
	int mcount=0, bcount=0;	//mcount=match tiles count, bcount=bonus count
	int pt=98, ct=99;	//previous tile and current tile
	int i, q;

	p1p=0, playtime=0;	//player1 point
	m=0, mm=0;  		//m=matches, mm=misses
	bonus=0, bcount=0;
	tn=99;
	long int s_time=0, e_time=0, te_time=0, ts_time=0;
	int tpt;	      	//tmp play time

	randomize();
	while(1)
	{
		getmousepos(&mb, &mx, &my);
		ts_time = time(NULL);
		if ((mb &1)==1)
		{
			if (tn==99)
				s_time = time(NULL);
			set_tn();
			if (tn!=88) //no tiles clicked
			{
				pt = ct;
				ct = tn;
			}
			loadimage();
			if (cli==2)
			{
				cli=0;
				//mis matched
				if (tstatus[ct][0]!=tstatus[pt][0])
				{
					hidemouseptr();
					delay(1000);

					setfillstyle(1, BLUE);
					bar(tpos[pt][0], tpos[pt][1], tpos[pt][0]+30, tpos[pt][1]+40);
					bar(tpos[ct][0], tpos[ct][1], tpos[ct][0]+30, tpos[ct][1]+40);
					round_tiles(tpos[pt][0], tpos[pt][1]);
					round_tiles(tpos[ct][0], tpos[ct][1]);

					tstatus[ct][1] = 0;
					tstatus[pt][1] = 0;

					showmouseptr();
					mm = mm+1;
					bcount = 0;
					//update info bar for mm
					setfillstyle(1, LIGHTBLUE);
					settextstyle(2, 0, 0);
					bar(110, 245, 130, 260);
					outtextxy(110, 245, itoa(mm, ch, 10));
				}
				//matched
				else if (tstatus[ct][0]==tstatus[pt][0])
				{
					hidemouseptr();
					music(1);
					delay(300);

					setfillstyle(1, BLUE);
					bar(tpos[pt][0], tpos[pt][1], tpos[pt][0]+30, tpos[pt][1]+40);
					bar(tpos[ct][0], tpos[ct][1], tpos[ct][0]+30, tpos[ct][1]+40);

					tstatus[ct][2] = 1;
					tstatus[pt][2] = 1;

					mcount = mcount+2;
					showmouseptr();
					m = m+1;
					bcount = bcount+1;
					if (bcount==3)
					{
						bonus = bonus+1;
						bcount = 0;
						//update info bar for bonus
						setfillstyle(1, LIGHTBLUE);
						settextstyle(2, 0, 0);
						bar(110, 260, 130, 275);
						outtextxy(110, 260, itoa(bonus, ch, 10));
					}
					//update info bar for m
					setfillstyle(1, LIGHTBLUE);
					settextstyle(2, 0, 0);
					bar(110, 230, 130, 245);
					outtextxy(110, 230, itoa(m, ch, 10));
				}
			}
			if (mcount==ttn)
			{
				e_time = time(NULL);
				playtime = e_time-s_time;
				return 9;	  //shows the scores
				//code for plays end
				//score will come to show
			}
		}

		te_time = time(NULL);
		if(tn!=99 && tn!=88)
		{
			tpt = te_time-s_time;
			if((te_time-ts_time)>=1)
			{
				setfillstyle(1, LIGHTBLUE);
				settextstyle(2, 0, 0);
				bar(110, 275, 130, 290);
				outtextxy(110, 275, itoa(tpt, ch, 10));
			}
		}
		if (kbhit())
		{
			ch1 = getch();
			if (ch1==27)
			{
				q = quit();
				if (q==1)
				{
					cli = 0;
					return 0;
				}
				else if (q==2)
				{
					//goto previous state
					hidemouseptr();

					setfillstyle(11, BLUE);
					bar(150, 0, 490, getmaxy());
					board(gamemode);
					for (i=0; i<ttn; i++)
					{
						if (tstatus[i][2]==1)
						{
							setfillstyle(1, BLUE);
							bar(tpos[i][0], tpos[i][1], tpos[i][0]+30, tpos[i][1]+40);
						}
						if (tstatus[i][1]==1 && tstatus[i][2]==0)
							image(tpos[i][0], tpos[i][1], imagefile[tstatus[i][0]]);
					}
					showmouseptr();
				}
			}
		}
	}
}

int dp_game(void)
{
	cleardevice();
	hidemouseptr();

	setbkcolor(LIGHTBLUE);
	setfillstyle(11, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	board(gamemode);    //int value depends on the gamemode variable
	inittiles(gamemode);

	//draw the total match of player1
	round_rectangle(20, 190, 110, 100, 10);
	setfillstyle(1, LIGHTBLUE);
	floodfill(20, 190, WHITE);
	//draw the total match of player2
	round_rectangle(510, 190, 110, 100, 10);
	setfillstyle(1, LIGHTBLUE);
	floodfill(510, 190, WHITE);

	int txtwidth, txth;
	char ch[10], ch1;     //convert int var. to string
	int p1m=0, p2m=0;	//player 1 match, player2 match
	int playerturn=1;
	int mb, mx, my;
	int mcount=0;	    //mcount=match tiles count, bcount=bonus count
	int pt=98, ct=99;   //previous tile and current tile
	int q, i;
	tn = 99;

	//drawing player1 and player2
	pturn(playerturn);

	//no of mathing tiles for both player
	settextstyle(10, 0, 0);
	txtwidth = textwidth(itoa(p1m, ch, 10));
	outtextxy(20+(110-txtwidth)/2, 220, itoa(p1m, ch, 10));
	txtwidth = textwidth(itoa(p2m, ch, 10));
	outtextxy(510+(110-txtwidth)/2, 220, itoa(p2m, ch, 10));

	showmouseptr();
	while(1)
	{
		getmousepos(&mb, &mx, &my);
		if ((mb &1)==1)
		{
			set_tn();
			if (tn!=88)
			{
				pt = ct;
				ct = tn;
			}
			loadimage();
			if (cli==2)
			{
				cli=0;
				//mis matched
				if (tstatus[ct][0]!=tstatus[pt][0])
				{
					hidemouseptr();
					delay(1000);

					setfillstyle(1, BLUE);
					bar(tpos[pt][0], tpos[pt][1], tpos[pt][0]+30, tpos[pt][1]+40);
					bar(tpos[ct][0], tpos[ct][1], tpos[ct][0]+30, tpos[ct][1]+40);
					round_tiles(tpos[pt][0], tpos[pt][1]);
					round_tiles(tpos[ct][0], tpos[ct][1]);

					tstatus[ct][1] = 0;
					tstatus[pt][1] = 0;

					if (playerturn==1)
						playerturn = 2;
					else if (playerturn==2)
						playerturn = 1;

					pturn(playerturn);
					showmouseptr();
				}
				//matched
				else if (tstatus[ct][0]==tstatus[pt][0])
				{
					hidemouseptr();
					music(1);
					delay(300);

					setfillstyle(1, BLUE);
					bar(tpos[pt][0], tpos[pt][1], tpos[pt][0]+30, tpos[pt][1]+40);
					bar(tpos[ct][0], tpos[ct][1], tpos[ct][0]+30, tpos[ct][1]+40);

					tstatus[ct][2] = 1;
					tstatus[pt][2] = 1;

					mcount = mcount+2;
					if(playerturn==1)
					{
						p1m++;
						//update the value of p1m
						setfillstyle(1, LIGHTBLUE);
						bar(20, 221, 130, 290);
						settextstyle(10, 0, 0);
						txtwidth = textwidth(itoa(p1m, ch, 10));
						outtextxy(20+(110-txtwidth)/2, 220, itoa(p1m, ch, 10));
					}
					else if(playerturn==2)
					{
						p2m++;
						//update the value of p2m
						setfillstyle(1, LIGHTBLUE);
						bar(510, 221, 620, 290);
						settextstyle(10, 0, 0);
						txtwidth = textwidth(itoa(p2m, ch, 10));
						outtextxy(510+(110-txtwidth)/2, 220, itoa(p2m, ch, 10));
					}
					showmouseptr();
				}
			}
		}
		if (p1m==20 || p2m==20)
		{
			//declare which player has won
			setfillstyle(1, BLUE);
			if (gamemode==1)
				bar(209, 109, 429, 369);
			else if (gamemode==2)
				bar(159, 109, 479, 369);
			else if (gamemode==3)
				bar(159, 49, 479, 429);

			settextstyle(1, 0, 0);
			if (p1m>p2m)
			{
				//p1 wins
				txtwidth = textwidth("Player 1 wins!");
				txth = textheight("Player 1 wins!");
				while(1)
				{
					for (i=2; i<16; i++)
					{
						setcolor(i);
						outtextxy((getmaxx()-txtwidth)/2, (getmaxy()-txth)/2, "Player 1 wins!");
						sound(i*100);
						delay(200);
					}
					if(kbhit())
					{
						nosound();
						return 0;
					}
				}
			}
			else if (p2m>p1m)
			{
				//p2 wins
				txtwidth = textwidth("Player 2 wins!");
				txth = textheight("Player 2 wins!");
				while(1)
				{
					for (i=2; i<16; i++)
					{
						setcolor(i);
						outtextxy((getmaxx()-txtwidth)/2, (getmaxy()-txth)/2, "Player 2 wins!");
						sound(i*100);
						delay(200);
					}
					if(kbhit())
					{
						nosound();
						return 0;
					}
				}
			}
		}
		if (mcount==ttn)
		{
			hidemouseptr();
			board(gamemode);
			inittiles(gamemode);
			mcount = 0;
			showmouseptr();
		}
		if (kbhit())
		{
			ch1 = getch();
			if (ch1==27)
			{
				q = quit();
				if (q==1)
				{
					cli = 0;
					return 0;
				}
				else if (q==2)
				{
					//goto previous state
					hidemouseptr();

					setfillstyle(11, BLUE);
					bar(150, 0, 490, getmaxy());
					board(gamemode);
					for (i=0; i<ttn; i++)
					{
						if (tstatus[i][2]==1)
						{
							setfillstyle(1, BLUE);
							bar(tpos[i][0], tpos[i][1], tpos[i][0]+30, tpos[i][1]+40);
						}
						if (tstatus[i][1]==1 && tstatus[i][2]==0)
							image(tpos[i][0], tpos[i][1], imagefile[tstatus[i][0]]);
					}
					showmouseptr();
				}
			}
		}
	}
}

int screen0(void)
{
	cleardevice();
	hidemouseptr();

	setbkcolor(LIGHTBLUE);
	setfillstyle(11, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	//Draws the logo of the game
	image(0, 0, "Graphics\\Title.txt");
	settextstyle(2, 0, 0);
	setcolor(LIGHTGRAY);
	outtextxy(110, 6, "version 1.4");

	round_rectangle(getmaxx()/2-110, 80, 220, 15, 10);
	round_rectangle(getmaxx()/2-110, 125, 220, 200, 10);

	int txtwidth;		//determines width of button text for allignment
	int mb, mx, my;		//var for detect mouse position

	settextstyle(4, 0, 0);
	txtwidth = textwidth("Game Menu");
	outtextxy(getmaxx()/2-(txtwidth/2), 65, "Game Menu");

	//draws menu buttons for 5 menus
	menu_button((getmaxx()/2)-100, 130, 200, 30);
	menu_button((getmaxx()/2)-100, 170, 200, 30);
	menu_button((getmaxx()/2)-100, 210, 200, 30);
	menu_button((getmaxx()/2)-100, 250, 200, 30);
	menu_button((getmaxx()/2)-100, 290, 200, 30);

	settextstyle(1, 0, 2);
	setcolor(BLUE);
	txtwidth = textwidth("New Game");
	outtextxy(getmaxx()/2-(txtwidth/2), 130, "New Game");
	txtwidth = textwidth("Instructions");
	outtextxy(getmaxx()/2-(txtwidth/2), 170, "Instructions");
	txtwidth = textwidth("High Scores");
	outtextxy(getmaxx()/2-(txtwidth/2), 210, "High Scores");
	txtwidth = textwidth("Credit");
	outtextxy(getmaxx()/2-(txtwidth/2), 250, "Credit");
	txtwidth = textwidth("Exit");
	outtextxy(getmaxx()/2-(txtwidth/2), 290, "Exit");


	int button=1, flag=1;		//tell which button currently selected
	char ch;		//tracees the value of key hit
	showmouseptr();
	delay(200);
	while(1)
	{
		if(flag)
		{
			if (button==1)
			{
				delay(50);
				screen0_fn1();
				setcolor(RED);
				txtwidth = textwidth("New Game");
				outtextxy(getmaxx()/2-(txtwidth/2), 130, "New Game");	
			}
			else if (button==2)
			{
				delay(50);
				screen0_fn1();
				setcolor(RED);	
				txtwidth = textwidth("Instructions");
				outtextxy(getmaxx()/2-(txtwidth/2), 170, "Instructions");
			}
			else if (button==3)
			{
				delay(50);
				screen0_fn1();
				setcolor(RED);
				txtwidth = textwidth("High Scores");
				outtextxy(getmaxx()/2-(txtwidth/2), 210, "High Scores");
			}
			else if (button==4)
			{
				delay(50);
				screen0_fn1();
				setcolor(RED);
				txtwidth = textwidth("Credit");
				outtextxy(getmaxx()/2-(txtwidth/2), 250, "Credit");
			}
			else if (button==5)
			{
				delay(50);
				screen0_fn1();
				setcolor(RED);	
				txtwidth = textwidth("Exit");
				outtextxy(getmaxx()/2-(txtwidth/2), 290, "Exit");
			}
			flag = 0;
		}

		getmousepos(&mb, &mx, &my);
		if ((mb &0)==0)
		{
			if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=130 && my<=130+30)
			{	button = 1;	flag = 1;	}
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=170 && my<=170+30)
			{	button = 2;	flag = 1;	}
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=210 && my<=210+30)
			{	button = 3;	flag = 1;	}
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=250 && my<=250+30)
			{	button = 4;	flag = 1;	}
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=290 && my<=290+30)
			{	button = 5;	flag = 1;	}
		}
		if (kbhit())
		{
			ch = getch();
			if (ch==72 && button>=1)
			{
				button--;
				if (button==0)
					button = 5;
				flag = 1;
			}
			else if (ch==80 && button<=5)
			{
				button++;
				if (button==6)
					button = 1;
				flag = 1;
			}

			else if (ch=='N' || ch=='n')
			{	button = 1;	flag = 1;	}
			else if (ch=='I' || ch=='i')
			{	button = 2;	flag = 1;	}
			else if (ch=='S' || ch=='s')
			{	button = 3;	flag = 1;	}
			else if (ch=='C' || ch=='c')
			{	button = 4;	flag = 1;	}
			else if (ch=='E' || ch=='e')
			{	button = 5;	flag = 1;	}

			else if (ch==13)
			{
				if (button==1)
				{  music(4);  return 1;  }
				else if (button==2)
				{  music(4);  return 2;  }
				else if (button==3)
				{  music(4);  return 3;  }
				else if (button==4)
				{  music(4);  return 4;  }
				else if (button==5)
				{  music(4);  return 5;  }
			}
			else if (ch==27)
				{  music(4);  return 5;  }
		}
		if ((mb &1)==1)
		{
			if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=130 && my<=130+30)
				{  music(4);  return 1;  }
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=170 && my<=170+30)
				{  music(4);  return 2;  }
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=210 && my<=210+30)
				{  music(4);  return 3;  }
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=250 && my<=250+30)
				{  music(4);  return 4;  }
			else if (mx>=getmaxx()/2-100 && mx<=getmaxx()/2-100+200 && my>=290 && my<=290+30)
				{  music(4);  return 5;  }
		}

	}
}

int screen1(void)
{
	//clearing screen and drawing background
	cleardevice();
	hidemouseptr();
	setbkcolor(LIGHTBLUE);
	setfillstyle(11, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	//Draws the logo of the game
	image(0, 0, "Graphics\\Title.txt");
	settextstyle(2, 0, 0);
	setcolor(LIGHTGRAY);
	outtextxy(110, 6, "version 1.4");

	round_rectangle(getmaxx()/2-110, 80, 220, 15, 10);
	round_rectangle(getmaxx()/2-110, 125, 220, 160, 10);

	int txtwidth;		//determines width of button text for allignment
	int mb, mx, my;		//var for detect mouse position

	//draw menu name
	settextstyle(4, 0, 0);
	txtwidth = textwidth("Player Type");
	outtextxy(getmaxx()/2-(txtwidth/2), 65, "Player Type");

	//draws single player
	setcolor(LIGHTGRAY);
	rectangle(getmaxx()/2-100, 140, getmaxx()/2+100, 140+34);
	setfillstyle(1, BLUE);
	floodfill(getmaxx()/2-100+2, 140+2, LIGHTGRAY);
	image(getmaxx()/2-100+10, 140+1, "Graphics\\1 Player.txt");
	settextstyle(1, 0, 2);
	setcolor(WHITE);
	outtextxy(getmaxx()/2-100+15+32, 143, "Single Player");

	//draw double player
	setcolor(LIGHTGRAY);
	rectangle(getmaxx()/2-100, 190, getmaxx()/2+100, 190+34);
	setfillstyle(1, BLUE);
	floodfill(getmaxx()/2-100+2, 190+2, LIGHTGRAY);
	image(getmaxx()/2-100+10, 190+1, "Graphics\\2 Player.txt");
	settextstyle(1, 0, 2);
	setcolor(WHITE);
	outtextxy(getmaxx()/2-100+15+32, 193, "Double Player");

	//draw OK and Cancel button
	menu_button((getmaxx()/2)-100, 240, 90, 30);
	txtwidth = textwidth("OK");
	setcolor(BLUE);
	outtextxy(((getmaxx()/2)-100)+(90-txtwidth)/2, 241, "OK");
	menu_button((getmaxx()/2)+10, 240, 90, 30);
	txtwidth = textwidth("Cancel");
	setcolor(BLUE);
	outtextxy(((getmaxx()/2)+10)+(90-txtwidth)/2, 241, "Cancel");

	if (playertype==1)
	{
		setfillstyle(1, LIGHTBLUE);
		floodfill(219+2, 140+2, LIGHTGRAY);
		image(219+10, 140+1, "Graphics\\1 Player.txt");
		settextstyle(1, 0, 2);
		setcolor(WHITE);
		outtextxy(219+15+32, 143, "Single Player");
	}
	else if (playertype==2)
	{
		setfillstyle(1, LIGHTBLUE);
		floodfill(219+2, 190+2, LIGHTGRAY);
		image(219+10, 190+1, "Graphics\\2 Player.txt");
		settextstyle(1, 0, 2);
		setcolor(WHITE);
		outtextxy(219+15+32, 193, "Double Player");
	}

	int button=1;		//tell which button currently selected
	char ch;		//tracees the value of key hit
	showmouseptr();
	delay(200);
	while(1)
	{
		if (button==1)
		{
			setcolor(RED);
			txtwidth = textwidth("OK");
			outtextxy(((getmaxx()/2)-100)+(90-txtwidth)/2, 241, "OK");
			delay(50);
			screen1_fn1();
		}
		else if (button==2)
		{
			setcolor(RED);
			txtwidth = textwidth("Cancel");
			outtextxy(((getmaxx()/2)+10)+(90-txtwidth)/2, 241, "Cancel");
			delay(50);
			screen1_fn1();
		}

		getmousepos(&mb, &mx, &my);
		if ((mb &0)==0)
		{
			if (mx>=219 && mx<=219+90 && my>=240 && my<=240+30)
				button = 1;
			else if (mx>=329 && mx<=329+90 && my>=240 && my<=240+30)
				button = 2;
		}

		if (kbhit())
		{
			ch = getch();
			if (ch=='O' || ch=='o')
				button = 1;
			else if (ch=='C' || ch=='c')
				button = 2;
			else if (ch==13)
			{
				if (button==1 && playertype!=0)
					{  music(4);  return 6;  }//goto mode select screen
				else if (button==2)
					{  music(4);  return 0;  }//goto main menu
			}
			else if (ch=='S' || ch=='s')
			{
				playertype = 1;
				hidemouseptr();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 140+2, LIGHTGRAY);
				image(219+10, 140+1, "Graphics\\1 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 143, "Single Player");

				setfillstyle(1, BLUE);
				floodfill(219+2, 190+2, LIGHTGRAY);
				image(219+10, 190+1, "Graphics\\2 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 193, "Double Player");

				showmouseptr();
			}
			else if (ch=='D' || ch=='d')
			{
				playertype = 2;
				hidemouseptr();

				setfillstyle(1, BLUE);
				floodfill(219+2, 140+2, LIGHTGRAY);
				image(219+10, 140+1, "Graphics\\1 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 143, "Single Player");

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 190+2, LIGHTGRAY);
				image(219+10, 190+1, "Graphics\\2 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 193, "Double Player");

				showmouseptr();
			}
			else if (ch==27)
				{  music(4);  return 0;  } //back to main menu
		}
		if ((mb &1)==1)
		{
			if (mx>=219 && mx<=219+90 && my>=240 && my<=240+30 && playertype!=0)
				{  music(4);  return 6;  }//goto select game mode screen
			else if (mx>=329 && mx<=329+90 && my>=240 && my<=240+30)
				{  music(4);  return 0;  }//goto main menu
			else if (mx>=219 && mx<=419 && my>=140 && my<=140+34)
			{
				playertype = 1;
				hidemouseptr();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 140+2, LIGHTGRAY);
				image(219+10, 140+1, "Graphics\\1 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 143, "Single Player");

				setfillstyle(1, BLUE);
				floodfill(219+2, 190+2, LIGHTGRAY);
				image(219+10, 190+1, "Graphics\\2 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 193, "Double Player");

				showmouseptr();
			}
			else if (mx>=219 && mx<=419 && my>=190 && my<=190+34)
			{
				playertype = 2;
				hidemouseptr();

				setfillstyle(1, BLUE);
				floodfill(219+2, 140+2, LIGHTGRAY);
				image(219+10, 140+1, "Graphics\\1 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 143, "Single Player");

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 190+2, LIGHTGRAY);
				image(219+10, 190+1, "Graphics\\2 Player.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 193, "Double Player");

				showmouseptr();
			}
		}
	}
}

int screen2(void)
{
	initmouse();

	setbkcolor(LIGHTBLUE);
	setfillstyle(11, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	//Draws the logo of the game
	image(0, 0, "Graphics\\Title.txt");
	settextstyle(2, 0, 0);
	setcolor(LIGHTGRAY);
	outtextxy(110, 6, "version 1.4");

	round_rectangle(getmaxx()/2-110, 80, 220, 15, 10);
	round_rectangle(getmaxx()/2-270, 125, 540, 330, 10);

	int txtwidth;
	settextstyle(4, 0, 0);
	txtwidth = textwidth("Instructions");
	outtextxy(getmaxx()/2-(txtwidth/2), 65, "Instructions");

	setviewport(60, 130, 580, 450, 1);
	
	int mx, my, mb, i=0;
	char ch;
	instext(i);

	showmouseptr();
	while(1)
	{
		getmousepos(&mb, &mx, &my);
		if(kbhit())
		{
			ch = getch();
			if(ch==80 && i>-928)
			{
				i = i-32;
				hidemouseptr();
				instext(i);
				showmouseptr();
			}
			else if(ch==72 && i<0)
			{
				i = i+32;
				hidemouseptr();
				instext(i);
				showmouseptr();
			}
			else if(ch==27)
				return 0;
		}
	}
}

int screen3(void)
{
	cleardevice();
	hidemouseptr();
	setbkcolor(LIGHTBLUE);
	setfillstyle(11, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	//Draws the logo of the game
	image(0, 0, "Graphics\\Title.txt");
	settextstyle(2, 0, 0);
	setcolor(LIGHTGRAY);
	outtextxy(110, 6, "version 1.4");

	round_rectangle(getmaxx()/2-110, 80, 220, 15, 10);
	round_rectangle(getmaxx()/2-145, 125, 290, 210, 10);

	int txtwidth;
	settextstyle(4, 0, 0);
	txtwidth = textwidth("High Scores");
	outtextxy(getmaxx()/2-(txtwidth/2), 65, "High Scores");

	//drawing the 4 buttons
	menu_button(370, 135, 90, 30);
	menu_button(370, 175, 90, 30);
	menu_button(370, 215, 90, 30);
	menu_button(370, 295, 90, 30);
	//drawing menu_button label
	setcolor(BLUE);
	settextstyle(1, 0, 2);
	txtwidth = textwidth("Easy");
	outtextxy(370+(90-txtwidth)/2, 136, "Easy");
	txtwidth = textwidth("Normal");
	outtextxy(370+(90-txtwidth)/2, 176, "Normal");
	txtwidth = textwidth("Hard");
	outtextxy(370+(90-txtwidth)/2, 216, "Hard");
	txtwidth = textwidth("Back");
	outtextxy(370+(90-txtwidth)/2, 296, "Back");

	//declaring struct ob. to store highscores
	score easy[10], normal[10], hard[10];
	score_read(1, easy);
	score_read(2, normal);
	score_read(3, hard);

	if (gamemode==0)
		gamemode=1;

	//preparing the text back for highscore
	setfillstyle(1, LIGHTBLUE);
	bar(174, 135, 174+182, 135+30);
	setcolor(LIGHTBLUE);
	rectangle(174, 135, 174+182, 135+30+160);
	//drawing easy mode
	settextstyle(1, 0, 2);
	setcolor(WHITE);

	if (gamemode==1)
	{
		txtwidth = textwidth("Easy Mode");
		outtextxy(174+(182-txtwidth)/2, 136, "Easy Mode");
	}
	else if(gamemode==2)
	{
		txtwidth = textwidth("Normal Mode");
		outtextxy(174+(182-txtwidth)/2, 136, "Normal Mode");
	}
	else if(gamemode==3)
	{
		txtwidth = textwidth("Hard Mode");
		outtextxy(174+(182-txtwidth)/2, 136, "Hard Mode");
	}

	//writing easy mode highscore as default
	char t1[4];
	int i;
	setcolor(WHITE);
	settextstyle(2, 0, 0);

	if(gamemode==1)
	{
		for (i=0; i<10; i++)
		{
			outtextxy(180,     170+i*15, easy[i].name);
			outtextxy(180+150, 170+i*15, itoa(easy[i].point, t1, 10));
		}
	}
	else if(gamemode==2)
	{
		for (i=0; i<10; i++)
		{
			outtextxy(180,     170+i*15, normal[i].name);
			outtextxy(180+150, 170+i*15, itoa(normal[i].point, t1, 10));
		}
	}
	else if(gamemode==3)
	{
		for (i=0; i<10; i++)
		{
			outtextxy(180,     170+i*15, hard[i].name);
			outtextxy(180+150, 170+i*15, itoa(hard[i].point, t1, 10));
		}
	}

	showmouseptr();
	int mb, mx, my, button=1;
	char ch;

	while(1)
	{
		getmousepos(&mb, &mx, &my);
		if(button==1)
		{
			settextstyle(1, 0, 2);
			setcolor(RED);
			txtwidth = textwidth("Easy");
			outtextxy(370+(90-txtwidth)/2, 136, "Easy");
			delay(50);
			screen3_fn1();
		}
		else if(button==2)
		{
			settextstyle(1, 0, 2);
			setcolor(RED);
			txtwidth = textwidth("Normal");
			outtextxy(370+(90-txtwidth)/2, 176, "Normal");
			delay(50);
			screen3_fn1();
		}
		else if(button==3)
		{
			settextstyle(1, 0, 2);
			setcolor(RED);
			txtwidth = textwidth("Hard");
			outtextxy(370+(90-txtwidth)/2, 216, "Hard");
			delay(50);
			screen3_fn1();
		}
		else if(button==4)
		{
			settextstyle(1, 0, 2);
			setcolor(RED);
			txtwidth = textwidth("Back");
			outtextxy(370+(90-txtwidth)/2, 296, "Back");
			delay(50);
			screen3_fn1();
		}

		if((mb &0)==0)
		{
			if(mx>=370 && mx<=370+90 && my>=135 && my<=135+30)
				button = 1;
			else if(mx>=370 && mx<=370+90 && my>175 && my<=175+30)
				button = 2;
			else if(mx>=370 && mx<=370+90 && my>=215 && my<=215+30)
				button = 3;
			else if(mx>=370 && mx<=370+90 && my>=295 && my<=295+30)
				button = 4;
		}

		if(kbhit())
		{
			ch = getch();
			if(ch=='E' || ch=='e')
				button = 1;
			else if(ch=='N' || ch=='n')
				button = 2;
			else if(ch=='H' || ch=='h')
				button =3;
			else if(ch=='B' || ch=='b')
				button = 4;
			else if(ch==13)
			{
				if (button==1)
				{
					music(4);
					screen3_fn2(1);
					setcolor(WHITE);
					settextstyle(2, 0, 0);
					for (i=0; i<10; i++)
					{
						outtextxy(180,     170+i*15, easy[i].name);
						outtextxy(180+150, 170+i*15, itoa(easy[i].point, t1, 10));
					}
				}
				else if (button==2)
				{
					music(4);
					screen3_fn2(2);
					setcolor(WHITE);
					settextstyle(2, 0, 0);
					for (i=0; i<10; i++)
					{
						outtextxy(180,     170+i*15, normal[i].name);
						outtextxy(180+150, 170+i*15, itoa(normal[i].point, t1, 10));
					}
				}
				else if (button==3)
				{
					music(4);
					screen3_fn2(3);
					setcolor(WHITE);
					settextstyle(2, 0, 0);
					for (i=0; i<10; i++)
					{
						outtextxy(180,     170+i*15, hard[i].name);
						outtextxy(180+150, 170+i*15, itoa(hard[i].point, t1, 10));
					}
				}
				else if (button==4)
					{  music(4);  return 0;  }
			}
			else if(ch==27)
				{  music(4);  return 0;  }
		}

		if((mb &1)==1)
		{
			if(mx>=370 && mx<=370+90 && my>=135 && my<=135+30)
			{
				music(4);
				screen3_fn2(1);
				setcolor(WHITE);
				settextstyle(2, 0, 0);
				for (i=0; i<10; i++)
				{
					outtextxy(180,     170+i*15, easy[i].name);
					outtextxy(180+150, 170+i*15, itoa(easy[i].point, t1, 10));
				}
			}
			else if(mx>=370 && mx<=370+90 && my>175 && my<=175+30)
			{
				music(4);
				screen3_fn2(2);
				setcolor(WHITE);
				settextstyle(2, 0, 0);
				for (i=0; i<10; i++)
				{
					outtextxy(180,     170+i*15, normal[i].name);
					outtextxy(180+150, 170+i*15, itoa(normal[i].point, t1, 10));
				}
			}
			else if(mx>=370 && mx<=370+90 && my>=215 && my<=215+30)
			{
				music(4);
				screen3_fn2(3);
				setcolor(WHITE);
				settextstyle(2, 0, 0);
				for (i=0; i<10; i++)
				{
					outtextxy(180,     170+i*15, hard[i].name);
					outtextxy(180+150, 170+i*15, itoa(hard[i].point, t1, 10));
				}
			}
			else if(mx>=370 && mx<=370+90 && my>=295 && my<=295+30)
				{  music(4);  return 0;  }
		}
	}
}

int screen4(void)
{
	cleardevice();
	setbkcolor(LIGHTBLUE);
	setfillstyle(1, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	//Draws the logo of the game
	image(0, 0, "Graphics\\Title.txt");
	settextstyle(2, 0, 0);
	setcolor(LIGHTGRAY);
	outtextxy(110, 6, "version 1.4");

	//setcolor(WHITE);
	settextstyle(2, 0, 0);
	outtextxy(10, getmaxy()-20, "Press ESCAPE to back.");


	setviewport(0, getmaxy()/2-175, getmaxx(), getmaxy()/2+175, 1);

	int i, txtw;
	char ch;
	hidemouseptr();
	for (i=370; i>-735; i=i-1)
	{
		if (kbhit())
		{
			ch = getch();
			if (ch==27)
			{
				showmouseptr();
				music(4);
				return 0;
			}
		}
		clearviewport();

		credit_title(i, "Lead Programmer");
		credit_text(i+30, "Sk. Md. Arif Uddin");

		credit_title(i+100, "Assistant Programmer");
		credit_text(i+130, "A. B. M. Faruque");
		credit_text(i+145, "Rajib Ahmed");

		credit_title(i+215, "Graphic Artist");
		credit_text(i+245, "A. B. M. Faruque");
		credit_text(i+260, "Sk. Md. Arif Uddin");

		credit_title(i+330, "Music Composer");
		credit_text(i+360, "Rajib Ahmed");

		credit_title(i+430, "Code Tester");
		credit_text(i+460, "Sk. Md. Arif Uddin");
		credit_text(i+475, "A. B. M. Faruque");
		credit_text(i+490, "Rajib Ahmed");

		credit_title(i+560, "Acknowledgement");
		credit_text(i+590, "Thanks to almighty Allah for giving us the strength, power and ability to");
		credit_text(i+605, "complete this game. Thanks to the class for inspiring us all the time.");
		credit_text(i+620, "They showed interest and talked about our work which we find very useful.");
		credit_text(i+635, "Thanks to all of our teachers, without their help we would not be able");
		credit_text(i+650, "to finish the work. Special thanks to Paris Sir and Shibly Sir for giving");
		credit_text(i+665, "us proper guidance to finish the game successfully.");

		delay(75);
	}

	showmouseptr();
	return 0;
}

int screen5(void)
{
	hidemouseptr();

	setfillstyle(1, BLUE);
	bar(159, 139, 479, 339);
	setcolor(WHITE);
	rectangle(159+7, 139+7, 479-7, 339-7);
	rectangle(159+9, 139+9, 479-9, 339-9);

	settextstyle(8, 0, 1);
	setcolor(WHITE);
	outtextxy(159+20, 139+20, "Do you really want to exit?");

	menu_button(319-130, 279, 110, 30);
	menu_button(319+20,  279, 110, 30);

	int txtwidth;
	setcolor(BLUE);
	settextstyle(8, 0, 1);
	txtwidth = textwidth("Yes");
	outtextxy(189+(110-txtwidth)/2, 279, "Yes");
	txtwidth = textwidth("No");
	outtextxy(339+(110-txtwidth)/2, 279, "No");

	showmouseptr();
	int mb, mx, my, button=2;
	char ch;
	delay(200);
	while(1)
	{
		getmousepos(&mb, &mx, &my);
		if (button==1)
		{
			setcolor(RED);
			txtwidth = textwidth("Yes");
			outtextxy(189+(110-txtwidth)/2, 279, "Yes");
			delay(50);
			screen5_fn1();
		}
		else if (button==2)
		{
			setcolor(RED);
			txtwidth = textwidth("No");
			outtextxy(339+(110-txtwidth)/2, 279, "No");
			delay(50);
			screen5_fn1();
		}

		if ((mb &0)==0)
		{
			if (mx>=319-130 && mx<=319-130+110  && my>=279  && my<=279+30)
				button = 1;
			else if (mx>=319+20 && mx<=319+20+110 & my>=279 && my<=279+30)
				button = 2;
		}
		if((mb &1)==1)
		{
			if (mx>=319-130 && mx<=319-130+110  && my>=279  && my<=279+30)
				{  music(4);  exit(1);  }
			else if (mx>=319+20 && mx<=319+20+110 & my>=279 && my<=279+30)
				{  music(4);  return 0;  }
		}

		if (kbhit())
		{
			ch = getch();
			if (ch=='Y' || ch=='y')
				button = 1;
			else if (ch=='N' || ch=='n')
				button = 2;
			else if (ch==13)
			{
				if (button==1)
					{  music(4);  exit(1);  }
				else if(button==2)
					{  music(4);  return 0;  }
			}
			else if (ch==27)
				{  music(4);  return 0;  }
		}

	}
}

int screen6(void)
{
	cleardevice();
	hidemouseptr();

	setbkcolor(LIGHTBLUE);
	setfillstyle(11, BLUE);
	bar(0, 0, getmaxx(), getmaxy());

	//Draws the logo of the game
	image(0, 0, "Graphics\\Title.txt");
	settextstyle(2, 0, 0);
	setcolor(LIGHTGRAY);
	outtextxy(110, 6, "version 1.4");

	round_rectangle(getmaxx()/2-110, 80, 220, 15, 10);
	round_rectangle(getmaxx()/2-110, 125, 220, 210, 10);

	int txtwidth;		//determines width of button text for allignment
	int mb, mx, my;		//var for detect mouse position

	//draw menu name
	settextstyle(4, 0, 0);
	txtwidth = textwidth("Game Mode");
	outtextxy(getmaxx()/2-(txtwidth/2), 65, "Game Mode");

	//draw the rectangles
	setcolor(LIGHTGRAY);
	rectangle(219, 140, 419, 140+34);
	rectangle(219, 190, 419, 190+34);
	rectangle(219, 240, 419, 240+34);

	//draw icon and text for > easy, normal, hard
	screen6_fn2();

	//draw OK and Cancel button
	menu_button(219, 290, 90, 30);
	txtwidth = textwidth("OK");
	setcolor(BLUE);
	outtextxy(219+(90-txtwidth)/2, 291, "OK");
	menu_button(319+10, 290, 90, 30);
	txtwidth = textwidth("Cancel");
	setcolor(BLUE);
	outtextxy((319+10)+(90-txtwidth)/2, 291, "Cancel");

	if (gamemode==1)
	{
		setfillstyle(1, LIGHTBLUE);
		floodfill(219+2, 140+2, LIGHTGRAY);
		image(219+10, 140+1, "Graphics\\Easy.txt");
		settextstyle(1, 0, 2);
		setcolor(WHITE);
		outtextxy(219+15+32, 143, "Easy Mode");
	}
	else if (gamemode==2)
	{
		setfillstyle(1, LIGHTBLUE);
		floodfill(219+2, 190+2, LIGHTGRAY);
		image(219+10, 190+1, "Graphics\\Normal.txt");
		settextstyle(1, 0, 2);
		setcolor(WHITE);
		outtextxy(219+15+32, 193, "Normal Mode");
	}
	else if (gamemode==3)
	{
		setfillstyle(1, LIGHTBLUE);
		floodfill(219+2, 240+2, LIGHTGRAY);
		image(219+10, 240+1, "Graphics\\Hard.txt");
		settextstyle(1, 0, 2);
		setcolor(WHITE);
		outtextxy(219+15+32, 243, "Hard Mode");
	}

	int button=1;		//tell which button currently selected
	char ch;		//tracees the value of key hit
	showmouseptr();
	delay(200);
	while(1)
	{
		if (button==1)
		{
			setcolor(RED);
			txtwidth = textwidth("OK");
			outtextxy(219+(90-txtwidth)/2, 291, "OK");
			delay(50);
			screen6_fn1();
		}
		else if (button==2)
		{
			setcolor(RED);
			txtwidth = textwidth("Cancel");
			outtextxy((319+10)+(90-txtwidth)/2, 291, "Cancel");
			delay(50);
			screen6_fn1();
		}

		getmousepos(&mb, &mx, &my);
		if ((mb &0)==0)
		{
			if (mx>=219 && mx<=219+90 && my>=290 && my<=290+30)
				button = 1;
			else if (mx>=329 && mx<=329+90 && my>=290 && my<=290+30)
				button = 2;
		}

		if (kbhit())
		{
			ch = getch();
			if (ch=='O' || ch=='o')
				button = 1;
			else if (ch=='C' || ch=='c')
				button = 2;
			else if (ch==13)
			{
				if (button==1 && gamemode!=0)
					{  music(4);  return 8;  } //start game function
				else if (button==2)
					{  music(4);  return 1;  }
			}
			else if (ch=='E' || ch=='e')
			{
				gamemode = 1;
				hidemouseptr();
				screen6_fn2();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 140+2, LIGHTGRAY);
				image(219+10, 140+1, "Graphics\\Easy.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 143, "Easy Mode");

				showmouseptr();
			}
			else if (ch=='N' || ch=='n')
			{
				gamemode = 2;
				hidemouseptr();
				screen6_fn2();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 190+2, LIGHTGRAY);
				image(219+10, 190+1, "Graphics\\Normal.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 193, "Normal Mode");

				showmouseptr();
			}
			else if (ch=='H' || ch=='h')
			{
				gamemode = 3;
				hidemouseptr();
				screen6_fn2();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 240+2, LIGHTGRAY);
				image(219+10, 240+1, "Graphics\\Hard.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 243, "Hard Mode");

				showmouseptr();
			}
			else if (ch==27)
				{  music(4);  return 1;  }
		}
		if ((mb &1)==1)
		{
			if (mx>=219 && mx<=219+90 && my>=290 && my<=290+30 && gamemode!=0)
				{  music(4);  return 8;  } //start game function
			else if (mx>=329 && mx<=329+90 && my>=290 && my<=290+30)
				{  music(4);  return 1;  }
			else if (mx>=219 && mx<=419 && my>=140 && my<=140+34)
			{
				gamemode = 1;
				hidemouseptr();
				screen6_fn2();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 140+2, LIGHTGRAY);
				image(219+10, 140+1, "Graphics\\Easy.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 143, "Easy Mode");

				showmouseptr();
				delay(100);
			}
			else if (mx>=219 && mx<=419 && my>=190 && my<=190+34)
			{
				gamemode = 2;
				hidemouseptr();
				screen6_fn2();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 190+2, LIGHTGRAY);
				image(219+10, 190+1, "Graphics\\Normal.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 193, "Normal Mode");

				showmouseptr();
				delay(100);
			}
			else if (mx>=219 && mx<=419 && my>=240 && my<=240+34)
			{
				gamemode = 3;
				hidemouseptr();
				screen6_fn2();

				setfillstyle(1, LIGHTBLUE);
				floodfill(219+2, 240+2, LIGHTGRAY);
				image(219+10, 240+1, "Graphics\\Hard.txt");
				settextstyle(1, 0, 2);
				setcolor(WHITE);
				outtextxy(219+15+32, 243, "Hard Mode");

				showmouseptr();
				delay(100);
			}

		}

	}
}

int quit(void)
{
	hidemouseptr();

	//inactivate the tile to function on click
	int temp[36], i;
	for (i=0; i<ttn; i++)
	{
		temp[i] = tstatus[i][2];
		tstatus[i][2] = 1;
	}

	setfillstyle(1, BLUE);
	bar(159, 139, 479, 339);
	setcolor(LIGHTBLUE);
	rectangle(159+7, 139+7, 479-7, 339-7);
	rectangle(159+9, 139+9, 479-9, 339-9);

	settextstyle(8, 0, 1);
	setcolor(WHITE);
	outtextxy(159+20, 139+20, "Do you really want to quit?");

	menu_button(319-130, 279, 110, 30);
	menu_button(319+20,  279, 110, 30);

	int txtwidth;
	setcolor(BLUE);
	settextstyle(8, 0, 1);
	txtwidth = textwidth("Yes");
	outtextxy(189+(110-txtwidth)/2, 279, "Yes");
	txtwidth = textwidth("No");
	outtextxy(339+(110-txtwidth)/2, 279, "No");

	showmouseptr();
	int mb, mx, my, button=2;
	char ch;
	while(1)
	{
		getmousepos(&mb, &mx, &my);
		if (button==1)
		{
			setcolor(RED);
			txtwidth = textwidth("Yes");
			outtextxy(189+(110-txtwidth)/2, 279, "Yes");
			delay(50);
			screen5_fn1();
		}
		else if (button==2)
		{
			setcolor(RED);
			txtwidth = textwidth("No");
			outtextxy(339+(110-txtwidth)/2, 279, "No");
			delay(50);
			screen5_fn1();
		}
		if ((mb &0)==0)
		{
			if (mx>=319-130 && mx<=319-130+110  && my>=279  && my<=279+30)
				button = 1;
			else if (mx>=319+20 && mx<=319+20+110 & my>=279 && my<=279+30)
				button = 2;
		}
		if((mb &1)==1)
		{
			if (mx>=319-130 && mx<=319-130+110  && my>=279  && my<=279+30)
				{  music(4);  return 1;  }
			else if (mx>=319+20 && mx<=319+20+110 & my>=279 && my<=279+30)
			{
				//goto previous state
				for (i=0; i<ttn; i++)
					tstatus[i][2] = temp[i];
				music(4);
				return 2;
			}
		}
		if (kbhit())
		{
			ch = getch();
			if (ch=='Y' || ch=='y')
				button = 1;
			else if (ch=='N' || ch=='n')
				button = 2;
			else if (ch==13)
			{
				if (button==1)
					{  music(4);  return 1;  }
				else if(button==2)
				{
					//goto previous state
					for (i=0; i<ttn; i++)
						tstatus[i][2] = temp[i];
					music(4);
					return 2;
				}
			}
			else if (ch==27)
			{
				//goto previous state
				for (i=0; i<ttn; i++)
					tstatus[i][2] = temp[i];
				music(4);
				return 2;
			}
		}
	}
}

int score(void)
{
	hidemouseptr();

	//Draw background
	setbkcolor(BLUE);
	setfillstyle(11, LIGHTBLUE);
	floodfill(1, 1, WHITE);
	round_rectangle(getmaxx()/2-110, getmaxy()/2-150, 220, 300, 10);
	setfillstyle(1, LIGHTBLUE);
	floodfill(209, 89, WHITE);

	//Draw score factor frame
	setfillstyle(1, BLUE);
	bar(209, 90, 429, 120);
	setcolor(WHITE);
	settextstyle(8, 0, 1);
	outtextxy(219, 90, "Score Factors");
	setcolor(BLUE);
	rectangle(209, 90, 429, 205);

	//writing text and calculating score for score factor frame
	//for all three game mode
	setcolor(WHITE);
	settextstyle(2, 0, 0);
	//mp=matches point, mmp=misses point, bp=bonus point,
	//tp=total point, ptp=play time point
	int mp=0, mmp=0, bp=0, tp=0, ptp=0;
	if (gamemode==1)
	{
		outtextxy(219, 125, "Game mode                    Easy");
		outtextxy(219, 140, "For every match                 4");
		outtextxy(219, 155, "For every miss                 -2");
		outtextxy(219, 170, "Bonus point                     6");
		outtextxy(219, 185, "Time bonus for < 45 sec. play time.");
		mp  = m*4;
		mmp = mm*(-2);
		bp  = bonus*6;
		if (playtime<=45)
			ptp = 46-playtime;
		tp  = mp+mmp+bp+ptp;
	}
	else if (gamemode==2)
	{
		outtextxy(219, 125, "Game mode                  Normal");
		outtextxy(219, 140, "For every match                 8");
		outtextxy(219, 155, "For every miss                 -4");
		outtextxy(219, 170, "Bonus point                    12");
		outtextxy(219, 185, "Time bonus for < 90 sec. play time.");
		mp  = m*8;
		mmp = mm*(-4);
		bp  = bonus*12;
		if (playtime<=90)
			ptp = 91-playtime;
		tp  = mp+mmp+bp+ptp;
	}
	else if (gamemode==3)
	{
		outtextxy(219, 125, "Game mode                    Hard");
		outtextxy(219, 140, "For every match                14");
		outtextxy(219, 155, "For every miss                 -7");
		outtextxy(219, 170, "Bonus point                    21");
		outtextxy(219, 185, "Time bonus for < 160 sec. play time.");
		mp  = m*14;
		mmp = mm*(-7);
		bp  = bonus*21;
		if (playtime<=160)
			ptp = 161-playtime;
		tp  = mp+mmp+bp+ptp;
	}

	p1p = tp;
	//draw your score frame
	setfillstyle(1, BLUE);
	bar(209, 225, 429, 255);
	setcolor(WHITE);
	settextstyle(8, 0, 1);
	outtextxy(219, 225, "Your Score");
	setcolor(BLUE);
	rectangle(209, 224, 429, 345);

	setcolor(WHITE);
	settextstyle(2, 0, 0);
	//ch[35] is a temp var. for changing integer values to charecter
	char ch[35], ch1;  //ch for keyboard response
	int mb, mx, my;
	//writing text and points to your score frame
	if (gamemode==1)
	{
		outtextxy(219, 260, "Matches                    =     ");
		outtextxy(390, 260, itoa(mp, ch, 10));
		outtextxy(300, 260, strcat( strcat(itoa(m, ch, 10),"X"), "4" ));

		outtextxy(219, 275, "Misses                     =     ");
		outtextxy(390, 275, itoa(mmp, ch, 10));
		outtextxy(300, 275, strcat( strcat(itoa(mm, ch, 10),"X"), "-2" ));

		outtextxy(218, 290, "Bonus                      =     ");
		outtextxy(390, 290, itoa(bp, ch, 10));
		outtextxy(300, 290, strcat( strcat(itoa(bonus, ch, 10),"X"), "6" ));

		outtextxy(219, 305, "Time Bonus                 =     ");
		outtextxy(390, 305, itoa(ptp, ch, 10));

		outtextxy(219, 312, "_________________________________");
		outtextxy(220, 325, "             Total         =     ");
		outtextxy(390, 325, itoa(tp, ch, 10));
	}
	else if (gamemode==2)
	{
		outtextxy(219, 260, "Matches                    =     ");
		outtextxy(390, 260, itoa(mp, ch, 10));
		outtextxy(300, 260, strcat( strcat(itoa(m, ch, 10),"X"), "8" ));

		outtextxy(219, 275, "Misses                     =     ");
		outtextxy(390, 275, itoa(mmp, ch, 10));
		outtextxy(300, 275, strcat( strcat(itoa(mm, ch, 10),"X"), "-4" ));

		outtextxy(218, 290, "Bonus                      =     ");
		outtextxy(390, 290, itoa(bp, ch, 10));
		outtextxy(300, 290, strcat( strcat(itoa(bonus, ch, 10),"X"), "12" ));

		outtextxy(219, 305, "Time Bonus                 =     ");
		outtextxy(390, 305, itoa(ptp, ch, 10));

		outtextxy(219, 312, "_________________________________");
		outtextxy(220, 325, "             Total         =     ");
		outtextxy(390, 325, itoa(tp, ch, 10));
	}
	else if (gamemode==3)
	{
		outtextxy(219, 260, "Matches                    =     ");
		outtextxy(390, 260, itoa(mp, ch, 10));
		outtextxy(300, 260, strcat( strcat(itoa(m, ch, 10),"X"), "14" ));

		outtextxy(219, 275, "Misses                     =     ");
		outtextxy(390, 275, itoa(mmp, ch, 10));
		outtextxy(300, 275, strcat( strcat(itoa(mm, ch, 10),"X"), "-7" ));

		outtextxy(218, 290, "Bonus                      =     ");
		outtextxy(390, 290, itoa(bp, ch, 10));
		outtextxy(300, 290, strcat( strcat(itoa(bonus, ch, 10),"X"), "21" ));

		outtextxy(219, 305, "Time Bonus                 =     ");
		outtextxy(390, 305, itoa(ptp, ch, 10));

		outtextxy(219, 312, "_________________________________");
		outtextxy(220, 325, "             Total         =     ");
		outtextxy(390, 325, itoa(tp, ch, 10));
	}

	//drawing ok button
	int txtwidth;
	menu_button(265, 358, 110, 30);
	setcolor(RED);
	settextstyle(1, 0, 2);
	txtwidth = textwidth("OK");
	outtextxy(265+(55+txtwidth)/2, 360, "OK");

	showmouseptr();

	while(1)
	{
		getmousepos(&mb, &mx, &my);
		if((mb &1)==1)
		{
			if(mx>=265 && mx<=375 && my>=358 && my<=388)
				{  music(4);  return 7;  }
		}
		if(kbhit())
		{
			ch1 = getch();
			if(ch1==27 || ch1==13)
				{  music(4);  return 7;  }
		}
	}
}

void main(void)
{
	int gd=DETECT, gm;
	initgraph(&gd, &gm, "bgi");
	cleardevice();
	initmouse();
	showmouseptr();
	int_open();

	int screen=0;
	while(1)
	{
		if (screen==0)
			screen = screen0();
		else if (screen==1)
			screen = screen1();
		else if (screen==2)
		{
			screen = screen2();
			setviewport(0, 0, getmaxx(), getmaxy(), 0);
		}
		else if (screen==3)
			screen = screen3();
		else if (screen==4)
		{
			screen = screen4();
			setviewport(0, 0, getmaxx(), getmaxy(), 0);
		}
		else if (screen==5)
			screen = screen5();
		else if (screen==6)
			screen = screen6();
		else if (screen==7)
			screen = score_update();
		else if (screen==8)
		{
			if (playertype==1)
				screen = sp_game();
			else if (playertype==2)
				screen = dp_game();
		}
		else if (screen==9)
			screen = score();
	}
}


//If all criteria fulfill then this function will load a image
void loadimage(void)
{
	hidemouseptr();
	if (tstatus[tn][1]!=1 && tstatus[tn][2]!=1 && tn!=88)
	{
		image(tpos[tn][0], tpos[tn][1], imagefile[tstatus[tn][0]]);
		cli++;
		tstatus[tn][1] = 1;
		music(4);
	}
	showmouseptr();
	delay(200);
}

//determine which tile is clicked and set this value to variable.
void set_tn(void)
{
	int mb, mx, my;
	getmousepos(&mb, &mx, &my);

	if     (mx>=tpos [0][0] && mx<=tpos [0][0]+30 && my>=tpos [0][1] && my<=tpos [0][1]+40)	
			tn =  0;
	else if(mx>=tpos [1][0] && mx<=tpos [1][0]+30 && my>=tpos [1][1] && my<=tpos [1][1]+40)
			tn =  1;
	else if(mx>=tpos [2][0] && mx<=tpos [2][0]+30 && my>=tpos [2][1] && my<=tpos [2][1]+40)
			tn =  2;
	else if(mx>=tpos [3][0] && mx<=tpos [3][0]+30 && my>=tpos [3][1] && my<=tpos [3][1]+40)
			tn =  3;
	else if(mx>=tpos [4][0] && mx<=tpos [4][0]+30 && my>=tpos [4][1] && my<=tpos [4][1]+40)
			tn =  4;
	else if(mx>=tpos [5][0] && mx<=tpos [5][0]+30 && my>=tpos [5][1] && my<=tpos [5][1]+40)
			tn =  5;
	else if(mx>=tpos [6][0] && mx<=tpos [6][0]+30 && my>=tpos [6][1] && my<=tpos [6][1]+40)
			tn =  6;
	else if(mx>=tpos [7][0] && mx<=tpos [7][0]+30 && my>=tpos [7][1] && my<=tpos [7][1]+40)
			tn =  7;
	else if(mx>=tpos [8][0] && mx<=tpos [8][0]+30 && my>=tpos [8][1] && my<=tpos [8][1]+40)
			tn =  8;
	else if(mx>=tpos [9][0] && mx<=tpos [9][0]+30 && my>=tpos [9][1] && my<=tpos [9][1]+40)
			tn =  9;
	else if(mx>=tpos[10][0] && mx<=tpos[10][0]+30 && my>=tpos[10][1] && my<=tpos[10][1]+40)
			tn =  10;
	else if(mx>=tpos[11][0] && mx<=tpos[11][0]+30 && my>=tpos[11][1] && my<=tpos[11][1]+40)
			tn = 11;
	else if(mx>=tpos[12][0] && mx<=tpos[12][0]+30 && my>=tpos[12][1] && my<=tpos[12][1]+40)
			tn = 12;
	else if(mx>=tpos[13][0] && mx<=tpos[13][0]+30 && my>=tpos[13][1] && my<=tpos[13][1]+40)
			tn = 13;
	else if(mx>=tpos[14][0] && mx<=tpos[14][0]+30 && my>=tpos[14][1] && my<=tpos[14][1]+40)
			tn = 14;
	else if(mx>=tpos[15][0] && mx<=tpos[15][0]+30 && my>=tpos[15][1] && my<=tpos[15][1]+40)
			tn = 15;
	//tiles 17-24
	else if(mx>=tpos[16][0] && mx<=tpos[16][0]+30 && my>=tpos[16][1] && my<=tpos[16][1]+40)	tn = 16;
	else if(mx>=tpos[17][0] && mx<=tpos[17][0]+30 && my>=tpos[17][1] && my<=tpos[17][1]+40)	tn = 17;
	else if(mx>=tpos[18][0] && mx<=tpos[18][0]+30 && my>=tpos[18][1] && my<=tpos[18][1]+40)	tn = 18;
	else if(mx>=tpos[19][0] && mx<=tpos[19][0]+30 && my>=tpos[19][1] && my<=tpos[19][1]+40)	tn = 19;
	else if(mx>=tpos[20][0] && mx<=tpos[20][0]+30 && my>=tpos[20][1] && my<=tpos[20][1]+40)	tn = 20;
	else if(mx>=tpos[21][0] && mx<=tpos[21][0]+30 && my>=tpos[21][1] && my<=tpos[21][1]+40)	tn = 21;
	else if(mx>=tpos[22][0] && mx<=tpos[22][0]+30 && my>=tpos[22][1] && my<=tpos[22][1]+40)	tn = 22;
	else if(mx>=tpos[23][0] && mx<=tpos[23][0]+30 && my>=tpos[23][1] && my<=tpos[23][1]+40)	tn = 23;

	//tiles 25-36
	else if(mx>=tpos[24][0] && mx<=tpos[24][0]+30 && my>=tpos[24][1] && my<=tpos[24][1]+40)	tn = 24;
	else if(mx>=tpos[25][0] && mx<=tpos[25][0]+30 && my>=tpos[25][1] && my<=tpos[25][1]+40)	tn = 25;
	else if(mx>=tpos[26][0] && mx<=tpos[26][0]+30 && my>=tpos[26][1] && my<=tpos[26][1]+40)	tn = 26;
	else if(mx>=tpos[27][0] && mx<=tpos[27][0]+30 && my>=tpos[27][1] && my<=tpos[27][1]+40)	tn = 27;
	else if(mx>=tpos[28][0] && mx<=tpos[28][0]+30 && my>=tpos[28][1] && my<=tpos[28][1]+40)	tn = 28;
	else if(mx>=tpos[29][0] && mx<=tpos[29][0]+30 && my>=tpos[29][1] && my<=tpos[29][1]+40)	tn = 29;
	else if(mx>=tpos[30][0] && mx<=tpos[30][0]+30 && my>=tpos[30][1] && my<=tpos[30][1]+40)	tn = 30;
	else if(mx>=tpos[31][0] && mx<=tpos[31][0]+30 && my>=tpos[31][1] && my<=tpos[31][1]+40)	tn = 31;
	else if(mx>=tpos[32][0] && mx<=tpos[32][0]+30 && my>=tpos[32][1] && my<=tpos[32][1]+40)	tn = 32;
	else if(mx>=tpos[33][0] && mx<=tpos[33][0]+30 && my>=tpos[33][1] && my<=tpos[33][1]+40)	tn = 33;
	else if(mx>=tpos[34][0] && mx<=tpos[34][0]+30 && my>=tpos[34][1] && my<=tpos[34][1]+40)	tn = 34;
	else if(mx>=tpos[35][0] && mx<=tpos[35][0]+30 && my>=tpos[35][1] && my<=tpos[35][1]+40)	tn = 35;
	else											tn = 88;

	if(tn>=0 && tn<=35 && tstatus[tn][1]==1 && tstatus[tn][2]==1)                           tn = 88;
}

//randomize the tile image for every tile
void inittiles(int tno)
{
	//x=image const., y=random tsyutas[36][0]
	int x=0, y, i, flag, count=0;

	if (tno==1)	  ttn = 16;
	else if (tno==2)  ttn = 24;
	else		  ttn = 36;

	for (i=0; i<ttn; i++)
	{
		tstatus[i][0] = 99;
		tstatus[i][1] = 0;
		tstatus[i][2] = 0;
	}
	randomize();
	for (i=0; i<ttn; i++)
	{
		flag = 1;
		while(flag)
		{
			y = rand()%ttn;
			if (tstatus[y][0]==99)
			{
				tstatus[y][0] = x;
				count++;
				if (count==2)
				{
					x++;
					count = 0;
				}
				flag = 0;
			}
		}
	}

}

//Draw a tile with WHITE border and BROWN fill.
void round_tiles(int x, int y)
{
	setcolor(WHITE);
	line(x+3, y, x+27, y);
	putpixel(x+2, y+1, WHITE);
	putpixel(x+1, y+2, WHITE);
	line(x, y+3, x, y+37);
	putpixel(x+1, y+38, WHITE);
	putpixel(x+2, y+39, WHITE);
	line(x+3, y+40, x+27, y+40);
	putpixel(x+28, y+39, WHITE);
	putpixel(x+29, y+38, WHITE);
	line(x+30, y+37, x+30, y+3);
	putpixel(x+29, y+2, WHITE);
	putpixel(x+28, y+1, WHITE);

	setfillstyle(1, BROWN);
	floodfill(x+3, y+3, WHITE);
}

//Draws a board of tile and store their position in the tpos[72] arrey.
void board(int type)
{
	int x, y;

	setfillstyle(1, BLUE);
	if (type==1)
	{
		x = (getmaxx()-220)/2;
		y = (getmaxy()-260)/2;
		bar(x, y, x+220, y+260);
	}
	else if (type==2)
	{
		x = (getmaxx()-320)/2;
		y = (getmaxy()-260)/2;
		bar(x, y, x+320, y+260);
	}
	else if (type==3)
	{
		x = (getmaxx()-320)/2;
		y = (getmaxy()-380)/2;
		bar(x, y, x+320, y+380);
	}

	round_tiles(x+20,  y+20);    tpos [0][0] = x+20;  tpos [0][1] = y+20;
	round_tiles(x+70,  y+20);    tpos [1][0] = x+70;  tpos [1][1] = y+20;
	round_tiles(x+120, y+20);    tpos [2][0] = x+120; tpos [2][1] = y+20;
	round_tiles(x+170, y+20);    tpos [3][0] = x+170; tpos [3][1] = y+20;
	round_tiles(x+20,  y+80);    tpos [4][0] = x+20;  tpos [4][1] = y+80;
	round_tiles(x+70,  y+80);    tpos [5][0] = x+70;  tpos [5][1] = y+80;
	round_tiles(x+120, y+80);    tpos [6][0] = x+120; tpos [6][1] = y+80;
	round_tiles(x+170, y+80);    tpos [7][0] = x+170; tpos [7][1] = y+80;
	round_tiles(x+20,  y+140);   tpos [8][0] = x+20;  tpos [8][1] = y+140;
	round_tiles(x+70,  y+140);   tpos [9][0] = x+70;  tpos [9][1] = y+140;
	round_tiles(x+120, y+140);   tpos[10][0] = x+120; tpos[10][1] = y+140;
	round_tiles(x+170, y+140);   tpos[11][0] = x+170; tpos[11][1] = y+140;
	round_tiles(x+20,  y+200);   tpos[12][0] = x+20;  tpos[12][1] = y+200;
	round_tiles(x+70,  y+200);   tpos[13][0] = x+70;  tpos[13][1] = y+200;
	round_tiles(x+120, y+200);   tpos[14][0] = x+120; tpos[14][1] = y+200;
	round_tiles(x+170, y+200);   tpos[15][0] = x+170; tpos[15][1] = y+200;
	if (type==2 || type==3)
	{
		round_tiles(x+220, y+20);    tpos[16][0] = x+220;   tpos[16][1] = y+20;
		round_tiles(x+270, y+20);    tpos[17][0] = x+270;   tpos[17][1] = y+20;
		round_tiles(x+220, y+80);    tpos[18][0] = x+220;   tpos[18][1] = y+80;
		round_tiles(x+270, y+80);    tpos[19][0] = x+270;   tpos[19][1] = y+80;
		round_tiles(x+220, y+140);   tpos[20][0] = x+220;   tpos[20][1] = y+140;
		round_tiles(x+270, y+140);   tpos[21][0] = x+270;   tpos[21][1] = y+140;
		round_tiles(x+220, y+200);   tpos[22][0] = x+220;   tpos[22][1] = y+200;
		round_tiles(x+270, y+200);   tpos[23][0] = x+270;   tpos[23][1] = y+200;
	}
	if (type==3)
	{
		round_tiles(x+20,  y+260);   tpos[24][0] = x+20;    tpos[24][1] = y+260;
		round_tiles(x+70,  y+260);   tpos[25][0] = x+70;    tpos[25][1] = y+260;
		round_tiles(x+120, y+260);   tpos[26][0] = x+120;   tpos[26][1] = y+260;
		round_tiles(x+170, y+260);   tpos[27][0] = x+170;   tpos[27][1] = y+260;
		round_tiles(x+220, y+260);   tpos[28][0] = x+220;   tpos[28][1] = y+260;
		round_tiles(x+270, y+260);   tpos[29][0] = x+270;   tpos[29][1] = y+260;
		round_tiles(x+20,  y+320);   tpos[30][0] = x+20;    tpos[30][1] = y+320;
		round_tiles(x+70,  y+320);   tpos[31][0] = x+70;    tpos[31][1] = y+320;
		round_tiles(x+120, y+320);   tpos[32][0] = x+120;   tpos[32][1] = y+320;
		round_tiles(x+170, y+320);   tpos[33][0] = x+170;   tpos[33][1] = y+320;
		round_tiles(x+220, y+320);   tpos[34][0] = x+220;   tpos[34][1] = y+320;
		round_tiles(x+270, y+320);   tpos[35][0] = x+270;   tpos[35][1] = y+320;
	}
}

//It load an image from a text file.
//The text file denine the image in 0/1 format.
void image(int x, int y, char filename[100])
{
	FILE *f;
	char ch;

	f = fopen(filename, "r");

	int i=x, j=y;
	while ((ch=getc(f))!=EOF)
	{
		if (ch=='1')
			putpixel(i, j, WHITE);
		if (ch!='0' && ch!='1')
		{
			j++;
			i = x;
		}
		i++;
	}
	fclose(f);
}

//Draws a round rectangle with desired curve
void round_rectangle(int x, int y, int w, int h, int round)
{
	setcolor(WHITE);
	arc(x,   y,   90,  180, round);
	arc(x+w, y,   0,   90,  round);
	arc(x,   y+h, 180, 270, round);
	arc(x+w, y+h, 270, 360, round);
	line(x+1,       y-round,   x+w-1,     y-round  );
	line(x-round,   y+1,       x-round,   y+h-1    );
	line(x+1,       y+h+round, x+w-1,     y+h+round);
	line(x+w+round, y+1,       x+w+round, y+h-1    );
	setfillstyle(1, BLUE);
	floodfill(x, y, WHITE);
}

//draws a button
void menu_button(int x, int y, int w, int h)
{
	setfillstyle(1, LIGHTGRAY);
	bar(x, y, x+w, y+h);
	setcolor(WHITE);
	line(x,   y,   x,     y+h-1);
	line(x+1, y,   x+1,   y+h-2);
	line(x+2, y,   x+w,   y);
	line(x+2, y+1, x+w-1, y+1);
	setcolor(DARKGRAY);
	line(x,     y+h,   x+w,   y+h);
	line(x+1,   y+h-1, x+w,   y+h-1);
	line(x+w,   y+1,   x+w,   y+h-1);
	line(x+w-1, y+2,   x+w-1, y+h-1);
}


//Speacial type of text, which has a shadow effect
void credit_title(int y, char text[100])
{
	int txtw;
	settextstyle(8, 0, 1);
	txtw = textwidth(text);
	setcolor(LIGHTGRAY);
	outtextxy((getmaxx()/2)-(txtw/2)+2, y+2, text);
	setcolor(WHITE);
	outtextxy((getmaxx()/2)-(txtw/2), y, text);
	line((getmaxx()/2)-(txtw/2), y+27, (getmaxx()/2)-(txtw/2)+txtw-10, y+27);
}

//Definition for body credit text
void credit_text(int y, char text[100])
{
	int txtw;
	settextstyle(2, 0, 5);
	txtw = textwidth(text);
	setcolor(WHITE);
	outtextxy((getmaxx()/2)-(txtw/2), y, text);
}

//highlight the player name for dp_game
void pturn(int turn)
{
	int txtwidth;
	//drawing player1 and player2
	if (turn==1)
	{
		setfillstyle(1, RED);
		bar(20, 190, 130, 220);
		setfillstyle(1, BLUE);
		bar(510, 190, 620, 220);
		settextstyle(8, 0, 1);
		txtwidth = textwidth("Player 1");
		outtextxy(20+(110-txtwidth)/2, 190, "Player 1");
		txtwidth = textwidth("Player 2");
		outtextxy(510+(110-txtwidth)/2, 190, "Player 2");
	}
	else if (turn==2)
	{
		setfillstyle(1, BLUE);
		bar(20, 190, 130, 220);
		setfillstyle(1, RED);
		bar(510, 190, 620, 220);
		settextstyle(8, 0, 1);
		txtwidth = textwidth("Player 1");
		outtextxy(20+(110-txtwidth)/2, 190, "Player 1");
		txtwidth = textwidth("Player 2");
		outtextxy(510+(110-txtwidth)/2, 190, "Player 2");
	}
}

void screen0_fn1(void)
{
	//the purpose of this function is to set the color of
	//all the buttons text of screen0 to BLUE

	settextstyle(1, 0, 2);
	setcolor(BLUE);
	int txtwidth;

	txtwidth = textwidth("New Game");
	outtextxy(getmaxx()/2-(txtwidth/2), 130, "New Game");
	txtwidth = textwidth("Instructions");
	outtextxy(getmaxx()/2-(txtwidth/2), 170, "Instructions");
	txtwidth = textwidth("High Scores");
	outtextxy(getmaxx()/2-(txtwidth/2), 210, "High Scores");
	txtwidth = textwidth("Credit");
	outtextxy(getmaxx()/2-(txtwidth/2), 250, "Credit");
	txtwidth = textwidth("Exit");
	outtextxy(getmaxx()/2-(txtwidth/2), 290, "Exit");
}
void screen1_fn1(void)
{
	//the purpose of this function is to set the color of
	//all the buttons text of screen1 to BLUE
	int txtwidth;
	settextstyle(1, 0, 2);
	setcolor(BLUE);

	txtwidth = textwidth("OK");
	outtextxy(((getmaxx()/2)-100)+(90-txtwidth)/2, 241, "OK");
	txtwidth = textwidth("Cancel");
	outtextxy(((getmaxx()/2)+10)+(90-txtwidth)/2, 241, "Cancel");
}
void screen3_fn1(void)
{
	int txtwidth;
	setcolor(BLUE);
	settextstyle(1, 0, 2);
	txtwidth = textwidth("Easy");
	outtextxy(370+(90-txtwidth)/2, 136, "Easy");
	txtwidth = textwidth("Normal");
	outtextxy(370+(90-txtwidth)/2, 176, "Normal");
	txtwidth = textwidth("Hard");
	outtextxy(370+(90-txtwidth)/2, 216, "Hard");
	txtwidth = textwidth("Back");
	outtextxy(370+(90-txtwidth)/2, 296, "Back");
}
void screen3_fn2(int button)
{
	int txtwidth;
	//eraseing back
	setfillstyle(1, BLUE);
	bar(174, 135, 174+182, 135+190);
	//preparing the text back for highscore
	setfillstyle(1, LIGHTBLUE);
	bar(174, 135, 174+182, 135+30);
	setcolor(LIGHTBLUE);
	rectangle(174, 135, 174+182, 135+30+160);
	settextstyle(1, 0, 2);
	setcolor(WHITE);

	if (button==1)
	{
		txtwidth = textwidth("Easy Mode");
		outtextxy(174+(182-txtwidth)/2, 136, "Easy Mode");
	}
	else if (button==2)
	{
		txtwidth = textwidth("Normal Mode");
		outtextxy(174+(182-txtwidth)/2, 136, "Normal Mode");
	}
	else if (button==3)
	{
		txtwidth = textwidth("Hard Mode");
		outtextxy(174+(182-txtwidth)/2, 136, "Hard Mode");
	}
}
void screen5_fn1(void)
{
	//the purpose of this function is to set the color of
	//all the buttons text of screen5 to BLUE
	int txtwidth;
	settextstyle(8, 0, 1);
	setcolor(BLUE);

	txtwidth = textwidth("Yes");
	outtextxy(189+(110-txtwidth)/2, 279, "Yes");
	txtwidth = textwidth("No");
	outtextxy(339+(110-txtwidth)/2, 279, "No");
}
void screen6_fn1(void)
{
	//the purpose of this function is to set the color of
	//all the buttons text of screen6 to BLUE
	int txtwidth;
	settextstyle(1, 0, 2);
	setcolor(BLUE);

	txtwidth = textwidth("OK");
	outtextxy(219+(90-txtwidth)/2, 291, "OK");
	txtwidth = textwidth("Cancel");
	outtextxy((319+10)+(90-txtwidth)/2, 291, "Cancel");
}
void screen6_fn2(void)
{
	//draws easy mode
	setfillstyle(1, BLUE);
	floodfill(219+2, 140+2, LIGHTGRAY);
	image(219+10, 140+1, "Graphics\\Easy.txt");
	settextstyle(1, 0, 2);
	setcolor(WHITE);
	outtextxy(219+15+32, 143, "Easy Mode");

	//draw normal mode
	setfillstyle(1, BLUE);
	floodfill(219+2, 190+2, LIGHTGRAY);
	image(219+10, 190+1, "Graphics\\Normal.txt");
	settextstyle(1, 0, 2);
	setcolor(WHITE);
	outtextxy(219+15+32, 193, "Normal Mode");

	//draw hard mode
	setfillstyle(1, BLUE);
	floodfill(219+2, 240+2, LIGHTGRAY);
	image(219+10, 240+1, "Graphics\\Hard.txt");
	settextstyle(1, 0, 2);
	setcolor(WHITE);
	outtextxy(219+15+32, 243, "Hard Mode");
}

void anitext(int x, int y, char text[500])
{
	int w, i;
	gotoxy(x, y);
	w = strlen(text);
	for (i=0; i<w; i++)
	{
		delay(100);
		cprintf("%c", text[i]);
		if (text[i]=='.')
		{
			sound(1000);
			delay(20);
			nosound();
			delay(500);
		}
		if (text[i]!=32 && text[i]!='.')
		{
			sound(1000);
			delay(20);
		}
		nosound();
		if (kbhit())
			break;
	}
}

//Playes different types of music
void music(int type)
{
	float octave[7] = {130.81, 146.83, 164.81, 174.61, 196, 220, 246.94};
	int n, i;

	if (type==1)
	{
		for (i=6; i>=0; i--)
		{
			sound(octave[i]*8);
			delay(30);
		}
		nosound();
	}
	else if(type==2)
	{
		for (i=0; i<15; i++)
		{
			n = random(7);
			sound(octave[n]*4);
			delay(100);
		}
		nosound();
	}
	else if(type==3)
	{
		for (i=4; i>=0; i--)
		{
			sound(octave[i]*4);
			delay(15);
		}
		nosound();
	}
	else if(type==4)
	{
		sound(octave[6]*4);
		delay(50);
		nosound();
	}
	else if(type==5)
	{
		while(!kbhit())
		{
		       n = random(7);
		       sound(octave[n]*4);
		       delay(100);
		}
		nosound();
		if (getch()==0)
			getch();
	}
}

void instext(int i)
{
	settextstyle(2, 0, 0);
	setfillstyle(1, BLUE);
	bar(0, 0, 520, 320);

	outtextxy(0, i+0, "How to navigate instructions page");
	outtextxy(0, i+8, "-------------------------------");
	outtextxy(0, i+23, "Press ESCAPE key to back to the Main Menu. Press UP and DOWN key to scroll through");
	outtextxy(0, i+35, "the text.");

	outtextxy(0, i+60, "Surfing Menu Items");
	outtextxy(0, i+68, "------------------");
	outtextxy(0, i+83, "There is two ways a user can navigate through the menu items in this game. One is by");
	outtextxy(0, i+95, "using the mouse (clicking the buttons) and the other is by using the keyboard. While");
	outtextxy(0, i+107, "using the keyboard use UP and DOWN arrow to go to desired button then press ENTER key.");
	outtextxy(0, i+119, "If user want to back from any state, just  press ESC.");

	outtextxy(0, i+144, "Description of Main Menu screen");
	outtextxy(0, i+152, "-------------------------------");
	outtextxy(0, i+167, "After starting the game there will be five menu items.");
	outtextxy(0, i+179, "     1. New Game");
	outtextxy(0, i+191, "     2. Instructions");
	outtextxy(0, i+203, "     3. High Scores");
	outtextxy(0, i+215, "     4. Credit");
	outtextxy(0, i+227, "     5. Exit");

	outtextxy(0, i+252, "     New Game");
	outtextxy(0, i+260, "     ----------------");
	outtextxy(0, i+275, "     If user selects the New Game from the game menu a new screen will come asking");
	outtextxy(0, i+287, "     whether user want to play a single player game or a double player game. Rules are");
	outtextxy(0, i+299, "     slightly different for a single player game and a double player game. ");

	outtextxy(0, i+324, "     Instructions");
	outtextxy(0, i+332, "     -----------");
	outtextxy(0, i+347, "     In this menu, user will find some useful help about the game. Like rules of playing");
	outtextxy(0, i+359, "     game, scoring system, menu description etc. It is helpful for user who does not");
	outtextxy(0, i+371, "     understand the utility of the game.");

	outtextxy(0, i+396, "     High scores");
	outtextxy(0, i+404, "     -----------");
	outtextxy(0, i+419, "     If user selects High Scores from the Game Menu a new screen will come showing user");
	outtextxy(0, i+431, "     the name of the high scorer in all the three modes (Easy, Normal and Hard).");

	outtextxy(0, i+456, "     Credit");
	outtextxy(0, i+464, "     ------");
	outtextxy(0, i+479, "     If user selects the credit menu from the Game Menu a new screen will come where");
	outtextxy(0, i+491, "     user can see the critiques for this game.");

	outtextxy(0, i+516, "     Exit");
	outtextxy(0, i+524, "     ----");
	outtextxy(0, i+539, "     If user selects the Exit menu from the game a new pop up window will come asking");
	outtextxy(0, i+551, "     if user really wants to quit. If user press y or select yes then a program");
	outtextxy(0, i+563, "     terminates and if user press n or select no the program get back to the Main Menu.");

	outtextxy(0, i+588, "Description of Player Type screen");
	outtextxy(0, i+596, "--------------------------------");
	outtextxy(0, i+611, "Here user can select Player Type. User has to choose a Player Type. In this menu there");
	outtextxy(0, i+623, "are two types of player available:");
	outtextxy(0, i+635, "     A. Single Player");
	outtextxy(0, i+647, "     B. Double Player");

	outtextxy(0, i+672, "     Single Player");
	outtextxy(0, i+680, "     ------------");
	outtextxy(0, i+695, "     If user selects Single Player then he/she will play on their own. The only goal of");
	outtextxy(0, i+707, "     single player game is to get high scores. There are some factors on which high");
	outtextxy(0, i+719, "     score depend. Like: time, bonus point etc.");

	outtextxy(0, i+744, "     Double Player");
	outtextxy(0, i+752, "     -------------");
	outtextxy(0, i+767, "     In double player game two human players can play in one interface. The main goal");
	outtextxy(0, i+779, "     of Double Player game is to match 20 tiles first. Which player gets this target");
	outtextxy(0, i+791, "     first will be the winner in Double Player game.");

	outtextxy(0, i+816, "Description of Game Mode screen");
	outtextxy(0, i+824, "--------------------------------");
	outtextxy(0, i+839, "Here user can select Player Type. User has to choose a Player Type. In this menu there");
	outtextxy(0, i+851, "are two types of Game Mode. Game mode will be found in single/double player. There will");
	outtextxy(0, i+863, "appear three options, Easy, Normal and Hard. If user select Easy mode and press OK, a");
	outtextxy(0, i+875, "game screen will appear. Where there will be 4×4 tiles. Their color is brown. If user");
	outtextxy(0, i+887, "chooses Normal mode there will be 4×6 tiles and in Hard mode there will be 6×6 tiles.");
	outtextxy(0, i+899, "User will found a score board on the left of the game screen. There is counting for");
	outtextxy(0, i+911, "match, miss match, bonus and time.");

	outtextxy(0, i+936, "Playing the game");
	outtextxy(0, i+944, "-----------------");
	outtextxy(0, i+959, "Playing the game is very simple. All user have to do is just click the tile. Then the");
	outtextxy(0, i+971, "tile will turn around showing user a picture then user will click another tile which");
	outtextxy(0, i+983, "will also show user a picture. If the two pictures match then the tiles will be vanished.");
	outtextxy(0, i+995, "And if not the tiles will turn around again. User goal is to match all the tiles of");
	outtextxy(0, i+1007, "the board.");

	outtextxy(0, i+1032, "Single player Menu");
	outtextxy(0, i+1040, "------------------");
	outtextxy(0, i+1055, "If user clicks single player or press s and press OK then another menu will appear. It");
	outtextxy(0, i+1067, "contains three options. If user match two tiles user earn a point and each time user");
	outtextxy(0, i+1079, "mismatch a tiles user lose points. If user matches three sets of tiles in a row user");
	outtextxy(0, i+1091, "will get a bonus point. If user able to beat the high scores then user name will be added");
	outtextxy(0, i+1103, "to the high scores list. There is also a time counting in single player. For Easy mode");
	outtextxy(0, i+1115, "it's 45, for Normal mode it's 90 and for Hard mode it's 160. Match all the tiles in time,");
	outtextxy(0, i+1127, "user will get 1 bonus point for each second left. If user match three one after another,");
	outtextxy(0, i+1139, "user will get bonus.");

	outtextxy(0, i+1164, "Double player Menu");
	outtextxy(0, i+1172, "------------------");
	outtextxy(0, i+1187, "This menu is same as single player menu. In a double player game user play with another");
	outtextxy(0, i+1199, "human player. Users lose user turn for every mismatch and get a point for every match.");
	outtextxy(0, i+1211, "The player who first gets 20 points will win the game. Red color indicates which player");
	outtextxy(0, i+1223, "will play. When a player misses, player turn will change.");
}

//Mouse functions
int initmouse(void)
{
	i.x.ax=0;
	int86(0x33, &i, &o);
	return(o.x.ax);
}
void getmousepos(int *button, int *x, int *y)
{
	i.x.ax = 3;
	int86(0x33, &i, &o);
	*button = o.x.bx;
	*x = o.x.cx;
	*y = o.x.dx;
}
void showmouseptr(void)
{
	i.x.ax = 1;
	int86(0x33, &i, &o);
}
void hidemouseptr(void)
{
	i.x.ax = 2;
	int86(0x33, &i, &o);
}


//Interrepting function
void int_open(void)
{
   disable();
   oldhandler=getvect(INTR);
   setvect(INTR, handler);
   enable();
}

void int_close(void)
{
   disable();
   setvect(INTR, oldhandler);
   enable();
}