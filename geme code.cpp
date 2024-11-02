#include <stdio.h>
#include <easyx.h>
#include <windows.h>
#include "tools.h"
#include <stdlib.h>
#include "resource1.h"



#define LONG 795 // ���崰�ڵĳ���
#define WIDE 450 // ���崰�ڵĿ��



enum{DJ_COUNT=23,MOVE_OBJ=2}; // ���嶯�����������

IMAGE BKimage; // ����ͼ��
IMAGE chooseimage; // ��ťͼ��
IMAGE moveimage; // �ƶ��е�ͼ��
IMAGE pikaqiu; // Ƥ����ͼ��
IMAGE* PQ_MOVE[MOVE_OBJ][21]; // �洢����֡������

int curX[2],curY[2]; // �洢��ǰ��������
int curDH[2] = {0,0}; // �洢��ǰѡ��Ķ���
const char* url = "https://nie.v.netease.com/nie/2022/0427/4be7f104efe6d72befc28b471edad2a7.mp4";

struct DH
{
	int choose[2]; // �洢ѡ��Ķ���
	int type[2]; // �洢����������
	int frameIndex[2]; // �洢��ǰ��֡����
}dh = {{0,0},{0,0},{0,0}};


// ��Ĭ���������������ָ����ַ
void openURL(const char* url) {

	// ʹ��ϵͳĬ�ϵ��������ָ����ַ
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);

	// ��ȡ��������ھ��
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
            printf("��⵽Σ�ռ��̵���¼�\n");    
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
            printf("��⵽�����\n");
            return 1;  // Prevent this event from being processed further
        } else if (wParam == WM_MOUSEMOVE) {
            printf("��⵽����ƶ�\n");
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


// ����ļ��Ƿ���ڵĺ���
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


// ��Ϸ��ʼ������
void gameInit()
{
	// ���ر���ͼƬ
	loadimage(&BKimage,_T("PNG"),MAKEINTRESOURCE(126));
	// ��������
	initgraph(LONG,WIDE,1);


	// ��ȡ���ھ��
	HWND hwnd = GetHWnd();

	// �Ƴ����ڱ߿�
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION & ~WS_SIZEBOX);

	// ���ô���Ϊ��͸��
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, 248, LWA_ALPHA);

	memset(PQ_MOVE,0,sizeof(PQ_MOVE));

	for(int i=0;i<MOVE_OBJ;i++)
	{
		// ���ش�������
		for(int j=0;j<20;j++)
		{
			// �ж�֡�ļ��Ƿ����
			//if(fileExist(MAKEINTRESOURCE(j)))
			//{
				// �����ڴ�
				PQ_MOVE[i][j] = new IMAGE;
				// ���ض���֡
				loadimage(PQ_MOVE[i][j],_T("PNG"),MAKEINTRESOURCE(j+128));
			//}
			//else
			//{
			//break;
			//}
		}
	}

	// ����Ƥ����ͼ��
	loadimage(&pikaqiu, _T("PNG") , MAKEINTRESOURCE(127));
	// ����ѡ�ťͼƬ
	loadimage(&chooseimage, _T("PNG") , MAKEINTRESOURCE(125));
	// �����ƶ�ͼƬ
	loadimage(&moveimage, _T("PNG") , MAKEINTRESOURCE(128));
}

// ���´��ڵĺ���
void updateWindow()
{
	//printf("Σ���¼����������......\n");
	//printf("���Σ�ռ���������\n");
	// ����˫����
	BeginBatchDraw();
	// ��Ⱦ����
	putimagePNG(0,0,&BKimage);
	// ��ȾƤ����
	putimagePNG(400,230,&pikaqiu);

	LOGFONT f;
	gettextstyle(&f);						// ��ȡ��ǰ��������
	f.lfHeight = 15;						// ��������߶�Ϊ 48
	_tcscpy(f.lfFaceName, _T("����"));		// ��������Ϊ�����塱(�߰汾 VC �Ƽ�ʹ�� _tcscpy_s ����)
	f.lfQuality = ANTIALIASED_QUALITY;		// �������Ч��Ϊ�����  
	f.lfWeight = FW_NORMAL;
	settextstyle(&f);
	settextcolor(GREEN);
	outtextxy(560,420,"Σ�ռ������������.....");

	// ��Ⱦ��ť
	putimagePNG(50,51,&chooseimage);
	putimagePNG(50,171,&chooseimage);
	putimagePNG(50,291,&chooseimage);

	    // �����ı�����ʹ�С
    settextstyle(20, 0, _T("����"));
    settextcolor(BLACK);
	setbkmode(TRANSPARENT);

    // �ڰ�ť����Ⱦ�ı�
    const TCHAR* text1 = _T("�ȱ�����");
    outtextxy(50 + (chooseimage.getwidth() - textwidth(text1)) / 2, 51 + (chooseimage.getheight() - textheight(text1)) / 2, text1);

    const TCHAR* text2 = _T("�ȵ����");
    outtextxy(50 + (chooseimage.getwidth() - textwidth(text2)) / 2, 171 + (chooseimage.getheight() - textheight(text2)) / 2, text2);

    const TCHAR* text3 = _T("�˳�");
    outtextxy(50 + (chooseimage.getwidth() - textwidth(text3)) / 2, 291 + (chooseimage.getheight() - textheight(text3)) / 2, text3);




	// ��Ⱦ�϶�ͼ��
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

	// ����˫����
	EndBatchDraw();
}

// �����û�����ĺ���
void userClick()
{
	static int status[2] = {0,0};
	ExMessage msg;
	if(peekmessage(&msg))
	{
		if(msg.message == WM_LBUTTONDOWN) // ��갴��
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
					gettextstyle(&f);						// ��ȡ��ǰ��������
					f.lfHeight = 60;						// ��������߶�Ϊ 48
					_tcscpy(f.lfFaceName, _T("����"));		// ��������Ϊ�����塱(�߰汾 VC �Ƽ�ʹ�� _tcscpy_s ����)
					f.lfQuality = ANTIALIASED_QUALITY;		// �������Ч��Ϊ�����  
					f.lfWeight = FW_NORMAL;
					settextstyle(&f);
					settextcolor(RED);
					outtextxy(LONG/2 - 350,WIDE/2,"45�����ѵ������ڽ�������...");
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
		else if(msg.message == WM_MOUSEMOVE) // ����ƶ�
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
		else if(msg.message == WM_LBUTTONUP) // ����ɿ�
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

// ������Ϸ�ĺ���
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

// ������
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