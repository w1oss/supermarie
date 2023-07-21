#include <stdio.h>
#include <graphics.h>
#include <iostream>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#pragma comment(lib,"Winmm.lib")
//实现倒计时：
clock_t Start_time;
clock_t End_time;
char str[100];
int time_tt;
//int atend;
//敌人是相对地图静止，
#define KEY_DOWN(vKey) ((GetAsyncKeyState(vKey)&0x8000)?1:0)
#define BLOCKS 100
// 载入PNG图并去透明部分
IMAGE background,castle;
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标
	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
//int intheend=0;
int blo_cnt;
IMAGE beginBk, gameOver;
IMAGE humans[8], f_block[4], grass, a_block[4], cloud1, cloud2, tree;//玛丽有六种形态，左1、左2、右1、右2、跳、死
IMAGE enemy[10], pillar[3];   //不同的敌人列举:乌龟，小怪兽
int gamelevel = 1;//设置游戏关卡；
int life_num = 3;//生命值；
int width = 900, high = 600;
int maps[18][120];//每个数组所占面积为10*10
int maps2[18][120];//两个等级的地图；
int left_i = 2;
int right_i = 0;
int lift, lift_high, low_high; //为1则上升状态，为0则处于不变状态，为-1则处于下降状态。
//记录人物的横纵坐标，实时更新地图状态-》
bool boolenemymove[14];
int isMovemap = 0;
int speed_control;
int human_cnt;
int enemySpeed[14];
int leap[9] = { 49,36,25,16,9,4,1,1,1 };
int cntleap = 0;
int onthewallss;
int isCameraMove = 1;//视角固定,当为0的时候，且human.x大于等于width/2摄像机就会保持移动。
//当马里奥达到width/2的位置的时候，他的横坐标就不变了！
//所以设置一个changeX来确定当他到达width/2的时候，ismovemap++,但是，human.x保持不变；
int changeX = 0;
int isonthewall = 0;
int onthewall = 0;
int tothewall = 0;
int moneyx, moneyy;
int isonthewalls = 0;
int onthewalls = 0;
int last_time;
int ddd = 0;
int tt[14] = { 0 }, tt1[14] = { 0 };
int onit;
int leap_fre;
int right;
int movecnttop, movecntright, movecntleft;
typedef struct Mapsx
{
	double x;
	double y;
	int i;
	int j;
	int isblank;//判断下面两行是否为空的；
	int iscolumn;
	int iswall;
	int type;
}sss;
sss maps_xy[18][120];
typedef struct sRole
{
	double x, y;
	int speed;
	int leap;
	int alive;
	int victory;//击败敌人个数
	int arrow;//朝向问题；
}Role;
Role human;//创建了human这个结构体；
//菜单元素：
IMAGE beginning;
IMAGE munuBackground, menuBlack, beginGame;
typedef struct Enemy
{
	int x, y;
	int alive;
	int arrow;//敌人朝向；
	int speed;//敌人的速度设置；
	int cnt; //确定哪一种敌人；
	int mapsi, mapsj;
	int died;
}Enes;
Enes ene[20];//根据地图上所出现的初始位置设定自己的x，y；

//由于空中的石头的种类比较多，且有坐标元素等,所以采取结构体形式进行制作；
typedef struct Stone
{
	double x, y;
	int alive;//当玛丽变大，他就可以撞碎石头了，石头破碎；
	int mapsi, mapsj;
	int type;
	int appear;
	int leap;
}stone;
stone blo[BLOCKS]; //三种石头：铁箱子，带问号的铁箱子，普通石头
int score;
int isright;
int mapss, money
;//金币和地图的转动； 学下怎么实现动画效果！
//游戏菜单部分：完成！
void bkmusic()
{
	mciSendString(_T("open E:\\"), NULL, 0, NULL);
	mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);
}
void jumpmusic()
{
	mciSendString(_T("open C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\马里奥跳跃.mp3"), NULL, 0, NULL);
	mciSendString(_T("play jpmusic"), NULL, 0, NULL);
}
void login()//登录菜单栏界面
{
	loadimage(&beginBk, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\menubk.jpg"));
	putimage(50, 80, &beginBk);
	settextstyle(30, 0, _T("微软雅黑"));
	settextcolor(RGB(255, 255, 0));
	rectangle(300, 80, 550, 120);
	outtextxy(310, 85, _T("1.游戏开始"));
	rectangle(300, 140, 550, 180);
	outtextxy(310, 145, _T("2.播放音乐"));
	rectangle(300, 200, 550, 240);
	outtextxy(310, 205, _T("3.退出游戏"));
	bool flag = true;
	while (flag)
	{
		if (MouseHit())//判断是否有鼠标信息
		{
			MOUSEMSG msg = GetMouseMsg();
			if (msg.uMsg == WM_LBUTTONDOWN)
			{
				if ((msg.x >= 300 && msg.x <= 550) && (msg.y >= 80 && msg.y <= 120))
				{
					//启动游戏开始函数；
					flag = false;
				}
				else if ((msg.x >= 300 && msg.x <= 550) && (msg.y >= 140 && msg.y <= 180))
				{
					//打开音乐播放；
					bkmusic();
				}
				else if ((msg.x >= 300 && msg.x <= 550) && (msg.y >= 200 && msg.y <= 240))
				{
					exit(0);//退出游戏
				}
				else {
					flag = flag;
				}
			}
		}
	}
}
//游戏结束部分：
void gameover()
{
	loadimage(&gameOver, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\gameover.png"), 400, 300);
	setbkcolor(BLACK);
	cleardevice();
	putimage(30, 30, &gameOver);
	settextstyle(30, 0, _T("微软雅黑"));
	settextcolor(RGB(255, 255, 0));
	rectangle(330, 80, 550, 120);
	outtextxy(340, 85, _T("游戏结束！"));
	rectangle(330, 140, 550, 180);
	settextcolor(YELLOW);
	sprintf_s(str, "您的得分为： % d ", score);
	outtextxy(340, 145, _T(str));
}
//初始化各个物体的位置等参数；//初始化图像，人，敌人，砖块。
//对整个地图的初始化,规定哪个位置放什么
//实现人物碰到砖块,big的撞到就碎掉，碰到问号，出现蘑菇，蘑菇也要移动。(一般出现在人物跳跃的时候)
void isMoveMap()
{
	if (isCameraMove == 1 && human.x >= width / 2)
	{
		isCameraMove = 0;
		isMovemap += 1;//即地图的显示数组加1；
	}
	else if (isCameraMove == 0 && changeX >= 99.6 && changeX <= 100.4)
	{
		isMovemap++;
		changeX = 0;
	}
	else if (human.x < width / 2)
	{
		isCameraMove = 1;
		changeX = 0;
	}
}
void isMoveHuman(int a)
{
	if (human.x < width / 2 && human.x >= 0)
	{
		if (a == 2)
		{
			human.x += 0.3;
		}
	}
	else
	{
		int j, i;
		//human.x保持不变；
		changeX += 2;
		if (changeX % 10 == 0)
		{
			for (i = 0; i < 18; i++)
			{
				for (j = 0; j < 120; j++)
				{
					maps_xy[i][j].x -= 3;//用来确定砖块的x坐标位置；
				}
			}
			for (int k = 0; k < 14; k++)
			{
				if (ene[k].speed != 0 && ene[k].x>-30 && ene[k].alive==1)
				{
					ene[k].x -= 3;
				}
			}
		}
	}
}
void init()
{
	//初始化地图：
	int i, j;
	//设置不同level的地图形式
	if (gamelevel == 1)
	{   //1是地砖，2是小草，3是树木，4是问号砖块，5是普通砖块，6是云朵1，7是云朵2，8是短柱1,12是围成萧墙的砖块
		//9是中柱，10是长柱，20是马里奥，11是怪兽
		int maps1[18][120] = {//初始化地图
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,0,5,5,5,4,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,5,5,5,0,0,0,5,4,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,4,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,5,4,5,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,4,5,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,5,5,0,0,0,4,0,4,0,4,0,0,5,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			2,2,0,0,0,0,0,0,0,0,0,0,11,0,11,0,2,2,0,0,0,0,0,0,0,8,33,0,0,9,0,0,2,11,0,9,33,0,11,0,0,0,0,0,0,2,2,2,0,0,0,0,0,11,0,0,0,11,0,0,0,11,11,0,0,0,11,0,2,2,2,11,11,0,0,0,0,11,0,0,11,0,0,0,0,0,0,0,0,0,0,8,33,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,33,33,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,33,33,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1
		};
		for (i = 0; i < 18; i++)
		{
			for (j = 0; j < 120; j++)
			{
				maps[i][j] = maps1[i][j];
				maps_xy[i][j].x = j * 30;
				maps_xy[i][j].y = i * 30;
				maps_xy[i][j].i = i;
				maps_xy[i][j].j = j;
				maps_xy[i][j].type = maps1[i][j];
				maps_xy[i][j].iswall = 0;
				maps_xy[i][j].iscolumn = 0;
				maps_xy[i][j].isblank = 0;
			}
		}
	}
	//人物：
	loadimage(&humans[0], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\玛丽.png"), 30, 30);
	loadimage(&humans[1], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\玛丽右2.png"), 30, 30);
	loadimage(&humans[2], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\玛丽左1.png"), 30, 30);
	loadimage(&humans[3], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\玛丽左2.png"), 30, 30);
	loadimage(&humans[4], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\big玛丽.png"), 60, 60);
	loadimage(&humans[5], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\跳跃.png"), 30, 30);
	loadimage(&humans[6], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\防护罩玛丽.png"), 30, 30);

	//各种砖块：
	loadimage(&a_block[0], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\可撞碎方块.png"), 30, 30);
	loadimage(&a_block[1], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\宝藏方块.png"), 30, 30);
	loadimage(&a_block[2], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\铁方块.png"), 30, 30);
	loadimage(&f_block[0], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\草地.png"), 30, 30);
	loadimage(&f_block[1], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\地砖.png"), 30, 30);
	loadimage(&f_block[2], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\地砖.png"), 30, 30);

	//敌人：
	loadimage(&enemy[0], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\乌龟.png"), 30, 30);
	loadimage(&enemy[1], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\小怪兽.png"), 30, 30);
	loadimage(&enemy[2], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\小怪兽2.png"), 30, 30);
	loadimage(&enemy[3], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\小怪兽3.png"), 30, 30);
	//背景装饰
	loadimage(&grass, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\草.png"), 30, 30);
	loadimage(&cloud1, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\云层1.png"), 90, 60);
	loadimage(&cloud2, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\云层.png"), 90, 60);
	loadimage(&tree, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\树.png"), 80, 100);

	//柱子
	loadimage(&pillar[0], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\长柱.png"), 60, 120);
	loadimage(&pillar[1], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\中柱.png"), 60, 100);
	loadimage(&pillar[2], _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\短柱.png"), 60, 80);
	loadimage(&background, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\bk.jpg"), 900, 540, 1);
	loadimage(&castle, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\公主城堡.png"), 150, 200);
	human.x = 30;
	human.y = high - 90;
	human.alive = 6;
	human.speed = 0.5;
	int k = 0, k2 = 0;
	for (i = 0; i < 14; i++)
	{
		boolenemymove[i] = true;
	}
	for (j = 0; j < 120; j++)
	{
		for (i = 0; i < 18; i++)
		{
			if (maps_xy[i][j].type == 11)
			{
				ene[k].speed = 0;
				ene[k].mapsi = i;
				ene[k].mapsj = j;
				ene[k].x = -80;
				ene[k].died = 0;
				ene[k].alive = 1;//生命为1，则显示，不为1，则不显示。
				ene[k++].y = i * 30;
			}
			else if (maps_xy[i][j].type == 12)
			{
				maps_xy[i][j].iswall = 1;
			}
			else if (maps_xy[i][j].type >= 8 && maps_xy[i][j].type <= 10)
			{
				maps_xy[i][j].iscolumn = 1;
			}
			else if (maps_xy[i][j].type == 0 && i >= 16)
			{
				maps_xy[i][j].isblank = 1;
			}
		}
	}//初始化各个砖块的位置
	//blo_cnt = k2;
}
void updateWithInput()//根据键盘输入，（还未实现）可以同时输入两个字符
{
	//BeginBatchDraw();
	putimage(human.x, human.y, &humans[left_i]);
	int i;
	bool flag = true;
	int kk = 0;
	//if (intheend == 0)
	//{
		if (GetAsyncKeyState('A') & 0x8000 && human.x>=10)
		{
			if (movecntleft == 3)
			{
				if (left_i == 2)
					human_cnt = 2;
				else
					human_cnt = 3;
				left_i++;
				if (left_i == 4)
				{
					left_i = 2;
				}
				human.x -= 0.3;
				kk = 1;
				movecntleft = 0;
				isright = 1;
			}
			movecntleft++;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			if (movecntright == 3)
			{
				if (right_i == 0)
					human_cnt = 0;
				else
					human_cnt = 1;
				right_i++;
				if (right_i == 2)
				{
					right_i = 0;
				}
				isMoveHuman(2);
				kk = 1;
				movecntright = 0;
				right = 1;
				isright = 0;
			}
			movecntright++;
		}
		if (GetAsyncKeyState('W') & 0x8000)
		{
			if (movecnttop == 8)
			{
				if (leap_fre <= 2)
				{
					lift = 1;
					human_cnt = 5;
					leap_fre++;
					//每跳一次有一次音效：
					jumpmusic();
				}
				kk = 1;
				movecnttop = 0;
			}
			movecnttop++;
		}
		if (kk == 1)
		{
			for (i = 0; i < blo_cnt; i++) {
				if (human.x >= width / 2 - 10 && blo[i].appear == 1)
				{
					blo[i].x -= 0.5;
				}
			}
		}
	//}
	putimage(human.x, human.y, &humans[human_cnt]);
	//EndBatchDraw();
}
//若人物既向上又向右，则huamn.x++；
//实现敌人的出现。 //设定特定位置出现，并且开始行走，若被踩中，则变扁，分数增加
void enemyMove()
{
	int i;
	for (i = 0; i < 14; i++)
	{
		if (ene[i].mapsj <= isMovemap + 30 && boolenemymove[i] == true)
		{
			boolenemymove[i] = false;
			if (i == 0)
				ene[i].x = (ene[i].mapsj * 30) % 900;
			else {
				ene[i].x = 900;
			}
		}
		if (ene[i].x - human.x <= width / 2 && ene[i].x > -30)
		{
			ene[i].speed = 5;
		}
	}
	for (i = 0; i < 14; i++)
	{
		if (ene[i].speed != 0 && ene[i].alive == 1 && ene[i].x>-30)
		{
			maps_xy[ene[i].mapsi][ene[i].mapsj].type = 0;
			if (enemySpeed[i] == 30)
			{
				enemySpeed[i] = 0;
				ene[i].x = ene[i].x - 3;
			}
			enemySpeed[i]++;
		}
		else if (ene[i].x <= -30 && ene[i].x >= -60 && ene[i].alive == 1)
		{
			ene[i].alive = 0;
		}
	}
}
//把地图和人物显示在界面上
void show()
{
	//BeginBatchDraw();
	putimage(0, 0, &background);
	int i, j;
	if (isMovemap+15 < 120)
	{
		for (i = 0; i < 18; i++)
		{
			for (j = isMovemap-3; j < 30 + isMovemap; j++)
			{

				int k;
				k = j - isMovemap;
				if (maps_xy[i][j].type == 1)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &f_block[1]);
					//drawAlpha(&f_block[1], maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 2)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &grass);
					drawAlpha(&grass, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 3)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &tree);
					drawAlpha(&tree, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 4)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[1]);
				}
				else if (maps_xy[i][j].type == 5)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[0]);
				}
				else if (maps_xy[i][j].type == 6)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud1);

					drawAlpha(&cloud1, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 7)
				{
					//putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud2);

					drawAlpha(&cloud2, maps_xy[i][j].x, maps_xy[i][j].y);
				}
				else if (maps_xy[i][j].type == 8)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[2]);
				}
				else if (maps_xy[i][j].type == 9)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[1]);
				}
				else if (maps_xy[i][j].type == 10)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[0]);
				}
				else if (maps_xy[i][j].type == 12)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[2]);
				}
			}
		}
		for (i = 0; i < 14; i++)
		{
			if (ene[i].speed != 0 && ene[i].alive == 1)
			{
				drawAlpha(&enemy[0], ene[i].x, ene[i].y);
				//putimage(ene[i].x, ene[i].y, &enemy[0]);
			}
		}
		//drawAlpha(&humans[human_cnt], human.x, human.y);
		putimage(human.x, human.y, &humans[human_cnt]);
		if (tothewall == 1)
		{
			isonthewall++;
			if (isonthewall == 40)
			{
				human.y += 60;
				tothewall = 0;
				isonthewall = 0;
			}
		}
	}
	else {
		//drawAlpha(&castle, width - 150, high - 150);
		ddd++;
		if (ddd >= 6)
		{
			isMovemap = 120;
		}
		putimage(width - 200, high - 260, &castle);
		for (i = 0; i < 18; i++)
		{
			for (j = 105; j < 120; j++)
			{
				if (maps_xy[i][j].type == 1)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &f_block[1]);
				}
				else if (maps_xy[i][j].type == 2)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &grass);
				}
				else if (maps_xy[i][j].type == 3)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &tree);
				}
				else if (maps_xy[i][j].type == 4)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[1]);
				}
				else if (maps_xy[i][j].type == 5)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[0]);
				}
				else if (maps_xy[i][j].type == 6)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud1);
				}
				else if (maps_xy[i][j].type == 7)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &cloud2);
				}
				else if (maps_xy[i][j].type == 8)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[2]);
				}
				else if (maps_xy[i][j].type == 9)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[1]);
				}
				else if (maps_xy[i][j].type == 10)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &pillar[0]);
				}
				else if (maps_xy[i][j].type == 12)
				{
					putimage(maps_xy[i][j].x, maps_xy[i][j].y, &a_block[2]);
				}
			}
		}
		for (i = 0; i < 14; i++)
		{
			if (ene[i].speed != 0 && ene[i].alive == 1)
			{
				//drawAlpha(&enemy[0], ene[i].x, ene[i].y);
				putimage(ene[i].x, ene[i].y, &enemy[0]);
			}
			else if (ene[i].died >= 1 && ene[i].died <= 20)
			{
				//drawAlpha(&enemy[0], ene[i].x, ene[i].y);
				ene[i].died += 1;
			}
		}
		putimage(human.x, human.y, &humans[human_cnt]);
		if (tothewall == 1)
		{
			isonthewall++;
			if (isonthewall == 40)
			{
				human.y += 60;
				tothewall = 0;
				isonthewall = 0;
			}
		}
	}
	End_time = clock();
	time_tt = (int)((End_time - Start_time) / CLOCKS_PER_SEC);
	settextcolor(BLACK);
	sprintf_s(str, "生命值： %d ", human.alive / 2);
	settextcolor(BLACK);
	outtextxy(100, 20, _T(str));

	sprintf_s(str, "倒计时： %d s", 300 - time_tt);
	settextcolor(BLACK);
	outtextxy(700, 20, _T(str));

	sprintf_s(str, "金币： %d ", score);
	settextcolor(BLACK);
	outtextxy(500, 20, _T(str));
	if (money == 1)
	{
		if (last_time <=120)
		{
			IMAGE money;
			loadimage(&money, _T("C:\\Users\\Lenovo\\Desktop\\超级玛丽素材\\money.png"), 30, 30);
			drawAlpha(&money, moneyx, moneyy-30);
			last_time++;
		}
		else if (last_time > 120)
		{
			money = 0;
			score += 10;
			last_time = 0;
		}
	}
	//EndBatchDraw();
}
void updateWithoutInput()
{
	//4、5是砖块！
	//来判断生命值丢失等等问题；
	int i, j, m;
	for (i = 0; i < 14; i++)
	{
		if (tt[i] == 1)
		{
			tt1[i]++;
		}
		if (tt1[i] == 60)
		{
			tt[i] = 0;
		}
	}
	for (m = 0; m < 14; m++)
	{
		if (ene[m].speed != 0 && ene[m].alive == 1)
		{
			if (tt[m] == 0 && ene[m].x <= human.x + 30 && ene[m].x >= human.x && human.y >= ene[m].y - 2 && human.y <= ene[m].y + 2)
			{
				tt[m] = 1;
				human.alive -= 1;
				//此时人物获得重力加速度；下降
				if (human.alive == 0)
				{
					for (i = 0; i < 10; i++)
					{
						human.y += 0.9;
						FlushBatchDraw();
					}
					gameover();
					system("pause");
					isMovemap = 100;
				}
			}
			if (ene[m].x <= human.x + 30 && ene[m].x >= human.x && human.y<= ene[m].y && human.y>=ene[m].y-60)
			{
				ene[m].alive = 0;
				ene[m].died = 1;
				//ene[m].y += 120;
			}
		}//如果脚下没有砖块就掉落生命减一；人或enemy；完成！
		for (i = 0; i < 18; i++)
		{
			for (j = 0; j < 120; j++)
			{
				if (maps_xy[i][j].isblank == 1)
				{
					if (ene[m].x - maps_xy[i][j].x <= 2 && ene[m].x - maps_xy[i][j].x >= 0)
					{
						int lm;
						ene[m].alive = 0;
						ene[m].y += 120;
					}
					if (human.x > maps_xy[i][j].x+5 && human.y >= maps_xy[i][j].y && human.x < maps_xy[i][j].x+25)
					{
						int lm;
						human.alive -= 1;
						human.y -= 30;
						FlushBatchDraw();
						if (human.alive == 0)
						{
							human.y += 120;
							isMovemap = 140;
						}
						else {
							human.x -= 90;
							human.y = high - 90;
							/*settextcolor(BLACK);
							sprintf_s(str, "你已丢失一条生命,按键继续！");
							settextcolor(BLACK);
							outtextxy(300, 150, _T(str));
							*/
						}
					}
				}
				//判断人前面是否有障碍物；柱子和墙壁,完成：
				if (human.x + 30 >= maps_xy[i][j].x && human.x + 30 <= maps_xy[i][j].x+2 && (maps_xy[i][j].iswall == 1 || maps_xy[i][j].iscolumn == 1) && human.y >= maps_xy[i][j].y-2)
				{
					human.x -= 5;//此时没办法继续往前走；
				}
				else if (human.x >= maps_xy[i][j].x + 60 && human.x <= maps_xy[i][j].x + 62 && maps_xy[i][j].iscolumn == 1 && human.y >= maps_xy[i][j].y-2 && human_cnt >= 2 && human_cnt <= 3)
				{
					human.x += 5;
				}
				else if (human.x >= maps_xy[i][j].x + 30 && maps_xy[i][j].iswall == 1 && human.y >= maps_xy[i][j].y-2 && human_cnt >= 2 && human_cnt <= 3)
				{
					human.x += 5;
				}
				else if ((maps_xy[i][j].type == 4 || maps_xy[i][j].type == 5) && human.x >= maps_xy[i][j].x + 30 && human.x <= maps_xy[i][j].x + 32 && human.y >= maps_xy[i][j].y - 2 && human.y <= maps_xy[i][j].y + 2)
				{
					human.x += 5;
				}
				else if ((maps_xy[i][j].type == 4 || maps_xy[i][j].type == 5) && human.x + 30 <= maps_xy[i][j].x + 1 && human.x + 30 >= maps_xy[i][j].x - 1 && human.y >= maps_xy[i][j].y - 2 && human.y <= maps_xy[i][j].y + 2)
				{
					human.x -= 5;
				}
			}
		}
	}
	int first = 0;
	int second = 0;
	speed_control++;
	if (speed_control == 20)
	{
		if (lift == 1 && cntleap <= 8)
		{
			if (right == 1 && human.x <= width / 2 - 3)
			{
				human.x += 2;
			}
			for (int j = 0; j <= cntleap * 5; j++)
			{
				int i;
			}
			human.y -= leap[cntleap];
			cntleap++;
		}
		else if (lift == 1 && cntleap == 9 || cntleap >= 0 && lift != 0)
		{
			if (right == 1 && human.x <= width / 2 - 3)
			{
				human.x += 2;
			}
			for (int j = 0; j <= cntleap * 3; j++)
			{
				int i;
			}
			lift = -1;
			if (cntleap < 9)
				human.y += leap[cntleap];
			cntleap--;
		}
		else if (lift == -1 && cntleap == -1 || cntleap == -1)
		{
			right = 0;
			if (isright == 1)
				cntleap = -1;
			else
				cntleap = 0;
			lift = 0;
			leap_fre = 0;
		}
		else if (lift == 0)
		{
			human_cnt = human_cnt;
		}
		speed_control = 0;
		for (i = 0; i < 18; i++)
		{
			for (j = 0; j < 120; j++)
			{
				//上跳的所有功能汇集：
				//4，5成功！
				if (maps_xy[i][j].type == 4 || maps_xy[i][j].type == 5)
				{   //碰到砖块：挡住不继续上跳：完成
					if (human.y >= maps_xy[i][j].y + 35 && human.y - leap[cntleap] <= maps_xy[i][j].y + 30 && cntleap <= 8 && human.x >= maps_xy[i][j].x-30 && human.x <= maps_xy[i][j].x + 30 && lift == 1 && tothewall == 0)
					{
						human.y = maps_xy[i][j].y + 30;
						lift = 0;
						cntleap = -1;
						tothewall = 1;
						if (maps_xy[i][j].type == 4)
						{
							moneyx = maps_xy[i][j].x;
							moneyy = maps_xy[i][j].y;
							//展示金币并且总得分加一加：
							money = 1;
						}
					}	//在砖块上方，则可以站在上面并继续跳跃:
					if (lift==-1)
					{
							if (human.y + 30 <= maps_xy[i][j].y && human.y >= maps_xy[i][j].y - 50 && (human.x >= maps_xy[i][j].x - 30 && human.x <= maps_xy[i][j].x + 30 && isright==0 || human.x >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x+30 && isright==1))
							{
								if (first == 0)
								{
									cntleap = -1;
									lift = 0;
									human.y = maps_xy[i][j].y - 30;
									first = 1;
								}
								else if (second == 0 && first==1)
								{
									cntleap = -1;
									lift = 0;
									human.y = maps_xy[i][j].y - 30;
									second = 1;
								}
							}
						//保持在砖块上方；
					}
				}
				else if (maps_xy[i][j].iscolumn == 1 && lift == -1)//碰到柱子或者是墙壁则进行判断：
				{
					if (human.y + 30 <= maps_xy[i][j].y && human.y >= maps_xy[i][j].y - 50)
					{
						if (human.x + 30 >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x + 60 && isright == 0 || human.x <= maps_xy[i][j].x + 62 && human.x >= maps_xy[i][j].x && isright == 1)
						{
							human.y = maps_xy[i][j].y - 30;
							cntleap = -1;
							onthewalls = 1;
						}
					}
					/*else if (human.x + 30 < maps_xy[i][j].x || human.x > maps_xy[i][j].x + 30)
					{
						maps_xy[i][j].iscolumn = 0;
					}*/
				}
				else if (maps_xy[i][j].iswall == 1 && lift == -1)
				{
					if ((human.x + 30 >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x+3 && isright == 0 || human.x >= maps_xy[i][j].x && human.x <= maps_xy[i][j].x + 3 && isright == 1) && human.y + 30 <= maps_xy[i][j].y && human.y >= maps_xy[i][j].y - 50)
					{
						//正在下落中
						human.y = maps_xy[i][j].y - 30;
						cntleap = -1;
						onthewallss += 1;
						//再判断是否输入了跳跃这个按键->按需改变lift和cntleap的值
					}
				}
				if ((human.x > maps_xy[i][j].x + 60 && human.x<maps_xy[i][j].x + 65 && isright==0 || human.x + 30 <= maps_xy[i][j].x && human.x>=maps_xy[i][j].x - 40 && isright==1) && maps_xy[i][j].iscolumn == 1 && human.y + 30 >= maps_xy[i][j].y && onthewalls >= 1)
				{
					human.y = high - 90;//还要预判下一个柱子在哪，其他的障碍物等等
					cntleap = -1;
					onthewalls = 0;
					if (isright == 1)
					{
						human.x -= 4;
					}
					else{
						human.x += 4;
					}
				}
				//判断下落！
				if (tothewall == 0 && onthewalls==0 && i <= 16 && human.x >= maps_xy[i][j - 1].x && human.x <= maps_xy[i][j - 1].x + 32 && human.y >= maps_xy[i][j - 1].y - 1 && human.y <= maps_xy[i][j - 1].y + 1 && maps_xy[i + 1][j - 1].type != 1 && lift == 0)
				{
					if (maps_xy[i + 1][j-1].type == 0 || maps_xy[i+ 1][j-1].type < 4 && maps_xy[i + 1][j-1].type>1 || maps_xy[i + 1][j-1].type > 5 && maps_xy[i + 1][j-1].type <= 7 || maps_xy[i + 1][j-1].type == 11 )
					{

						if (maps_xy[i + 1][j].type >= 4 && maps_xy[i + 1][j].type <= 5 && human.x + 30 > maps_xy[i][j].x)
						{
							i = i;
						}
						else {
							//human.x += 4;
							if (isright == 1 && human.x +30 <= maps_xy[i][j].x)
							{
								human.x -= 4;
							}
							else if(isright==0 && human.x>= maps_xy[i][j].x+30)
							{
								human.x += 4;
							}
							human.y += 30;
						}
					}
				}
			}
		}
	}
}
int main()
{
	//采用数组的方式来实现超级马里奥；
	initgraph(900, 540);
	width = 900;
	high = 540;
	setbkcolor(WHITE);
	cleardevice();
	login();
	init();
	setbkcolor(WHITE);
	cleardevice();
	Start_time = clock();
	BeginBatchDraw();
	while (isMovemap+10 < 120)
	{
		isMoveMap();
		updateWithInput();
		updateWithoutInput();
		enemyMove();
		setbkcolor(WHITE);
		cleardevice();
		show();
		FlushBatchDraw();
	}
	int i;
	for (i = 0; i < 20; i++)
	{
		gameover();
		FlushBatchDraw();
		Sleep(500);
	}
	_kbhit();
	Sleep(500);
	EndBatchDraw();
	closegraph();
	return 0;
}