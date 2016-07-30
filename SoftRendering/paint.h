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
//surface 和 G_buffer 的像素顺序是相反的

PIX surface[WIDTH*HEIGHT];
//缓冲区rgb数据
COLORREF G_buffer[WIDTH*HEIGHT];

//初始化画布
HDC hScrDC, hMemDC;
void pix(HDC &hDC);
void setbuffer();
void setSurface();
void paintTrangle(int x1, int y1, int x2, int y2, int x3, int y3);
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
	m_bmi.bmiHeader.biSizeImage = WIDTH*HEIGHT * 32;
	
	hBitmap = CreateDIBSection(hMemDC, (BITMAPINFO *)&m_bmi,DIB_RGB_COLORS, (void **)&ptPixels, NULL, 0);
	//hBitmap = CreateCompatibleBitmap(hScrDC, WIDTH, HEIGHT);//创建一个与屏幕设备描述表兼容的位图
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	SetTimer(hWnd, 1, 10, NULL);
	setSurface();
}

void setpixel(int x, int y)
{
	int idx = y*WIDTH + x;
	surface[idx].R = 255;
	surface[idx].G = 0;
	surface[idx].B = 255;
}

void PutPixel(int x, int y, float color)
{
	int idx = y*WIDTH + x;
	surface[idx].R = 255;
	surface[idx].G = 0;
	surface[idx].B = 255;
}
void Bresenhamline(int x0, int y0, int x1, int y1, int color)
{
	int x, y, dx, dy;
	float k, e;
	dx = x1 - x0, dy = y1 - y0, k = dy / dx;
	e = -dx, x = x0, y = y0;
	int i;
	for (i = 0; i <= dx; i++)
	{
		//setpixel(x, y, color);
		
		x = x + 1; e = e + 2 * dy;
		if (e >= 0)
		{
			y++, e = e - 2*dx;
		}
		int idx = (y - 1)*WIDTH + x;
		surface[idx].R = 255;
		surface[idx].G = 0;
		surface[idx].B = 255;
	}
}

void InterBresenhamline(int x0, int y0, int x1, int y1, int color)
{
	int x, y, dx, dy, e;

	dx = x1 - x0; dy = y1 - y0; e = -dx;
	x = x0; y = y0;
	for (int i = 0; i<dx; i++){
		setpixel(x, y);
		x++; e = e + 2 * dy;
		if (e >= 0) { y++; e = e - 2 * dx; }
	}
}

void line_bresenham(int startX, int startY, int endX, int endY)
{
	int deltaX = endX - startX;
	int deltaY = endY - startY;
	int e = 0;
	int deltaX2 = deltaX << 1;
	int deltaY2 = deltaY << 1;

	setpixel(startX, startY);

	for (int x = startX + 1, y = startY; x <= endX; ++x)
	{
		// 关系式3) e2 =  e1 + 2 * deltaY – 2 * deltaX * (y2 – y1)
		// 关系式4) 当e2 + 2 * deltaY > deltaX时y2 = y1 + 1, 否则y2 = y1
		e += deltaY2;
		if (e > deltaX)
		{
			e -= deltaX2;
			++y;
		}
		setpixel(x, y);
	}
}
//兼顾所有斜率
void Glib_Line(int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, e;
	dx = x2 - x1;
	dy = y2 - y1;

	if (dx >= 0)
	{
		if (dy >= 0) // dy>=0
		{
			if (dx >= dy) // 1/8 octant
			{
				e = dy - dx / 2;
				while (x1 <= x2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ y1 += 1; e -= dx; }
					x1 += 1;
					e += dy;
				}
			}
			else  // 2/8 octant
			{
				e = dx - dy / 2;
				while (y1 <= y2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ x1 += 1; e -= dy; }
					y1 += 1;
					e += dx;
				}
			}
		}
		else     // dy<0
		{
			dy = -dy;   // dy=abs(dy)

			if (dx >= dy) // 8/8 octant
			{
				e = dy - dx / 2;
				while (x1 <= x2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ y1 -= 1; e -= dx; }
					x1 += 1;
					e += dy;
				}
			}
			else  // 7/8 octant
			{
				e = dx - dy / 2;
				while (y1 >= y2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ x1 += 1; e -= dy; }
					y1 -= 1;
					e += dx;
				}
			}
		}
	}
	else //dx<0
	{
		dx = -dx;  //dx=abs(dx)
		if (dy >= 0) // dy>=0
		{
			if (dx >= dy) // 4/8 octant
			{
				e = dy - dx / 2;
				while (x1 >= x2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ y1 += 1; e -= dx; }
					x1 -= 1;
					e += dy;
				}
			}
			else  // 3/8 octant
			{
				e = dx - dy / 2;
				while (y1 <= y2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ x1 -= 1; e -= dy; }
					y1 += 1;
					e += dx;
				}
			}
		}
		else     // dy<0
		{
			dy = -dy;   // dy=abs(dy)

			if (dx >= dy) // 5/8 octant
			{
				e = dy - dx / 2;
				while (x1 >= x2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ y1 -= 1; e -= dx; }
					x1 -= 1;
					e += dy;
				}
			}
			else  // 6/8 octant
			{
				e = dx - dy / 2;
				while (y1 >= y2)
				{
					PutPixel(x1, y1, color);
					if (e>0){ x1 -= 1; e -= dy; }
					y1 -= 1;
					e += dx;
				}
			}
		}
	}
}


void paintTrangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	Glib_Line(x1, y1, x2, y2, 0);
	////line_bresenham(x1, y1, x2, y2);
	Glib_Line(x2, y2, x3, y3, 0);
	Glib_Line(x3, y3, x1, y1, 0);
}

void setSurface()
{
	paintTrangle(0, 0, 400, 200, 100, 500);
	//Bresenhamline(0,0, 400,300, 0);
	
	/*int idx = 0;
	for (int i = 0; i < 100; i++)
	{
		surface[idx].R = 255;
		surface[idx].G = 0;
		surface[idx].B = 255;
		idx++;
	}*/
}

void mypaint(HWND &hWnd)
{
	
	hScrDC = GetDC(hWnd);
	
	//PatBlt(hMemDC, 0, 0, WIDTH, HEIGHT, BLACKNESS); //将内存设备映像刷成黑色
	setbuffer();
	BitBlt(hScrDC, 0, 0, WIDTH, HEIGHT, hMemDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hScrDC);
}

//同步rgb数据 将surface左上的数据调整到左下
void setbuffer()
{
	int index = 0;//surface的索引
	for (int j = HEIGHT-1; j >= 0; j--)
	{
		for (int i = 0; i < WIDTH; i++)
		{
			int idx = j*WIDTH + i;
			int r = surface[index].R;
			int g = surface[index].G;
			int b = surface[index].B;
			G_buffer[idx] = RGB(r, g, b);
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
	int index = 0;
	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j <WIDTH; ++j) {
			if (index <= WIDTH *HEIGHT/2) {
				int r = surface[index].R = idx;
				int g = surface[index].G = 0;// rand() % 255;
				int b = surface[index].B = 0;// rand() % 255;
			}
			else{
				int r = surface[index].R = rand() % 255;
				int g = surface[index].G = rand() % 255;
				int b = surface[index].B = rand() % 255;
			}
			++index;
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
	SetWindowPos(hWnd, 0, showToScreenx, showToScreeny, 0, 0, SWP_NOSIZE);
}

void destory(HWND &hWnd)
{
	DeleteDC(hMemDC);
	KillTimer(hWnd, ID_TIMER);
}

#endif