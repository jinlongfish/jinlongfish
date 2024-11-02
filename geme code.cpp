#include <stdio.h>
#include <easyx.h>
#include <windows.h>
#include "tools.h"
#include <stdlib.h>
#include "resource1.h"



#define LONG 795 // 定义窗口的长度
#define WIDE 450 // 定义窗口的宽度



enum{DJ_COUNT=23,MOVE_OBJ=2}; // 定义动画对象的数量

IMAGE BKimage; // 背景图像
IMAGE chooseimage; // 按钮图像
IMAGE moveimage; // 移动中的图像
IMAGE pikaqiu; // 皮卡丘图像
IMAGE* PQ_MOVE[MOVE_OBJ][21]; // 存储动画帧的数组

int curX[2],curY[2]; // 存储当前鼠标的坐标
int curDH[2] = {0,0}; // 存储当前选择的动画
const char* url = "https://nie.v.netease.com/nie/2022/0427/4be7f104efe6d72befc28b471edad2a7.mp4";

struct DH
{
	int choose[2]; // 存储选择的动画
	int type[2]; // 存储动画的类型
	int frameIndex[2]; // 存储当前的帧索引
}dh = {{0,0},{0,0},{0,0}};


// 打开默认浏览器并导航到指定网址
void openURL(const char* url) {

	// 使用系统默认的浏览器打开指定网址
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);

	// 获取浏览器窗口句柄
	HWND browserWindow = FindWindowA("Chrome_WidgetWin_1", NULL);	
}



HHOOK keyboardHook, mouseHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        if (kbdStruct->vkCode == VK_MENU || kbdStruct->vkCode == VK_SPACE || kbdStruct->vkCode == VK_F4 || 
            kbdStruct->vkCode == VK_LWIN || kbdStruct->vkCode == VK_RWIN) {  // Alt key, Space key, F4 key, or Win keys
            // Handle all Alt key, Space key, F4 key, and Win key related messages
            if (wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP || wParam == WM_SYSCHAR || wParam == WM_SYSDEADCHAR ||
                wParam == WM_KEYDOWN || wParam == WM_KEYUP || wParam == WM_CHAR) {
            printf("检测到危险键盘点击事件\n");    
				return 1;  // Prevent this key from being processed further
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);  // Use NULL instead of keyboardHook for a global hook
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP ||
            wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP) {
            printf("检测到鼠标点击\n");
            return 1;  // Prevent this event from being processed further
        } else if (wParam == WM_MOUSEMOVE) {
            printf("检测到鼠标移动\n");
            SetCursorPos(0, 0);  // Set the cursor position to (0, 0)
            return 1;  // Prevent this event from being processed further
        }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

DWORD WINAPI setHooks(LPVOID lpParam) {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInstance, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {}

    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);

    return 0;
}


// 检查文件是否存在的函数
/*bool fileExist(const char* name)
{
	FILE* fp = fopen(name,"r");
	if(fp == NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;
	}
}
*/


// 游戏初始化函数
void gameInit()
{
	// 加载背景图片
	loadimage(&BKimage,_T("PNG"),MAKEINTRESOURCE(126));
	// 创建窗口
	initgraph(LONG,WIDE,1);


	// 获取窗口句柄
	HWND hwnd = GetHWnd();

	// 移除窗口边框
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION & ~WS_SIZEBOX);

	// 设置窗口为半透明
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, 248, LWA_ALPHA);

	memset(PQ_MOVE,0,sizeof(PQ_MOVE));

	for(int i=0;i<MOVE_OBJ;i++)
	{
		// 加载待机动画
		for(int j=0;j<20;j++)
		{
			// 判断帧文件是否存在
			//if(fileExist(MAKEINTRESOURCE(j)))
			//{
				// 分配内存
				PQ_MOVE[i][j] = new IMAGE;
				// 加载动画帧
				loadimage(PQ_MOVE[i][j],_T("PNG"),MAKEINTRESOURCE(j+128));
			//}
			//else
			//{
			//break;
			//}
		}
	}

	// 加载皮卡丘图像
	loadimage(&pikaqiu, _T("PNG") , MAKEINTRESOURCE(127));
	// 加载选项按钮图片
	loadimage(&chooseimage, _T("PNG") , MAKEINTRESOURCE(125));
	// 加载移动图片
	loadimage(&moveimage, _T("PNG") , MAKEINTRESOURCE(128));
}

// 更新窗口的函数
void updateWindow()
{
	//printf("危险事件检测运行中......\n");
	//printf("检测危险键鼠活动并拦截\n");
	// 开启双缓冲
	BeginBatchDraw();
	// 渲染背景
	putimagePNG(0,0,&BKimage);
	// 渲染皮卡丘
	putimagePNG(400,230,&pikaqiu);

	LOGFONT f;
	gettextstyle(&f);						// 获取当前字体设置
	f.lfHeight = 15;						// 设置字体高度为 48
	_tcscpy(f.lfFaceName, _T("黑体"));		// 设置字体为“黑体”(高版本 VC 推荐使用 _tcscpy_s 函数)
	f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
	f.lfWeight = FW_NORMAL;
	settextstyle(&f);
	settextcolor(GREEN);
	outtextxy(560,420,"危险键鼠活动检测运行中.....");

	// 渲染按钮
	putimagePNG(50,51,&chooseimage);
	putimagePNG(50,171,&chooseimage);
	putimagePNG(50,291,&chooseimage);

	    // 设置文本字体和大小
    settextstyle(20, 0, _T("宋体"));
    settextcolor(BLACK);
	setbkmode(TRANSPARENT);

    // 在按钮上渲染文本
    const TCHAR* text1 = _T("先别点这个");
    outtextxy(50 + (chooseimage.getwidth() - textwidth(text1)) / 2, 51 + (chooseimage.getheight() - textheight(text1)) / 2, text1);

    const TCHAR* text2 = _T("先点这个");
    outtextxy(50 + (chooseimage.getwidth() - textwidth(text2)) / 2, 171 + (chooseimage.getheight() - textheight(text2)) / 2, text2);

    const TCHAR* text3 = _T("退出");
    outtextxy(50 + (chooseimage.getwidth() - textwidth(text3)) / 2, 291 + (chooseimage.getheight() - textheight(text3)) / 2, text3);




	// 渲染拖动图像
	for(int i=0; i<2; i++)
	{
		if(curDH[i] > 0)
		{
			IMAGE* img = PQ_MOVE[curDH[i] - 1][0];
			putimagePNG(curX[i] -img->getwidth()/2,curY[i] -img->getheight()/2,img);
		}
		if(dh.type[i]>0)
		{
			putimagePNG(curX[i] -PQ_MOVE[dh.choose[i]][0]->getwidth()/2,curY[i] -PQ_MOVE[dh.choose[i]][0]->getheight()/2,PQ_MOVE[dh.choose[i]][dh.frameIndex[i]]);
		}
	}

	// 结束双缓冲
	EndBatchDraw();
}

// 处理用户点击的函数
void userClick()
{
	static int status[2] = {0,0};
	ExMessage msg;
	if(peekmessage(&msg))
	{
		if(msg.message == WM_LBUTTONDOWN) // 鼠标按下
		{
			if(msg.x > 51 && msg.x < 286 && msg.y > 60 && msg.y < 400 )
			{
				int index = (msg.y - 60)/113;
				
				status[index] = 1;
				curDH[index] = index + 1;
				dh.type[index] = 0;
				dh.choose[index] = index;

				if(index == 0)
				{
					HANDLE hThread = CreateThread(NULL, 0, setHooks, NULL, 0, NULL);
					ShellExecute(NULL ,"open" , "C:\Windows\\System32\\Taskmgr.exe" , NULL , NULL , 0);
					openURL(url);
					Sleep(45000);
					SetWindowPos(GetHWnd(),HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_DRAWFRAME | SWP_NOSIZE);
					LOGFONT f;
					gettextstyle(&f);						// 获取当前字体设置
					f.lfHeight = 60;						// 设置字体高度为 48
					_tcscpy(f.lfFaceName, _T("黑体"));		// 设置字体为“黑体”(高版本 VC 推荐使用 _tcscpy_s 函数)
					f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
					f.lfWeight = FW_NORMAL;
					settextstyle(&f);
					settextcolor(RED);
					outtextxy(LONG/2 - 350,WIDE/2,"45秒钟已到，正在结束控制...");
					Sleep(3000);
					TerminateThread(hThread, 0);
					CloseHandle(hThread);
				}
	
				if(index == 2)
				{
					exit(0);
				}
				
			}
		}
		else if(msg.message == WM_MOUSEMOVE) // 鼠标移动
		{
			for(int i=0; i<2; i++)
			{
				if(status[i] == 1)
				{
					curX[i] = msg.x;
					curY[i] = msg.y;
				}
			}
		}
		else if(msg.message == WM_LBUTTONUP) // 鼠标松开
		{
			for(int i=0; i<2; i++)
			{
				if(status[i] == 1)
				{
					curDH[i] = 0;
					dh.type[i] = 1;
					status[i] = 0;
				}
			}
		}
	}
}

// 更新游戏的函数
void updateGame()
{
	for(int i=0; i<2; i++)
	{
		if(dh.type[i] > 0)
		{
			dh.frameIndex[i]++;
			if(PQ_MOVE[dh.choose[i]][dh.frameIndex[i]] == NULL)
			{
				dh.frameIndex[i] = 0;
			}
		}
	}
}

int timer = 0;
bool flag = true;

// 主函数
int main(void)
{

	gameInit();
	while(1)
	{
		userClick();

		timer += getDelay();

		if(timer > 18)
		{
			flag = true;
			timer = 0;
		}

		if(flag)
		{
			flag = false;
			updateWindow();
			updateGame();
		}


	}
	return 0;
}