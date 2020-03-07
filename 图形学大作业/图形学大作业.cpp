#include "stdafx.h"
#include <math.h>
#include <gl/glut.h>
#include <iostream>
#include "Texture.h"

using namespace std;

#define PI 3.14159
int inner = 10, outer = 80;   //torus's inner & outer radius
float s = outer + 4 * inner + 50;
float eyex = 0, eyey = 20, eyez = s;  //eye point initial position
float atx = 0, aty = 0, atz = 0;  //at point initial position
int ww, hh;  //variable for windows's width 
float angle = 0; //for angle between view-line and z axis
float r = s; //for distance between eye and at
float step = 3;  //for forward and backword step;
bool start = false;

void Display(void);
void Reshape(GLsizei w, GLsizei h);
void mytime(int value);
void init();
void mykeyboard(unsigned char key, int x, int y);
void mymenu(int value);

void selectFont(int size, int charset, const char* face); //选择字体
void drawCNString(const char* str); //生成中文字体函数

static GLuint MyPicture;


//三维向量
class Vector3
{
public:
	Vector3(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	static double dot(Vector3 v1, Vector3 v2)//点乘
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

private:
	double x;
	double y;
	double z;
};

//二维向量
class Vector2
{
public:
	Vector2(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2 operator -(const Vector2& v1)//向量相减
	{
		return Vector2(this->x - v1.x, this->y - v1.y);
	}

	Vector2 operator /(double num)//向量除以数
	{
		return Vector2(this->x / num, this->y / num);
	}

	double length()//向量模
	{
		return sqrt(pow(x, 2) + pow(y, 2));
	}

	static double dot(const Vector2& v1, const Vector2& v2)//点乘
	{
		return v1.x*v2.x + v1.y*v2.y;
	}

	double x;
	double y;
};

class WaterRendering
{
public:
	WaterRendering(int width, int height, double space)
	{
		this->width = width;
		this->height = height;
		this->space = space;

		points = new Point*[height];
		for (size_t i = 0; i < height; i++)
		{
			points[i] = new Point[width];
		}
		initPointsLocate();
	}

	void initPointsLocate()//初始化所有点的位置
	{
		double x = 0;
		double y = 0;
		double z = 0;

		for (size_t i = 0; i < height; i++, z += space)
		{
			for (size_t j = 0; j < width; j++, x += space)
			{
				points[i][j].x = x;
				points[i][j].y = y;
				points[i][j].z = z;
			}
			x = 0;
		}
	}

	void showPoints()//显示点的位置
	{
		glPushMatrix();
		glTranslated(-(width*space) / 2, 0, s - (height*space) - 17);
		glBegin(GL_POINTS);
		//透明效果
		glColor4d(0, 0.7, 1, 0.3);
		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				glVertex3d(points[i][j].x, points[i][j].y, points[i][j].z);
			}
		}
		glEnd();

		/*for (size_t i = 0; i < height-1; i++)
		{
			for (size_t j = 0; j < width-1; j++)
			{
				glBegin(GL_LINES);
				glVertex3d(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3d(points[i][j].x, points[i][j].y+1, points[i][j].z);
				glVertex3d(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3d(points[i][j].x+1, points[i][j].y, points[i][j].z);
				glVertex3d(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3d(points[i][j].x+1, points[i][j].y+1, points[i][j].z);
				glEnd();
			}
		}*/
		glPopMatrix();
	}

	void SinesoidWave1(double t)//运用正弦波的叠加模拟水面
	{
		Vector2 w1(1, 0);
		Vector2 w2(0, 1);
		Vector2 w3(0, -1);
		Vector2 w4(-1, -1);
		Vector2 w5(1, 1);

		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				Vector2 xz(points[i][j].x, points[i][j].z);
				double H = 0;
				//五个波形叠加
				double W1 = 1 * sin(Vector2::dot(xz, w1) * 0.4 + t);
				double W2 = 0.2*sin(Vector2::dot(xz, w2) * 0.8 + t);
				double W3 = 0.1*sin(Vector2::dot(xz, w3) * 1.2 + t);
				double W4 = 0.2*sin(Vector2::dot(xz, w4) * 1 + t);
				double W5 = 0.2*sin(Vector2::dot(xz, w5) * 0.7 + t);
				H = W1 + W2 + W3 + W4 + W5;

				points[i][j].y = H;
			}
		}
	}

	void SinesoidWave2(double t)//运用正弦波的叠加模拟水面
	{
		Vector2 w1(1, 0);
		Vector2 w2(0, 1);
		Vector2 w3(0, -1);
		Vector2 w4(-1, -1);
		Vector2 w5(1, 1);

		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				Vector2 xz(points[i][j].x, points[i][j].z);
				double H = 0;
				//五个波形叠加
				double W1 = 2 * pow(sin(Vector2::dot(xz, w1) * 0.3 + t), 4);
				double W2 = 0.5*sin(Vector2::dot(xz, w2) * 0.9 + t);
				double W3 = 0.2*pow(sin(Vector2::dot(xz, w3) * 1.5 + t), 2);
				double W4 = 0.4*sin(Vector2::dot(xz, w4) * 1 + t);
				double W5 = 0.3*sin(Vector2::dot(xz, w5) * 0.7 + t);
				H = W1 + W2 + W3 + W4 + W5;

				points[i][j].y = H;
			}
		}
	}

	void Ripples(double t)//涟漪效果
	{
		Vector2 circleCenter1(points[height / 2][width / 2].x + 50, points[height / 2][width / 2].z);
		//Vector2 circleCenter2(points[height / 2][width / 2].x + 100, points[height / 2][width / 2].z+20);

		for (size_t i = 0; i < height; i++)
		{
			for (size_t j = 0; j < width; j++)
			{
				Vector2 xz(points[i][j].x, points[i][j].z);
				Vector2 temp1(circleCenter1 - xz);
				//Vector2 temp2(circleCenter2 - xz);

				points[i][j].y = 0.2*sin(Vector2::dot(xz, temp1) * 0.1 + t);
			}
		}
	}

	~WaterRendering()
	{
		for (size_t i = 0; i < height; i++)
		{
			delete points[i];
		}
		delete points;
	}

private:
	typedef struct
	{
		double x;
		double y;
		double z;
	}Point;

	Point** points;
	int height, width;
	double space;
};

WaterRendering* water_rendering;

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	char *argv[] = { (char*)"1701040081",  (char*)"盛逶" };
	int argc = 2; // must/should match the number of strings in argv

	glutInit(&argc, argv);    //初始化GLUT库；
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(1024 / 2 - 250, 768 / 2 - 250);
	glutCreateWindow("3D大作业");

	glutReshapeFunc(Reshape);
	init();
	glutDisplayFunc(Display);  //用于绘制当前窗口；
	glutKeyboardFunc(mykeyboard);
	glutTimerFunc(20, mytime, 1);

	glutCreateMenu(mymenu);
	glutAddMenuEntry("SinesoidWaveMixed1", 1);
	glutAddMenuEntry("SinesoidWaveMixed2", 2);
	glutAddMenuEntry("Ripples", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();   //表示开始运行程序，用于程序的结尾；
	return 0;
}


void init()
{
	//开启光照
	static const GLfloat light_position[] = { -5.0f, 5.0f, 20.0f, 1.0f };
	static const GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	//关闭深度检测，开启混合模式
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_COLOR_MATERIAL);

	glPointSize(1);
	water_rendering = new WaterRendering(500, 200, 0.2);

	BuildTexture((char*)"Cover.jpg", MyPicture);
}


int choice = 1;
void mymenu(int value)
{
	choice = value;
}


double t = 0;
void mytime(int value)
{
	switch (choice)
	{
	case 1:
		water_rendering->initPointsLocate();
		water_rendering->SinesoidWave1(t);
		break;
	case 2:
		water_rendering->initPointsLocate();
		water_rendering->SinesoidWave2(t);
		break;
	case 3:
		water_rendering->initPointsLocate();
		water_rendering->Ripples(t);
		break;
	default:
		break;
	}

	glutPostRedisplay();
	t += 0.1;
	glutTimerFunc(20, mytime, 1);
}

void DrawCover()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, MyPicture);
	glPushMatrix();
	glTranslated(0, 20, -30);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-230, -250);

	glTexCoord2f(1, 0);
	glVertex2f(230, -250);

	glTexCoord2f(1, 1);
	glVertex2f(180, 160);

	glTexCoord2f(0, 1);
	glVertex2f(-180, 160);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void Display(void)
{
	glClearColor(0.0, 0.0, 0.0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, atx, aty, atz, 0, 1, 0);

	selectFont(24, GB2312_CHARSET, "楷体_GB2312"); //设置字体楷体48号字
	glColor3f(1, 1, 1);
	glRasterPos3f(-30, 50, 100);  //在世界坐标250,250处定位首字位置

	if (start)
	{
		drawCNString("  Water Simulate");
		water_rendering->showPoints();
	}
	else
	{
		DrawCover();
	}
	glutSwapBuffers();
}


void Reshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 10, 2 * outer + 8 * inner + 250+10000);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	ww = w;
	hh = h;
}

/************************************************************************/
/* 选择字体函数*/
/************************************************************************/
void selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}


/************************************************************************/
/* 生成中文字体函数*/
/************************************************************************/
void drawCNString(const char* str)
{
	int len, i;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	// 计算字符的个数
	// 如果是双字节字符的（比如中文字符），两个字节才算一个字符
	// 否则一个字节算一个字符
	len = 0;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}

	// 将混合字符转化为宽字符
	wstring = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	// 逐个输出字符
	for (i = 0; i < len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}

	// 回收所有临时资源
	free(wstring);
	glDeleteLists(list, 1);
}

void mykeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'W':
	case 'w':// 向前直走
	   //your code Here
		eyex = eyex - step * sin(angle*PI / 180.0);
		eyez = eyez - step * cos(angle*PI / 180.0);
		atx = atx - step * sin(angle*PI / 180.0);
		atz = atz - step * cos(angle*PI / 180.0);
		break;
	case 'S':
	case 's'://向后退
	   //your code Here
		eyex = eyex + step * sin(angle*PI / 180.0);
		eyez = eyez + step * cos(angle*PI / 180.0);
		atx = atx + step * sin(angle*PI / 180.0);
		atz = atz + step * cos(angle*PI / 180.0);
		break;
	case 'A':
	case 'a'://左看
		//your code Here
		angle = angle + 5;
		atx = eyex - r * sin(angle*PI / 180.0);
		atz = eyez - r * cos(angle*PI / 180.0);
		break;
	case 'D':
	case 'd'://右看
		//your code Here
		angle = angle - 5;
		atx = eyex - r * sin(angle*PI / 180.0);
		atz = eyez - r * cos(angle*PI / 180.0);
		break;
	case 13:
		start = true;
		break;
	case 27:
		exit(0);
		break;
	}
	glutPostRedisplay();//参数修改后调用重画函数，屏幕图形将发生改变
}

