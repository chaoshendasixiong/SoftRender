#ifndef __PAINT_H__
#define __PAINT_H__
#include <Windows.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600

#define ALIGN_DEBUG 0
#define DEBUG

#define ID_TIMER 1

typedef struct{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
} PIX;

//全局画布数据
PIX surface[WIDTH*HEIGHT];
//缓冲区rgb数据
COLORREF G_buffer[WIDTH*HEIGHT];

//初始化画布
HDC hScrDC, hMemDC;
void pix(HDC &hDC);
void setbuffer();
UINT * ptPixels;
void init(HWND &hWnd)
{
	hScrDC = GetDC(hWnd);
	hMemDC = CreateCompatibleDC(hScrDC);
	HBITMAP hBitmap, hOldBitmap;
	BITMAPINFO m_bmi;
	m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biWidth = WIDTH;
	m_bmi.bmiHeader.biHeight = HEIGHT;
	m_bmi.bmiHeader.biPlanes = 1;
	m_bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	m_bmi.bmiHeader.biCompression = BI_RGB;
	m_bmi.bmiHeader.biSizeImage = WIDTH*HEIGHT * 4;
	
	hBitmap = CreateDIBSection(hMemDC, (BITMAPINFO *)&m_bmi,DIB_RGB_COLORS, (void **)&ptPixels, NULL, 0);
	//hBitmap = CreateCompatibleBitmap(hScrDC, WIDTH, HEIGHT);//创建一个与屏幕设备描述表兼容的位图
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	SetTimer(hWnd, 1, 10, NULL);
}

void mypaint(HWND &hWnd)
{
	hScrDC = GetDC(hWnd);
	//PatBlt(hMemDC, 0, 0, WIDTH, HEIGHT, BLACKNESS); //将内存设备映像刷成黑色
	setbuffer();
	BitBlt(hScrDC, 0, 0, WIDTH, HEIGHT, hMemDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hScrDC);
}

//同步rgb数据
void setbuffer()
{
	int index = 0;
	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j <WIDTH; ++j) {
			int r = surface[index].R;
			int g = surface[index].G;
			int b = surface[index].B;
			//SetPixel(hDC, j, i, RGB(r, g, b));
			G_buffer[index] = RGB(r,g,b);
			++index;
		}
	}
	memcpy(ptPixels, G_buffer, WIDTH*HEIGHT*sizeof(COLORREF));
}
void random()
{
	static int idx = 0;
	if (idx >= 255)
		idx = 0;
	idx++;
	int index = WIDTH*HEIGHT-1;
	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j <WIDTH; ++j) {
			if (index >= WIDTH *HEIGHT/2) {
				int r = surface[index].R = idx;
				int g = surface[index].G = 0;// rand() % 255;
				int b = surface[index].B = 0;// rand() % 255;
			}
			else{
				int r = surface[index].R = rand() % 255;
				int g = surface[index].G = rand() % 255;
				int b = surface[index].B = rand() % 255;
			}
			--index;
		}
	}
}

void SetClientSize(HWND hWnd, int width, int height)
{
	if (!hWnd)
	{
		MessageBox(NULL, L"set window fail", NULL, 1);
		exit(1);
	}
	RECT  rectProgram, rectClient;
	GetWindowRect(hWnd, &rectProgram);   //获得程序窗口位于屏幕坐标
	GetClientRect(hWnd, &rectClient);      //获得客户区坐标
	//非客户区宽,高
	int nWidth = rectProgram.right - rectProgram.left - (rectClient.right - rectClient.left);
	int nHeiht = rectProgram.bottom - rectProgram.top - (rectClient.bottom - rectClient.top);
	nWidth += width;
	nHeiht += height;
	rectProgram.right = nWidth;
	rectProgram.bottom = nHeiht;
	int showToScreenx = GetSystemMetrics(SM_CXSCREEN) / 2 - nWidth / 2;    //居中处理
	int showToScreeny = GetSystemMetrics(SM_CYSCREEN) / 2 - nHeiht / 2;
	//MoveWindow(hWnd, showToScreenx, showToScreeny, rectProgram.right, rectProgram.bottom, false);
	SetWindowPos(hWnd, 0, showToScreenx, showToScreeny, rectProgram.right, rectProgram.bottom, SWP_NOMOVE);
	SetWindowPos(hWnd, HWND_TOPMOST, showToScreenx, showToScreeny, 0, 0, SWP_NOSIZE);
}

void destory(HWND &hWnd)
{
	DeleteDC(hMemDC);
	KillTimer(hWnd, ID_TIMER);
}

#endif