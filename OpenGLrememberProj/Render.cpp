#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

double n_otrisovki = 0.0;

bool FLAG_HELP = 0;

bool textureMode = true;
bool lightMode = true;
bool FLAG_VRACHENIE = 0;
float a = 0;				
float b = 0;	
bool flag_scheta = 0;
bool flag_scheta_1 = 0;
bool flag_scheta_2 = 0;
bool flag_scheta_3 = 0;

int dop_flag = 0;
int dop_flag_1 = 0;

//для перемещения
double X_D = 0,Y_D = 0,Z_D = 0;
bool FLAG_TOCHNOST_PEREMECH = 1;
double Z_begin = Z_D, Z_end = Z_D;

//Luch
bool FLAG_LUCH = 0;
double a_luch = 0;
double n_luch = 85;
bool flag_obratno_luch = 0;

double X_lucha = 0;
double Y_lucha = 0;
double Z_lucha = 0;


//вращения деталей
double a_detali_V = 0;
double a_detali_1 = 0;

//двигателей beta мое 
double a_dvigatel_1 = 0;
double a_dvigatel_2 = 0;

//===============================================

double X_KUB = 0;
double Y_KUB = 0;
double Z_KUB = 0;
double STORONA_KUB = 0;
double POLUSTORONA_KUB = 0;

double X_KUB_2 = 0;
double Y_KUB_2 = 0;
double Z_KUB_2 = 0;
double STORONA_KUB_2 = 0;
double POLUSTORONA_KUB_2 = 0;

double X_KUB_3 = 0;
double Y_KUB_3 = 0;
double Z_KUB_3 = 0;
double STORONA_KUB_3 = 0;
double POLUSTORONA_KUB_3 = 0;

double KOLVO_MATERIALOV = 0;
double $KOLVO_MATERIALOV = 0;

//===============================================


//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры
//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor4d(0.9, 0.8, 0,1);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor4d(0, 0, 0,1);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света
//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 300)
		return;

	camera.camDist += 0.06*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	//=======================================
	//Дополнительно MY на будущее // перемещение объекта
	if (key == 37)	//<--
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D+=0.5;
			 Y_D-=0.5;}
			else 
			{X_D+=0.05;
			 Y_D-=0.05;}
		}
	if (key == 38)	//^
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D-=0.5;
			 Y_D-=0.5;}
			else 
			{X_D-=0.05;
			 Y_D-=0.05;}
		}
	if (key == 39)	//-->
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D-=0.5;
			 Y_D+=0.5;}
			else 
			{X_D-=0.05;
			 Y_D+=0.05;}
		}
	if (key == 40)	//v
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D+=0.5;
			 Y_D+=0.5;}
			else 
			{X_D+=0.05;
			 Y_D+=0.05;}
		}
	if (key == 33)	//PgUp
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{Z_D+=0.5;}
			else 
			{Z_D+=0.05;}
			Z_begin = Z_D;
		}
	if (key == 34)	//PgDn
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{Z_D-=0.5;}
			else 
			{Z_D-=0.05;}
			Z_begin = Z_D;
		}
	//-------------------------
	if (key == 'A')	//<--
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{ Y_D-=0.5;}
			else 
			{Y_D-=0.05;}
		}
	if (key == 'W')	//^
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D-=0.5;}
			else 
			{X_D-=0.05;}
		}
	if (key == 'D')	//-->
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{Y_D+=0.5;}
			else 
			{Y_D+=0.05;}
		}
	if (key == 'S')	//v
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D+=0.5;}
			else 
			{X_D+=0.05;}
		}
	
	//-------------------------

	if (key == 16) // shift
		{
			FLAG_TOCHNOST_PEREMECH = !FLAG_TOCHNOST_PEREMECH;
		}

	if (key == 'H')
	{
		FLAG_HELP = !FLAG_HELP;
	}

	if (key == 'V')	//
		{
			FLAG_VRACHENIE = !FLAG_VRACHENIE;
		}
	if (key == 'B')	//
		{
		light.pos.setCoords(154.80773233123966, 156.23912926825460, 204.90061126301418);
		}
	if (key == 'Q')	//
	{
		FLAG_LUCH = !FLAG_LUCH;
	}
	//=========================
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	//OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::LoadBMP("texture_1v.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	GLuint texId;
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR более крутотенечка
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

//---------------------------------------------------------------------------------------------------------
double PI1 = 3.1415926535897932384626433832795;

double Mas_NORMAL[]={0,0,0};
double Vektor[] = {0,0,0};
double vektor_a[] = {0,0,0};
double vektor_b[] = {0,0,0};

void Krug(double z, double Radius , double color1,double color2,double color3,double alpha1)
{ 
glNormal3d(0,0,1);
int Count = 100 ;
 glBegin( GL_TRIANGLE_FAN );
   glColor4d(color1,color2,color3,alpha1);
         for( int i = 0; i <= Count; i++ ) 
		 {
			 double alpha = (double)i / (double)Count * PI1 * Radius;
           glVertex3d( cos( alpha ) * Radius, sin( alpha ) * Radius, z );
         }
    glEnd();
}
double Dlina_line(double A[], double B[])
{
	double peremennaya = sqrt(pow(A[0] - B[0], 2) + pow(A[1] - B[1], 2) + pow(A[2] - B[2], 2));
	return peremennaya;
}
void NORMAL(double a[], double b[])
{
	double ax = a[0];
	double ay = a[1];
	double az = a[2];

	double bx = b[0];
	double by = b[1];
	double bz = b[2];

	// i 
	double chast1 = ay*bz;
	double chast5 = az*by;
	double nx = chast1 - chast5;

	// j
	double chast2 = az*bx;
	double chast6 = ax*bz;
	double ny = (chast2 - chast6);

	//  z
	double chast3 = ax*by;
	double chast4 = ay*bx;
	double nz = (chast3 - chast4);
	
	double Dlina = sqrt(pow(nx,2) + pow(ny,2) + pow(nz,2));

	Mas_NORMAL[0] = -nx/Dlina;
	Mas_NORMAL[1] = -(ny)/Dlina;
	Mas_NORMAL[2] = nz/Dlina;
}
void COORD_VEKTOR(double A[], double B[])
{
	double x = A[0] - B[0];
	double y = A[1] - B[1];
	double z = A[2] - B[2];
	Vektor[0] = x;
	Vektor[1] = y;
	Vektor[2] = z;
}
//для линий
void LINE(double ONE[], double TWO[])
{
	glBegin(GL_LINES);

	glVertex3dv(ONE);
	glVertex3dv(TWO);

	glEnd();
}
//для треугольников ПОСЛЕ ТЕКСТУР ЛЕЖИТ ЗДЕСЬ ПРОСТО ТАК
void TRIANGLE(double One[], double Two[], double Three[])
{
	glBegin(GL_TRIANGLES);
	glVertex3dv(One);
	glVertex3dv(Two);
	glVertex3dv(Three);
	glEnd();
}

void SPHERA(double r, int nx, int ny)
{
	GLfloat amb_1[] = { 0.541, 0.459, 0.282, 1. }; //материал 1
	GLfloat dif_1[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_1[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_1 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

		int ix, iy;
		double x, y, z, sy, cy, sy1, cy1, sx, cx, piy, pix, ay, ay1, ax, tx, ty, ty1, dnx, dny, diy;
		dnx = 1.0 / (double)nx;
		dny = 1.0 / (double)ny;
		glBegin(GL_QUAD_STRIP);
		piy = PI1 * dny;
		pix = PI1 * dnx;
		for (iy = 0; iy < ny; iy++)
		{
			diy = (double)iy;
			ay = diy * piy;
			sy = sin(ay);
			cy = cos(ay);
			ty = diy * dny;
			ay1 = ay + piy;
			sy1 = sin(ay1);
			cy1 = cos(ay1);
			ty1 = ty + dny;
			for (ix = 0; ix <= nx; ix++)
			{
				ax = 2.0 * ix * pix;
				sx = sin(ax);
				cx = cos(ax);
				x = r * sy * cx;
				y = r * sy * sx;
				z = -r * cy;
				tx = (double)ix * dnx;
				glNormal3d(x, y, z);
				glVertex3d(x, y, z);
				x = r * sy1 * cx;
				y = r * sy1 * sx;
				z = -r * cy1;
				glNormal3d(x, y, z);
				glVertex3d(x, y, z);
			}
		}
		glEnd();
}
//================================================================================================================


void DOP_PLOSKOST(double a, double z_dop_ploskosti, double color1, double color2, double color3, double alpha1)
{	
	glBegin(GL_QUADS);
	
	glNormal3d(0,0,1);
	glColor4d(color1, color2, color3, alpha1);

	glVertex3d(-a,-a,z_dop_ploskosti);

	glVertex3d(-a,a,z_dop_ploskosti);

	glVertex3d(a,a,z_dop_ploskosti);

	glVertex3d(a,-a,z_dop_ploskosti);

	glEnd();
}
void DOP_PLOSKOST_V(double a, double z_dop_ploskosti, double color1, double color2, double color3, double alpha1)
{
	glBegin(GL_QUADS);

	glNormal3d(0, 0, -1);
	glColor4d(color1, color2, color3, alpha1);

	glVertex3d(-a, -a, z_dop_ploskosti);

	glVertex3d(-a, a, z_dop_ploskosti);

	glVertex3d(a, a, z_dop_ploskosti);

	glVertex3d(a, -a, z_dop_ploskosti);

	glEnd();
}
void DOP_PLOSKOST_S(double a, double z_dop_ploskosti, double visota, double color1, double color2, double color3, double alpha1)
{
	glBegin(GL_QUADS);

	glColor4d(color1, color2, color3, alpha1);

	glNormal3d(1, 0, 0);
	glVertex3d(-a, -a, z_dop_ploskosti);
	glVertex3d(-a, a, z_dop_ploskosti);
	glVertex3d(-a, a, z_dop_ploskosti+visota);
	glVertex3d(-a, -a, z_dop_ploskosti+visota);

	glNormal3d(0, -1, 0);
	glVertex3d(-a, a, z_dop_ploskosti);
	glVertex3d(a, a, z_dop_ploskosti);
	glVertex3d(a, a, z_dop_ploskosti + visota);
	glVertex3d(-a, a, z_dop_ploskosti + visota);
	
	glNormal3d(-1, 0, 0);
	glVertex3d(a, a, z_dop_ploskosti);
	glVertex3d(a, -a, z_dop_ploskosti);
	glVertex3d(a, -a, z_dop_ploskosti + visota);
	glVertex3d(a, a, z_dop_ploskosti + visota);

	glNormal3d(0, 1, 0);
	glVertex3d(-a, -a, z_dop_ploskosti);
	glVertex3d(a, -a, z_dop_ploskosti);
	glVertex3d(a, -a, z_dop_ploskosti + visota);
	glVertex3d(-a, -a, z_dop_ploskosti + visota);

	glEnd();
}
void DOP_PLOSKOST_KRUG(double x, double y, double z, double Radius, double color1, double color2, double color3, double alpha1)
{
	glNormal3d(0, 0, 1);
	int Count = 1000;
	glBegin(GL_TRIANGLE_FAN);
	glColor4d(color1, color2, color3, alpha1);
	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;
		glVertex3d(cos(alpha) * Radius, sin(alpha) * Radius, z);
	}
	glEnd();

}

void KUBIK_1(double Coord_X, double Coord_Y, double Coord_Z, double storona) //координаты центра!!!
{
	double polustorona = storona / 2.0;

	double A_V[] = { Coord_X + polustorona, Coord_Y - polustorona, Coord_Z + polustorona }, B_V[] = { Coord_X - polustorona, Coord_Y - polustorona, Coord_Z + polustorona }, C_V[] = { Coord_X - polustorona, Coord_Y + polustorona, Coord_Z + polustorona }, D_V[] = { Coord_X + polustorona, Coord_Y + polustorona, Coord_Z + polustorona };
	double A_N[] = { Coord_X + polustorona, Coord_Y - polustorona, Coord_Z - polustorona }, B_N[] = { Coord_X - polustorona, Coord_Y - polustorona, Coord_Z - polustorona }, C_N[] = { Coord_X - polustorona, Coord_Y + polustorona, Coord_Z - polustorona }, D_N[] = { Coord_X + polustorona, Coord_Y + polustorona, Coord_Z - polustorona };

	glColor4d(0.761, 0.188, 0.173, 1.0);

	glBegin(GL_QUADS);
	//верх
	glNormal3d(0, 0, 1);
	glVertex3dv(A_V);
	glVertex3dv(B_V);
	glVertex3dv(C_V);
	glVertex3dv(D_V);
	//низ
	glNormal3d(0, 0, -1);
	glVertex3dv(A_N);
	glVertex3dv(B_N);
	glVertex3dv(C_N);
	glVertex3dv(D_N);
	//бок1
	glNormal3d(0, -1, 0);
	glVertex3dv(A_V);
	glVertex3dv(B_V);
	glVertex3dv(B_N);
	glVertex3dv(A_N);
	//бок2
	glNormal3d(-1, 0, 0);
	glVertex3dv(B_V);
	glVertex3dv(C_V);
	glVertex3dv(C_N);
	glVertex3dv(B_N);
	//бок3
	glNormal3d(0, 1, 0);
	glVertex3dv(C_V);
	glVertex3dv(D_V);
	glVertex3dv(D_N);
	glVertex3dv(C_N);
	//бок4
	glNormal3d(1, 0, 0);
	glVertex3dv(D_V);
	glVertex3dv(A_V);
	glVertex3dv(A_N);
	glVertex3dv(D_N);


	glEnd();

}


void STORONA_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, int Count)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };

	for (int i = 0; i <= Count ; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], Mas_NORMAL[2]);

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_begin_down);
		glVertex3dv(Tocka_real_down);
		glVertex3dv(Tocka_real);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[1] = Tocka_real_down[1];

	}
	glEnd();
}
void STORONA_VNUTR_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, int Count)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(-Mas_NORMAL[0], -Mas_NORMAL[1], Mas_NORMAL[2]);

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_begin_down);
		glVertex3dv(Tocka_real_down);
		glVertex3dv(Tocka_real);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[1] = Tocka_real_down[1];

	}
	glEnd();
}
void DOWN_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count)
{
	glNormal3d(0, 0, -1);
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_in[] = { cos(alpha) * (Radius- Tolchina), sin(alpha) * (Radius - Tolchina), z_nachalo };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_in[] = { cos(alpha) * (Radius - Tolchina), sin(alpha) * (Radius - Tolchina), z_nachalo };

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_real);
		glVertex3dv(Tocka_real_in);
		glVertex3dv(Tocka_begin_in);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_in[0] = Tocka_real_in[0];
		Tocka_begin_in[1] = Tocka_real_in[1];

	}
	glEnd();
}
void UP_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count)
{

	glNormal3d(0, 0, 1);
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius_verch;

	double Tocka_begin[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
	double Tocka_begin_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo+visota };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
		double Tocka_real_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo + visota };

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_real);
		glVertex3dv(Tocka_real_in);
		glVertex3dv(Tocka_begin_in);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_in[0] = Tocka_real_in[0];
		Tocka_begin_in[1] = Tocka_real_in[1];

	}
	glEnd();
}
void KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, double color1, double color2, double color3, double alpha1)
{
	int Count = 500;
	glColor4d(color1, color2, color3, alpha1);
	
	UP_KONUS(z_nachalo, visota, Radius, Radius_verch,Tolchina, Count);
	DOWN_KONUS(z_nachalo, visota, Radius, Radius_verch, Tolchina, Count);
	STORONA_KONUS(z_nachalo, visota, Radius, Radius_verch, Count);

	STORONA_VNUTR_KONUS(z_nachalo, visota, Radius-Tolchina, Radius_verch - Tolchina, Count);
}

void KONUS_LINE(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, double color1, double color2, double color3, double alpha1)
{
	int Count = 500;
	glColor4d(color1, color2, color3, alpha1);
	
	glPushMatrix();

	glScaled(0.05, 0.05, 1);

	UP_KONUS(z_nachalo, visota, Radius, Radius_verch, Tolchina, Count);
	DOWN_KONUS(z_nachalo, visota, Radius, Radius_verch, Tolchina, Count);
	STORONA_KONUS(z_nachalo, visota, Radius, Radius_verch, Count);

	STORONA_VNUTR_KONUS(z_nachalo, visota, Radius - Tolchina, Radius_verch - Tolchina, Count);

	glPopMatrix();

}

double tocka_postroenia_konus_polosa_1[] = {0,0,0};
double tocka_postroenia_konus_polosa_2[] = {0,0,0};
double tocka_postroenia_konus_polosa_3[] = {0,0,0};
double tocka_postroenia_konus_polosa_4[] = {0,0,0};

double tocka_postroenia_konus_polosa_2_1[] = {0,0,0};
double tocka_postroenia_konus_polosa_2_2[] = {0,0,0};
double tocka_postroenia_konus_polosa_2_3[] = {0,0,0};
double tocka_postroenia_konus_polosa_2_4[] = {0,0,0};

void STORONA_KONUS_POLOSA(double z_nachalo, double visota, double Radius, double Radius_verch, int Count,double KOLVO, double SHIRINA_POLOSI)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };

	for (int i = 0; i <= Count ; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], Mas_NORMAL[2]);

			if ((alpha > (6.28/KOLVO)) && (alpha < ((6.28 / KOLVO) + (SHIRINA_POLOSI))))
		{
			if(dop_flag == 0)
			{
				tocka_postroenia_konus_polosa_1[0] = Tocka_begin[0];
				tocka_postroenia_konus_polosa_1[1] = Tocka_begin[1];
				tocka_postroenia_konus_polosa_1[2] = Tocka_begin[2];
				tocka_postroenia_konus_polosa_4[0] = Tocka_begin_down[0];
				tocka_postroenia_konus_polosa_4[1] = Tocka_begin_down[1];
				tocka_postroenia_konus_polosa_4[2] = Tocka_begin_down[2];
				dop_flag = 1;
			}

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_begin_down);
		glVertex3dv(Tocka_real_down);
		glVertex3dv(Tocka_real);
		}

			if (alpha < ((6.28 / KOLVO) + (SHIRINA_POLOSI)))
			{
				dop_flag_1 = 1 ;
			}

		if(dop_flag_1 == 1)
			{
				tocka_postroenia_konus_polosa_2_1[0] = Tocka_real[0];
				tocka_postroenia_konus_polosa_2_1[1] = Tocka_real[1];
				tocka_postroenia_konus_polosa_2_1[2] = Tocka_real[2];
				tocka_postroenia_konus_polosa_2_4[0] = Tocka_real_down[0];
				tocka_postroenia_konus_polosa_2_4[1] = Tocka_real_down[1];
				tocka_postroenia_konus_polosa_2_4[2] = Tocka_real_down[2];
				dop_flag_1 = 0;
			}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[1] = Tocka_real_down[1];

	}
	dop_flag = 0;
	dop_flag_1 = 0;
	glEnd();
}
void STORONA_VNUTR_KONUS_POLOSA(double z_nachalo, double visota, double Radius, double Radius_verch, int Count,double KOLVO, double SHIRINA_POLOSI)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(-Mas_NORMAL[0], -Mas_NORMAL[1], Mas_NORMAL[2]);

			if ((alpha > (6.28/KOLVO)) && (alpha < ((6.28 / KOLVO) + (SHIRINA_POLOSI))))
		{
			if(dop_flag == 0)
			{
				tocka_postroenia_konus_polosa_2[0] = Tocka_begin[0];
				tocka_postroenia_konus_polosa_2[1] = Tocka_begin[1];
				tocka_postroenia_konus_polosa_2[2] = Tocka_begin[2];
				tocka_postroenia_konus_polosa_3[0] = Tocka_begin_down[0];
				tocka_postroenia_konus_polosa_3[1] = Tocka_begin_down[1];
				tocka_postroenia_konus_polosa_3[2] = Tocka_begin_down[2];
				dop_flag = 1;
			}

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_begin_down);
		glVertex3dv(Tocka_real_down);
		glVertex3dv(Tocka_real);
		}

			if (alpha < ((6.28 / KOLVO) + (SHIRINA_POLOSI)))
			{
				dop_flag_1= 1 ;
			}

		if(dop_flag_1 == 1)
			{
				tocka_postroenia_konus_polosa_2_2[0] = Tocka_real[0];
				tocka_postroenia_konus_polosa_2_2[1] = Tocka_real[1];
				tocka_postroenia_konus_polosa_2_2[2] = Tocka_real[2];
				tocka_postroenia_konus_polosa_2_3[0] = Tocka_real_down[0];
				tocka_postroenia_konus_polosa_2_3[1] = Tocka_real_down[1];
				tocka_postroenia_konus_polosa_2_3[2] = Tocka_real_down[2];
				dop_flag_1 = 0;
			}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[1] = Tocka_real_down[1];

	}
	dop_flag_1 = 0;
	dop_flag = 0;
	glEnd();
}
void DOWN_KONUS_POLOSA(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count,double KOLVO, double SHIRINA_POLOSI)
{
	glNormal3d(0, 0, -1);
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_in[] = { cos(alpha) * (Radius- Tolchina), sin(alpha) * (Radius - Tolchina), z_nachalo };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_in[] = { cos(alpha) * (Radius - Tolchina), sin(alpha) * (Radius - Tolchina), z_nachalo };

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_real);
		glVertex3dv(Tocka_real_in);
		glVertex3dv(Tocka_begin_in);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_in[0] = Tocka_real_in[0];
		Tocka_begin_in[1] = Tocka_real_in[1];

	}
	glEnd();
}
void UP_KONUS_POLOSA(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count,double KOLVO, double SHIRINA_POLOSI)
{

	glNormal3d(0, 0, 1);
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius_verch;

	double Tocka_begin[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
	double Tocka_begin_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo+visota };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
		double Tocka_real_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo + visota };

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_real);
		glVertex3dv(Tocka_real_in);
		glVertex3dv(Tocka_begin_in);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_in[0] = Tocka_real_in[0];
		Tocka_begin_in[1] = Tocka_real_in[1];

	}
	glEnd();
}
void dop_polosi()
{
	glBegin(GL_QUADS);

	COORD_VEKTOR(tocka_postroenia_konus_polosa_2, tocka_postroenia_konus_polosa_1);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(tocka_postroenia_konus_polosa_2, tocka_postroenia_konus_polosa_3);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], Mas_NORMAL[2]);


	glVertex3dv(tocka_postroenia_konus_polosa_1);
	glVertex3dv(tocka_postroenia_konus_polosa_2);
	glVertex3dv(tocka_postroenia_konus_polosa_3);
	glVertex3dv(tocka_postroenia_konus_polosa_4);
	glEnd();
}
void dop_polosi_K()
{
	glBegin(GL_QUADS);

	COORD_VEKTOR(tocka_postroenia_konus_polosa_2_2, tocka_postroenia_konus_polosa_2_1);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(tocka_postroenia_konus_polosa_2_2, tocka_postroenia_konus_polosa_2_3);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(-Mas_NORMAL[0], -Mas_NORMAL[1], Mas_NORMAL[2]);


	glVertex3dv(tocka_postroenia_konus_polosa_2_1);
	glVertex3dv(tocka_postroenia_konus_polosa_2_2);
	glVertex3dv(tocka_postroenia_konus_polosa_2_3);
	glVertex3dv(tocka_postroenia_konus_polosa_2_4);
	glEnd();
}
void KONUS_POLOSA(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina,double KOLVO, double SHIRINA_POLOSI, double color1, double color2, double color3, double alpha1)
{
	int Count = 500;
	glColor4d(color1, color2, color3, alpha1);
	
	//UP_KONUS_POLOSA(z_nachalo, visota, Radius, Radius_verch,Tolchina, Count,KOLVO);
	//DOWN_KONUS_POLOSA(z_nachalo, visota, Radius, Radius_verch, Tolchina, Count,KOLVO);
	STORONA_KONUS_POLOSA(z_nachalo, visota, Radius, Radius_verch, Count,KOLVO, SHIRINA_POLOSI);
	STORONA_VNUTR_KONUS_POLOSA(z_nachalo, visota, Radius-Tolchina, Radius_verch - Tolchina, Count,KOLVO, SHIRINA_POLOSI);
	dop_polosi();	
	dop_polosi_K();
}



//для низа
void STORONA_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, int Count, double UGOL)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], Mas_NORMAL[2]);

		if ((alpha > (UGOL/2*PI1 / 180))&&(alpha < 6.28-(UGOL/2*PI1 / 180)))
		{
			if (dop_flag == 0)
			{
				tocka_postroenia_konus_polosa_1[0] = Tocka_begin[0];
				tocka_postroenia_konus_polosa_1[1] = Tocka_begin[1];
				tocka_postroenia_konus_polosa_1[2] = Tocka_begin[2];
				tocka_postroenia_konus_polosa_4[0] = Tocka_begin_down[0];
				tocka_postroenia_konus_polosa_4[1] = Tocka_begin_down[1];
				tocka_postroenia_konus_polosa_4[2] = Tocka_begin_down[2];
				dop_flag = 1;
			}

			glVertex3dv(Tocka_begin);
			glVertex3dv(Tocka_begin_down);
			glVertex3dv(Tocka_real_down);
			glVertex3dv(Tocka_real);
		}

		if (alpha < 6.28 - (UGOL / 2 * PI1 / 180))
		{
			dop_flag_1 = 1;
		}

		if (dop_flag_1 == 1)
		{
			tocka_postroenia_konus_polosa_2_1[0] = Tocka_real[0];
			tocka_postroenia_konus_polosa_2_1[1] = Tocka_real[1];
			tocka_postroenia_konus_polosa_2_1[2] = Tocka_real[2];
			tocka_postroenia_konus_polosa_2_4[0] = Tocka_real_down[0];
			tocka_postroenia_konus_polosa_2_4[1] = Tocka_real_down[1];
			tocka_postroenia_konus_polosa_2_4[2] = Tocka_real_down[2];
			dop_flag_1 = 0;
		}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[1] = Tocka_real_down[1];

	}
	dop_flag = 0;
	dop_flag_1 = 0;
	glEnd();
}
void STORONA_VNUTR_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, int Count, double UGOL)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * (Radius+2.0);

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(-Mas_NORMAL[0], -Mas_NORMAL[1], Mas_NORMAL[2]);

		if ((alpha > (UGOL / 2 * PI1 / 180)) && (alpha < 6.28 - (UGOL / 2 * PI1 / 180)))
		{
			if (dop_flag == 0)
			{
				tocka_postroenia_konus_polosa_2[0] = Tocka_begin[0];
				tocka_postroenia_konus_polosa_2[1] = Tocka_begin[1];
				tocka_postroenia_konus_polosa_2[2] = Tocka_begin[2];
				tocka_postroenia_konus_polosa_3[0] = Tocka_begin_down[0];
				tocka_postroenia_konus_polosa_3[1] = Tocka_begin_down[1];
				tocka_postroenia_konus_polosa_3[2] = Tocka_begin_down[2];
				dop_flag = 1;
			}

			glVertex3dv(Tocka_begin);
			glVertex3dv(Tocka_begin_down);
			glVertex3dv(Tocka_real_down);
			glVertex3dv(Tocka_real);
		}

		if (alpha < (6.28 - (UGOL / 2.0 * PI1 / 180.0)))
		{
			dop_flag_1 = 1;
		}

		if (dop_flag_1 == 1)
		{
			tocka_postroenia_konus_polosa_2_2[0] = Tocka_real[0];
			tocka_postroenia_konus_polosa_2_2[1] = Tocka_real[1];
			tocka_postroenia_konus_polosa_2_2[2] = Tocka_real[2];
			tocka_postroenia_konus_polosa_2_3[0] = Tocka_real_down[0];
			tocka_postroenia_konus_polosa_2_3[1] = Tocka_real_down[1];
			tocka_postroenia_konus_polosa_2_3[2] = Tocka_real_down[2];
			dop_flag_1 = 0;
		}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[1] = Tocka_real_down[1];

	}
	dop_flag_1 = 0;
	dop_flag = 0;
	glEnd();
}
void DOWN_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count, double UGOL)
{
	glNormal3d(0, 0, -1);
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
	double Tocka_begin_in[] = { cos(alpha) * (Radius - Tolchina), sin(alpha) * (Radius - Tolchina), z_nachalo };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius, sin(alpha) * Radius, z_nachalo };
		double Tocka_real_in[] = { cos(alpha) * (Radius - Tolchina), sin(alpha) * (Radius - Tolchina), z_nachalo };
		
		if ((alpha > (UGOL / 2 * PI1 / 180)) && (alpha < 6.28 - (UGOL / 2 * PI1 / 180)))
		{
			glVertex3dv(Tocka_begin);
			glVertex3dv(Tocka_real);
			glVertex3dv(Tocka_real_in);
			glVertex3dv(Tocka_begin_in);
		}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_in[0] = Tocka_real_in[0];
		Tocka_begin_in[1] = Tocka_real_in[1];

	}
	glEnd();
}
void UP_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count, double UGOL)
{

	glNormal3d(0, 0, 1);
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius_verch;

	double Tocka_begin[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
	double Tocka_begin_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo + visota };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
		double Tocka_real_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo + visota };

		if ((alpha > (UGOL / 2 * PI1 / 180)) && (alpha < 6.28 - (UGOL / 2 * PI1 / 180)))
		{
			glVertex3dv(Tocka_begin);
			glVertex3dv(Tocka_real);
			glVertex3dv(Tocka_real_in);
			glVertex3dv(Tocka_begin_in);
		}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_in[0] = Tocka_real_in[0];
		Tocka_begin_in[1] = Tocka_real_in[1];

	}
	glEnd();
}
void DOP_STORONI_KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count, double UGOL)
{
	double one_1[] = { 0,0,0 };
	double two_1[] = { 0,0,0 };
	double three_1[] = { 0,0,0 };
	double four_1[] = { 0,0,0 };

	double one_2[] = { 0,0,0 };
	double two_2[] = { 0,0,0 };
	double three_2[] = { 0,0,0 };
	double four_2[] = { 0,0,0 };

	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius_verch;
	double Tocka_begin[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
	double Tocka_begin_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo + visota };
	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;
		
		if ((alpha >= (UGOL / 2 * PI1 / 180)))
			flag_scheta = 1;

		if (flag_scheta == 1)
		{
			one_1[0] = Tocka_begin[0];
			one_1[1] = Tocka_begin[1];
			one_1[2] = Tocka_begin[2];

			two_1[0] = Tocka_begin_in[0];
			two_1[1] = Tocka_begin_in[1];
			two_1[2] = Tocka_begin_in[2];

			flag_scheta = 0;

		}


		if (alpha < 6.28 - (UGOL / 2 * PI1 / 180))
			flag_scheta_1 = 1;

		if (flag_scheta_1 == 1)
		{
			one_2[0] = Tocka_begin[0];
			one_2[1] = Tocka_begin[1];
			one_2[2] = Tocka_begin[2];

			two_2[0] = Tocka_begin_in[0];
			two_2[1] = Tocka_begin_in[1];
			two_2[2] = Tocka_begin_in[2];

			flag_scheta_1 = 0;

		}
		
		double Tocka_real[] = { cos(alpha) * Radius_verch, sin(alpha) * Radius_verch, z_nachalo + visota };
		double Tocka_real_in[] = { cos(alpha) * (Radius_verch - Tolchina), sin(alpha) * (Radius_verch - Tolchina), z_nachalo + visota };
		
		
	
		
		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[1] = Tocka_real[1];
		Tocka_begin_in[0] = Tocka_real_in[0];
		Tocka_begin_in[1] = Tocka_real_in[1];
	}


	int j = 0;
	double alpha_2 = (double)j / (double)Count * PI1 * Radius;

	double Tocka_begin_2[] = { cos(alpha_2) * Radius, sin(alpha_2) * Radius, z_nachalo };
	double Tocka_begin_in_2[] = { cos(alpha_2) * (Radius - Tolchina), sin(alpha_2) * (Radius - Tolchina), z_nachalo };

	for (int j = 0; j <= Count; j++)
	{
		double alpha_2 = (double)j / (double)Count * PI1 * Radius;

		if ((alpha_2 >= (UGOL / 2 * PI1 / 180)))
			flag_scheta_2 = 1;

		if (flag_scheta_2 == 1)
		{
			four_1[0] = Tocka_begin_2[0];
			four_1[1] = Tocka_begin_2[1];
			four_1[2] = Tocka_begin_2[2];

			three_1[0] = Tocka_begin_in_2[0];
			three_1[1] = Tocka_begin_in_2[1];
			three_1[2] = Tocka_begin_in_2[2];

			flag_scheta_2 = 0;

		}


		if (alpha_2 <= 6.28 - (UGOL / 2 * PI1 / 180))
			flag_scheta_3 = 1;

		if (flag_scheta_3 == 1)
		{
			four_2[0] = Tocka_begin_2[0];
			four_2[1] = Tocka_begin_2[1];
			four_2[2] = Tocka_begin_2[2];

			three_2[0] = Tocka_begin_in_2[0];
			three_2[1] = Tocka_begin_in_2[1];
			three_2[2] = Tocka_begin_in_2[2];

			flag_scheta_3 = 0;

		}



		double Tocka_real_2[] = { cos(alpha_2) * Radius, sin(alpha_2) * Radius, z_nachalo };
		double Tocka_real_in_2[] = { cos(alpha_2) * (Radius - Tolchina), sin(alpha_2) * (Radius - Tolchina), z_nachalo };


		
		Tocka_begin_2[0] = Tocka_real_2[0];
		Tocka_begin_2[1] = Tocka_real_2[1];
		Tocka_begin_in_2[0] = Tocka_real_in_2[0];
		Tocka_begin_in_2[1] = Tocka_real_in_2[1];

	}


	glBegin(GL_QUADS);

	glVertex3dv(one_1);
	glVertex3dv(two_1);
	glVertex3dv(three_1);
	glVertex3dv(four_1);

	glVertex3dv(one_2);
	glVertex3dv(two_2);
	glVertex3dv(three_2);
	glVertex3dv(four_2);

	glEnd();

}
//Count не меньше 1000
void KONUS(double z_nachalo, double visota, double Radius, double Radius_verch, double Tolchina,double UGOL, double color1, double color2, double color3, double alpha1)
{
	int Count = 1000;
	glColor4d(color1, color2, color3, alpha1);

	UP_KONUS(z_nachalo, visota, Radius, Radius_verch, Tolchina, Count, UGOL);
	DOWN_KONUS(z_nachalo, visota, Radius, Radius_verch, Tolchina, Count, UGOL);
	STORONA_KONUS(z_nachalo, visota, Radius, Radius_verch, Count, UGOL);
	STORONA_VNUTR_KONUS(z_nachalo, visota, (Radius - Tolchina), (Radius_verch - Tolchina), Count, UGOL);
	//DOP_STORONI_KONUS(z_nachalo, visota, Radius, Radius_verch, Tolchina, Count, UGOL);
	dop_polosi();
	dop_polosi_K();
}





void STORONA_Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina,int Count)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, y_nachalo, sin(alpha) * Radius};
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch,y_nachalo+visota, sin(alpha) * Radius_verch};

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius,y_nachalo, sin(alpha) * Radius};
		double Tocka_real_down[] = { cos(alpha) * Radius_verch,y_nachalo+visota, sin(alpha) * Radius_verch, };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(-Mas_NORMAL[0], -Mas_NORMAL[1], -Mas_NORMAL[2]);

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_begin_down);
		glVertex3dv(Tocka_real_down);
		glVertex3dv(Tocka_real);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[2] = Tocka_real[2];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[2] = Tocka_real_down[2];

	}
	glEnd();
}
void STORONA_VNUTR_Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina,int Count)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, y_nachalo, sin(alpha) * Radius};
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch,y_nachalo+visota, sin(alpha) * Radius_verch};

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius,y_nachalo, sin(alpha) * Radius};
		double Tocka_real_down[] = { cos(alpha) * Radius_verch,y_nachalo+visota, sin(alpha) * Radius_verch, };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], Mas_NORMAL[2]);

		glVertex3dv(Tocka_begin);
		glVertex3dv(Tocka_begin_down);
		glVertex3dv(Tocka_real_down);
		glVertex3dv(Tocka_real);

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[2] = Tocka_real[2];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[2] = Tocka_real_down[2];

	}
	glEnd();

}
//С колвом построения
void STORONA_Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count, double KOLVO)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, y_nachalo, sin(alpha) * Radius };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch,y_nachalo + visota, sin(alpha) * Radius_verch };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius,y_nachalo, sin(alpha) * Radius };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch,y_nachalo + visota, sin(alpha) * Radius_verch, };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(-Mas_NORMAL[0], -Mas_NORMAL[1], -Mas_NORMAL[2]);

		if ((alpha > (6.28/KOLVO)) && (alpha < ((6.28 / KOLVO) + (0.5))))
		{
			glVertex3dv(Tocka_begin);
			glVertex3dv(Tocka_begin_down);
			glVertex3dv(Tocka_real_down);
			glVertex3dv(Tocka_real);
		}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[2] = Tocka_real[2];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[2] = Tocka_real_down[2];

	}
	glEnd();
}
void STORONA_VNUTR_Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, int Count, double KOLVO)
{
	glBegin(GL_QUADS);
	int i = 0;
	double alpha = (double)i / (double)Count * PI1 * Radius;

	double Tocka_begin[] = { cos(alpha) * Radius, y_nachalo, sin(alpha) * Radius };
	double Tocka_begin_down[] = { cos(alpha) * Radius_verch,y_nachalo + visota, sin(alpha) * Radius_verch };

	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;

		double Tocka_real[] = { cos(alpha) * Radius,y_nachalo, sin(alpha) * Radius };
		double Tocka_real_down[] = { cos(alpha) * Radius_verch,y_nachalo + visota, sin(alpha) * Radius_verch, };


		COORD_VEKTOR(Tocka_begin, Tocka_begin_down);
		double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		COORD_VEKTOR(Tocka_real_down, Tocka_begin_down);
		double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
		NORMAL(vektor_a1, vektor_b1);
		glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], Mas_NORMAL[2]);

		if ((alpha > (6.28 / KOLVO)) && (alpha < ((6.28 / KOLVO) + (0.5))))
		{
			glVertex3dv(Tocka_begin);
			glVertex3dv(Tocka_begin_down);
			glVertex3dv(Tocka_real_down);
			glVertex3dv(Tocka_real);
		}

		Tocka_begin[0] = Tocka_real[0];
		Tocka_begin[2] = Tocka_real[2];
		Tocka_begin_down[0] = Tocka_real_down[0];
		Tocka_begin_down[2] = Tocka_real_down[2];

	}
	glEnd();

}

void Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina)
{
	glColor4d(0.522, 0.58, 0.506, 1);
	int Count = 500;
	STORONA_Dvigatel(y_nachalo, visota, Radius, Radius_verch, Tolchina, Count);
	STORONA_VNUTR_Dvigatel(y_nachalo, visota, Radius-Tolchina, Radius_verch-Tolchina, Tolchina, Count);
}
//С колвом построения
void Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, double KOLVO)
{
	glColor4d(0.522, 0.58, 0.506, 1);
	int Count = 500;
	STORONA_Dvigatel(y_nachalo, visota, Radius, Radius_verch, Tolchina, Count,KOLVO);
	STORONA_VNUTR_Dvigatel(y_nachalo, visota, Radius - Tolchina, Radius_verch - Tolchina, Tolchina, Count, KOLVO);
}
// С цветом
void Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, double color1, double color2, double color3, double alpha1)
{
	glColor4d(color1, color2, color3, alpha1);
	int Count = 500;
	STORONA_Dvigatel(y_nachalo, visota, Radius, Radius_verch, Tolchina, Count);
	STORONA_VNUTR_Dvigatel(y_nachalo, visota, Radius - Tolchina, Radius_verch - Tolchina, Tolchina, Count);
}
//С колвом построения + цвет
void Dvigatel(double y_nachalo, double visota, double Radius, double Radius_verch, double Tolchina, double KOLVO, double color1, double color2, double color3, double alpha1)
{
	glColor4d(color1, color2, color3, alpha1);
	int Count = 500;
	STORONA_Dvigatel(y_nachalo, visota, Radius, Radius_verch, Tolchina, Count, KOLVO);
	STORONA_VNUTR_Dvigatel(y_nachalo, visota, Radius - Tolchina, Radius_verch - Tolchina, Tolchina, Count, KOLVO);
}

void Dvigatel_vnutr_krug(double y_nachalo,  double Radius)
{
	int Count = 500;
	glNormal3d(0, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4d(0.631, 0.098, 0.008, 1);
	for (int i = 0; i <= Count; i++)
	{
		double alpha = (double)i / (double)Count * PI1 * Radius;
		glVertex3d(cos(alpha) * Radius,y_nachalo , sin(alpha) * Radius);
	}
	glEnd();
}

//================================================================================================================
/*<<<Функции построения>>>*/
void POSTROENIE_DVIGATELEY() 
{
	/*<<<Двигатель начало>>>*/
	glPushMatrix();
	glTranslated(0, 25 / 2.0 -0.5, 25);
	glRotated(a_dvigatel_1+=0.3,0,1,0);
	//
	GLfloat amb_1[] = { 0.541, 0.459, 0.282, 1. }; //материал 1
	GLfloat dif_1[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_1[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_1 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

	Dvigatel(0, 3, 7.5 / 2.0, 7.5 / 2.0, 0.1);

	GLfloat amb_2[] = { 0.541, 0.416, 0.416, 1. }; //материал 2
	GLfloat dif_2[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_2[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_2 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);

	Dvigatel(3, 0.3, 7.5 / 2.0, 7 / 2.0, 0.1);
	Dvigatel(3.3, 2.5, 7 / 2.0, 7 / 2.0, 0.1);
	Dvigatel(5.8, 0.3, 7 / 2.0, 7.5 / 2.0, 0.1);

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

	//средняя часть
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	//средняя часть доп
	Dvigatel(6.1 + 2.8, 1, 7.5 / 2.0, 7.5 / 2.0, 0.1, 1, 0.824, 0.18, 1);
	Dvigatel(6.1 + 2.8 + 3, 1, 7.5 / 2.0, 7.5 / 2.0, 0.1, 1, 0.824, 0.18, 1);
	Dvigatel(6.1 + 2.8 + 3 + 3.2, 1, 7.5 / 2.0, 7.5 / 2.0, 0.1, 1, 0.824, 0.18, 1);
	//

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);

	Dvigatel(18.6, 4, 7.5 / 2.0, 7.5 / 2.0, 0.1);
	GLfloat amb_Dvig[] = { 0.231,0.059,0, 1. }; //материал двигатель круг
	GLfloat dif_Dvig[] = { 0.231,0.059,0, 1. };
	GLfloat spec_Dvig[] = { 0.231,0.059,0, 1. };
	GLfloat sh_Dvig = 0.01f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_Dvig);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_Dvig);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_Dvig);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_Dvig);
	glShadeModel(GL_SMOOTH);
	Dvigatel_vnutr_krug(6.1, 7.5 / 2.0);
	glPopMatrix();
	/*<<<Двигатель конец>>>*/

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);

	/*<<<Двигатель начало>>>*/
	glPushMatrix();
	glRotated(180, 0, 0, 1);
	glTranslated(0, 25 / 2.0 -0.5, 25);
	glRotated(a_dvigatel_2+=0.3,0,1,0);
	//

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

	Dvigatel(0, 3, 7.5 / 2.0, 7.5 / 2.0, 0.1);

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);

	Dvigatel(3, 0.3, 7.5 / 2.0, 7 / 2.0, 0.1);
	Dvigatel(3.3, 2.5, 7 / 2.0, 7 / 2.0, 0.1);
	Dvigatel(5.8, 0.3, 7 / 2.0, 7.5 / 2.0, 0.1);

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

	//средняя часть
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 1, 0);
	Dvigatel(6.1, 12.5, 7.5 / 2.0, 7.5 / 2.0, 0.1, 10, 1, 0.824, 0.18, 1);
	//средняя часть доп
	Dvigatel(6.1 + 2.8, 1, 7.5 / 2.0, 7.5 / 2.0, 0.1, 1, 0.824, 0.18, 1);
	Dvigatel(6.1 + 2.8 + 3, 1, 7.5 / 2.0, 7.5 / 2.0, 0.1, 1, 0.824, 0.18, 1);
	Dvigatel(6.1 + 2.8 + 3 + 3.2, 1, 7.5 / 2.0, 7.5 / 2.0, 0.1, 1, 0.824, 0.18, 1);
	//

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);

	Dvigatel(18.6, 4, 7.5 / 2.0, 7.5 / 2.0, 0.1);

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_Dvig);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_Dvig);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_Dvig);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_Dvig);
	glShadeModel(GL_SMOOTH);
	Dvigatel_vnutr_krug(6.1, 7.5 / 2.0);
	glPopMatrix();
	/*<<<Двигатель конец>>>*/
}
void POSTROENIE_DETALI_2()
{
	glPushMatrix();
	double KOLVO = 16;
	KONUS_POLOSA(27, 25, 25 / 2.0, 32.5 / 2.0, 2, KOLVO, 0.3, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 0, 1);
	KONUS_POLOSA(27, 25, 25 / 2.0, 32.5 / 2.0, 2, KOLVO, 0.3, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 0, 1);
	KONUS_POLOSA(27, 25, 25 / 2.0, 32.5 / 2.0, 2, KOLVO, 0.3, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 0, 1);
	KONUS_POLOSA(27, 25, 25 / 2.0, 32.5 / 2.0, 2, KOLVO, 0.3, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 0, 1);
	KONUS_POLOSA(27, 25, 25 / 2.0, 32.5 / 2.0, 2, KOLVO, 0.3, 1, 0.824, 0.18, 1);
	glRotated(60, 0, 0, 1);
	KONUS_POLOSA(27, 25, 25 / 2.0, 32.5 / 2.0, 2, KOLVO,0.3, 1, 0.824, 0.18, 1);
	glPopMatrix();
}
void POSTROENIE_DETALI_D1()
{
	//D1
	glPushMatrix();
	KONUS_POLOSA(-9, 10.5, 52.5 / 2.0, 20.2, 2, 8, 0.2, 0.569, 0.196, 0.392, 1);
	glRotated(44, 0, 0, 1);
	KONUS_POLOSA(-9, 10.5, 52.5 / 2.0, 20.2, 2, 8, 0.2, 0.569, 0.196, 0.392, 1);
	glRotated(44, 0, 0, 1);
	KONUS_POLOSA(-9, 10.5, 52.5 / 2.0, 20.2, 2, 8, 0.2, 0.569, 0.196, 0.392, 1);
	glRotated(44, 0, 0, 1);
	KONUS_POLOSA(-9, 10.5, 52.5 / 2.0, 20.2, 2, 8, 0.2, 0.569, 0.196, 0.392, 1);
	glRotated(44, 0, 0, 1);
	KONUS_POLOSA(-9, 10.5, 52.5 / 2.0, 20.2, 2, 8, 0.2, 0.569, 0.196, 0.392, 1);
	glRotated(44, 0, 0, 1);
	KONUS_POLOSA(-9, 10.5, 52.5 / 2.0, 20.2, 2, 8, 0.2, 0.569, 0.196, 0.392, 1);
	glRotated(44, 0, 0, 1);
	KONUS_POLOSA(-9, 10.5, 52.5 / 2.0, 20.2, 2, 8, 0.2, 0.569, 0.196, 0.392, 1);

	glPopMatrix();

}

void POSTROENIE_DETALI_V()
{
	glPushMatrix();
	//V
	glTranslated(0,0,-4);
	glRotated(a+=0.8, 0 , 0 , 1);
	KONUS(49, 3.5, 46.5 / 2.0, 46 / 2.0, 2, 0.569, 0.196, 0.392, 1);
	KONUS(52.5, 3.5, 46 / 2.0, 46.5 / 2.0, 2, 0.569, 0.196, 0.392, 1);


	//---------------
	GLfloat amb_1[] = { 0.541, 0.459, 0.282, 1. }; //материал 1
	GLfloat dif_1[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_1[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_1 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(49.5, 6, 52 / 2.0, 48.5 / 2.0, 2.7,350, 0.569, 0.196, 0.392, 1);

	glPopMatrix();
}
void POSTROENIE_DETALI_V1()
{
	glPushMatrix();
	
	//V1
	KONUS(49, 7.5, 37.5 / 2.0, 35 / 2.0, 2, 0.569, 0.196, 0.392, 1);

	//======================
	GLfloat amb_2[] = { 0.541, 0.416, 0.416, 1. }; //материал 2
	GLfloat dif_2[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_2[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_2 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);
	glPushMatrix();
	glTranslated(-17.5, -7.5, 0);
		KONUS(50, 8, 2, 2, 2, 0.569, 0.196, 0.392, 1);
		glTranslated(0, 0, 59);
			SPHERA(1.5, 20, 20);
			KONUS_LINE(0, 4, 2, 2, 2, 0.569, 0.196, 0.392, 1);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);
	glPushMatrix();
	glTranslated(-17.5, 7.5, 0);
		KONUS(50, 8, 2, 2, 2, 0.569, 0.196, 0.392, 1);
		glTranslated(0, 0, 59);
			SPHERA(1.5, 20, 20);
			KONUS_LINE(0, 4.5, 2, 2, 2, 0.569, 0.196, 0.392, 1);
			glTranslated(0, 1, 0);
				KONUS_LINE(0, 3, 2, 2, 2, 0.569, 0.196, 0.392, 1);
	glPopMatrix();
	//========================

	//---------------
	GLfloat amb_1[] = { 0.541, 0.459, 0.282, 1. }; //материал 1
	GLfloat dif_1[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_1[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_1 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	glRotated(45,0,0,1);
	KONUS(50.5, 4.5,41/ 2.0, 36.5 / 2.0, 2.5,350, 0.569, 0.196, 0.392, 1);
	
	glPopMatrix();
}
void POSTROENIE_DETALI_1()
{
	glPushMatrix();

	// 1 
	KONUS(14.5, 12.5, 30 / 2.0, 25 / 2.0, 2, 0.569, 0.196, 0.392, 1);

	glRotated(a_detali_1-=0.6,0,0,1);

	KONUS(1, 3.5, 42.5 / 2.0, 42.5 / 2.0, 3, 0.569, 0.196, 0.392, 1); // передаем начало по высоте, высоту, основание нижнее, основание верхнее, и толщину + цвет
	KONUS(4.5, 10, 42.5 / 2.0, 30 / 2.0, 2, 0.569, 0.196, 0.392, 1);
	
	//---------------
	GLfloat amb_21[] = { 0.450, 0.300, 0.300, 1. }; //материал 2
	GLfloat dif_21[] = { 0.450, 0.300, 0.300, 1. };
	GLfloat spec_21[] = { 0.4, 0.4, 0.1, 1. };
	GLfloat sh_21 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_21);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_21);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_21);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_21);
	glShadeModel(GL_SMOOTH);

	KONUS(1.75, 1.75,44/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(1.25, 0.5,43/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(3.5, 0.5,44/ 2.0, 43/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	glRotated(60,0,0,1);
	KONUS(1.75, 1.75,44/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(1.25, 0.5,43/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(3.5, 0.5,44/ 2.0, 43/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	glRotated(60,0,0,1);
	KONUS(1.75, 1.75,44/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(1.25, 0.5,43/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(3.5, 0.5,44/ 2.0, 43/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	glRotated(60,0,0,1);
	KONUS(1.75, 1.75,44/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(1.25, 0.5,43/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(3.5, 0.5,44/ 2.0, 43/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	glRotated(60,0,0,1);
	KONUS(1.75, 1.75,44/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(1.25, 0.5,43/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(3.5, 0.5,44/ 2.0, 43/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	glRotated(60,0,0,1);
	KONUS(1.75, 1.75,44/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(1.25, 0.5,43/ 2.0, 44/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	KONUS(3.5, 0.5,44/ 2.0, 43/ 2.0, 2.5,335, 0.569, 0.196, 0.392, 1);
	
	glPopMatrix();
}

/*
void TREUGOLNIK_1()
{
	glTranslated(13, 13, 0);

	double storona = 12;

	double z = -15;
	double z1 = z + 3.0;
	double one[] = { storona,storona,z1 }, two[] = { storona,0,z1 }, three[] = { 0,storona,z1 };

	COORD_VEKTOR(one, two);
	double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
	COORD_VEKTOR(one, three);
	double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
	NORMAL(vektor_a1, vektor_b1);
	glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], -Mas_NORMAL[2]);
	TRIANGLE(one, two, three);

	double one1[] = { storona,storona,z }, two1[] = { storona,0,z }, three1[] = { 0,storona,z };
	COORD_VEKTOR(one1, two1);
	vektor_a1[0] = -Vektor[0];
	vektor_a1[1] = Vektor[1];
	vektor_a1[2] = Vektor[2];
	COORD_VEKTOR(one1, three1);
	vektor_b1[0] = -Vektor[0];
	vektor_b1[1] = -Vektor[1];
	vektor_b1[2] = Vektor[2];
	NORMAL(vektor_a1, vektor_b1);
	glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], -Mas_NORMAL[2]);
	TRIANGLE(one1, two1, three1);
}
void TREUGOLNIK_2()
{
	glTranslated(10.3, -15.3, 0);
	glRotated(-102, 0, 0, 1);
	double storona = 12;

	double z = -15;
	double z1 = z + 3.0;
	double one[] = { storona,storona,z1 }, two[] = { storona,0,z1 }, three[] = { 0,storona,z1 };

	COORD_VEKTOR(one, two);
	double vektor_a1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
	COORD_VEKTOR(one, three);
	double vektor_b1[] = { -Vektor[0],-Vektor[1],Vektor[2] };
	NORMAL(vektor_a1, vektor_b1);
	glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], -Mas_NORMAL[2]);
	TRIANGLE(one, two, three);

	double one1[] = { storona,storona,z }, two1[] = { storona,0,z }, three1[] = { 0,storona,z };
	COORD_VEKTOR(one1, two1);
	vektor_a1[0] = -Vektor[0];
	vektor_a1[1] = Vektor[1];
	vektor_a1[2] = Vektor[2];
	COORD_VEKTOR(one1, three1);
	vektor_b1[0] = -Vektor[0];
	vektor_b1[1] = -Vektor[1];
	vektor_b1[2] = Vektor[2];
	NORMAL(vektor_a1, vektor_b1);
	glNormal3d(Mas_NORMAL[0], Mas_NORMAL[1], -Mas_NORMAL[2]);
	TRIANGLE(one1, two1, three1);
}
void POSTROENIE_TREUGOLNIKOW()
{
	glPushMatrix();
	TREUGOLNIK_1();
	glPopMatrix();
	glPushMatrix();
	TREUGOLNIK_2();
	glPopMatrix();
}
*/
//================================================================================================================
void LUCH()
{
	glPushMatrix();
	glRotated(a_luch+= 1, 0, 0, 1);
	glTranslated(0,0,-10);
	glScaled(0.38,0.38,0.5);
	KONUS(1-n_luch+1, n_luch, 3, 3, 3, 0.569, 0.196, 0.392, 1);
	if ((n_luch <= 90)&&(flag_obratno_luch == 0)) 
		n_luch+= 0.2;
	if (n_luch >= 90)
		flag_obratno_luch = 1; 
	if (flag_obratno_luch == 1)
		n_luch -= 0.2;
	if (n_luch == 85) 
		flag_obratno_luch = 0;
	glTranslated(0, 0, -5);
	X_lucha = X_D;
	Y_lucha = Y_D;
	Z_lucha = (Z_D - (105*0.25));

	glPushMatrix();
		glScaled(0.25, 0.25, 0.25);
		Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
	glPopMatrix();
	glTranslated(0, 0, -25);
	glPushMatrix();
		glScaled(0.25, 0.25, 0.25);
		Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
	glPopMatrix();
	glTranslated(0, 0, -40);
	glPushMatrix();
		glScaled(0.25, 0.25, 0.25);
		Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
			glRotated(90, 0, 0, 1);
			Dvigatel(10, 10, 3, 3, 1);
	glPopMatrix();

	glPopMatrix();
}
//================================================================================================================

void Render(OpenGL *ogl)
{       	
	n_otrisovki += 0.00001;
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);// закрываем текстуры

	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_ALPHA_TEST);


	//настройка материала
	/*GLfloat amb[] = { 0.2, 0.2, 0.1, 1. }; //материал
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };*/
	//-----------My
	/*GLfloat amb[] = { 0.3, 0.3, 0.3, 1. }; //материал серый
	GLfloat dif[] = { 0.902, 0.902, 0.902, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };*/

	GLfloat amb[] = { 0.133, 0.196, 0.2, 1. }; //материал
	GLfloat dif[] = { 0.133, 0.196, 0.2, 1. };
	GLfloat spec[] = { 0.4, 0.4, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	GLfloat amb__2[] = { 0.133, 0.196, 0.2, 1. }; //материал
	GLfloat dif__2[] = { 0.133, 0.196, 0.2, 1. };
	GLfloat spec__2[] = { 0.4, 0.4, 0.3, 1. };
	GLfloat sh__2 = 0.1f * 256;

	GLfloat amb__3[] = { 0.133, 0.196, 0.2, 1. }; //материал
	GLfloat dif__3[] = { 0.133, 0.196, 0.2, 1. };
	GLfloat spec__3[] = { 0.4, 0.4, 0.3, 1. };
	GLfloat sh__3 = 0.1f * 256;

	///-------------------My
	GLfloat em[] = {0, 0, 0, 1.0};
	//GLfloat em[] = {0.184, 0.02, 0.412, 0.5};//СИРЕНЕВЫЙ СВЕТ ОТ ПРИЗМЫ

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);
	//-----------------my
	glMaterialfv(GL_FRONT, GL_EMISSION, em);

    //чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================

	//Прогать тут  

	//=================================================================================================
	glPushMatrix();
		//Тудунь-сюдунь
	glTranslated(X_D,Y_D,Z_D);
	glRotated(180, 0, 0, 1);
	glScaled(0.5, 0.5, 0.5);

	/*<<<< Общий цвет >>>>>*/
	glColor4d(0.522, 0.58, 0.506,1);

	//KUBIK_1(0, 0, 1, 10);
	

	GLfloat amb_3[] = { 0.161,0.141,0.125, 1. }; //материал 3 коричн-черн
	GLfloat dif_3[] = { 0.161,0.141,0.125, 1. };
	GLfloat spec_3[] = { 0.5, 0.5, 0.3, 1. };
	GLfloat sh_3 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_3);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_3);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_3);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_3);
	glShadeModel(GL_SMOOTH);

	POSTROENIE_DETALI_1();
	
	//D
	KONUS(-15, 3.5, 57.5 / 2.0, 57.5 / 2.0, 2,60, 0.569, 0.196, 0.392, 1);
	KONUS(-11.5, 2.5, 57.5 / 2.0, 52.5 / 2.0, 2,60, 0.569, 0.196, 0.392, 1);

	
	POSTROENIE_DETALI_V1();

	// >>> Построения деталей <<<
	GLfloat amb_1[] = { 0.541, 0.459, 0.282, 1. }; //материал 1
	GLfloat dif_1[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_1[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_1 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_1);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_1);
	glShadeModel(GL_SMOOTH);

		POSTROENIE_DETALI_D1();
		POSTROENIE_DETALI_2();



	GLfloat amb_21[] = { 0.450, 0.300, 0.300, 1. }; //материал 2
	GLfloat dif_21[] = { 0.450, 0.300, 0.300, 1. };
	GLfloat spec_21[] = { 0.4, 0.4, 0.1, 1. };
	GLfloat sh_21 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_21);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_21);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_21);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_21);
	glShadeModel(GL_SMOOTH);
			
	POSTROENIE_DETALI_V();


	GLfloat amb_2[] = { 0.541, 0.416, 0.416, 1. }; //материал 2
	GLfloat dif_2[] = { 0.541, 0.416, 0.416, 1. };
	GLfloat spec_2[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh_2 = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_2);
	glShadeModel(GL_SMOOTH);

		POSTROENIE_DVIGATELEY();

		

	// >>>					  <<<

	GLfloat amb_luch[] = { 1,0.992,0.82, 1.0 }; //материал луча
	GLfloat dif_luch[] = { 1,0.988,0.678, 1.0 };
	GLfloat spec_luch[] = { 1, 1, 1, 1. };
	GLfloat sh_luch = 5 * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_luch);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_luch);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_luch);
	glMaterialf(GL_FRONT, GL_SHININESS, sh_luch);
	glShadeModel(GL_SMOOTH);
	if (FLAG_LUCH == 1)
	{LUCH();
		if ((X_lucha<=(X_KUB+POLUSTORONA_KUB))&&(X_lucha>=(X_KUB-POLUSTORONA_KUB))&&(Y_lucha<=(Y_KUB+POLUSTORONA_KUB))&&(Y_lucha>=(Y_KUB-POLUSTORONA_KUB))&&(Z_lucha<=(Z_KUB+POLUSTORONA_KUB))&&(Z_lucha>=(Z_KUB-POLUSTORONA_KUB)))
	{
		KOLVO_MATERIALOV+=1;

		amb[0] = 0.9;
		amb[1] = 0.9;
		amb[2] = 0.9;
		amb[3] = 1.0;
		dif[0] = 0.9;
		dif[1] = 0.9;
		dif[2] = 0.9;
		dif[3] = 0.9;
	}
		if ((X_lucha <= (X_KUB_2 + POLUSTORONA_KUB_2)) && (X_lucha >= (X_KUB_2 - POLUSTORONA_KUB_2)) && (Y_lucha <= (Y_KUB_2 + POLUSTORONA_KUB_2)) && (Y_lucha >= (Y_KUB_2 - POLUSTORONA_KUB_2)) && (Z_lucha <= (Z_KUB_2 + POLUSTORONA_KUB_2)) && (Z_lucha >= (Z_KUB_2 - POLUSTORONA_KUB_2)))
		{
			KOLVO_MATERIALOV += 1;

			amb__2[0] = 0.9;
			amb__2[1] = 0.9;
			amb__2[2] = 0.9;
			amb__2[3] = 1.0;
			dif__2[0] = 0.9;
			dif__2[1] = 0.9;
			dif__2[2] = 0.9;
			dif__2[3] = 0.9;
		}
		if ((X_lucha <= (X_KUB_3 + POLUSTORONA_KUB_3)) && (X_lucha >= (X_KUB_3 - POLUSTORONA_KUB_3)) && (Y_lucha <= (Y_KUB_3 + POLUSTORONA_KUB_3)) && (Y_lucha >= (Y_KUB_3 - POLUSTORONA_KUB_3)) && (Z_lucha <= (Z_KUB_3 + POLUSTORONA_KUB_3)) && (Z_lucha >= (Z_KUB_3 - POLUSTORONA_KUB_3)))
		{
			KOLVO_MATERIALOV += 1;

			amb__3[0] = 0.9;
			amb__3[1] = 0.9;
			amb__3[2] = 0.9;
			amb__3[3] = 1.0;
			dif__3[0] = 0.9;
			dif__3[1] = 0.9;
			dif__3[2] = 0.9;
			dif__3[3] = 0.9;
		}
	}
	glPopMatrix();


	//=================================================================================================
	/*<<<Объекты>>>*/
	glPushMatrix();
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	X_KUB = -30;
	Y_KUB = -30;
	Z_KUB = -30;
	STORONA_KUB = 30;
	POLUSTORONA_KUB = STORONA_KUB / 2.0;

	KUBIK_1(X_KUB,Y_KUB,Z_KUB,STORONA_KUB);


	glMaterialfv(GL_FRONT, GL_AMBIENT, amb__2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif__2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec__2);
	glMaterialf(GL_FRONT, GL_SHININESS, sh__2);

	X_KUB_2 = 25;
	Y_KUB_2 = -25;
	Z_KUB_2 = -10;
	STORONA_KUB_2 = 10;
	POLUSTORONA_KUB_2 = STORONA_KUB_2 / 2.0;

	KUBIK_1(X_KUB_2, Y_KUB_2, Z_KUB_2, STORONA_KUB_2);

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb__3);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif__3);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec__3);
	glMaterialf(GL_FRONT, GL_SHININESS, sh__3);

	X_KUB_3 = 20;
	Y_KUB_3 = 30;
	Z_KUB_3 = -20;
	STORONA_KUB_3 = 18;
	POLUSTORONA_KUB_3 = STORONA_KUB_3 / 2.0;

	KUBIK_1(X_KUB_3, Y_KUB_3, Z_KUB_3, STORONA_KUB_3);

	glPopMatrix();




	//сцена
	glClearColor(0.117,0.11,0.148, 1);
	/*
	GLfloat amb_S[] = { 0.107,0.1,0.135, 1 }; //материал сцены
	GLfloat dif_S[] = { 0.107,0.1,0.135, 1 };
	GLfloat spec_S[] = { 0, 0, 0, 0};
	GLfloat sh_S = 0.1f * 256;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_S);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_S);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_S);
	glMaterialf(GL_FRONT, GL_SHININESS, 0);
	glShadeModel(GL_SMOOTH);
		
	glPushMatrix();
	DOP_PLOSKOST(250,-250, 0.831, 0.525, 0.125, 1);//v
	DOP_PLOSKOST_V(250, 250, 0.831, 0.525, 0.125, 1);//^
	DOP_PLOSKOST_S(250 +1, -250, 150, 1, 1, 1, 1);
	DOP_PLOSKOST_S(250 +1, 250, -150, 1, 1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DOP_PLOSKOST_S(250, -250, 250, 0.58, 0.91, 1, 0.3);
	DOP_PLOSKOST_S(250, 250, -250, 0.58, 0.91, 1, 0.3);
	glDisable(GL_BLEND);
	glPopMatrix();
	*/
	//================================================================================================================
	//текст сообщения вверху слева, если надоест - закоментировать, или заменить =)

	//===============================================================================================================

	
	char c[600];  //максимальная длина сообщения
	if (FLAG_HELP == 0) 
	{
		if (FLAG_TOCHNOST_PEREMECH == 1)
		{
			sprintf_s(c, "...............\n"
				"H - дополнительная информация/управление\n\n"
				"Данные буровой платформы:\n"
				"Количество материалов : %f \n"
				"Ускорение: Выкл", KOLVO_MATERIALOV);
		}
		if (FLAG_TOCHNOST_PEREMECH == 0)
		{
			sprintf_s(c, "...............\n"
				"H - дополнительная информация/управление\n\n"
				"Данные буровой платформы:\n"
				"Количество материалов : %f \n"
				"Ускорение: Вкл", KOLVO_MATERIALOV);
		}
	}
	if (FLAG_HELP == 1) 
	{
		if (FLAG_TOCHNOST_PEREMECH == 1)
		{
			sprintf_s(c, "...............\n"
				"H - назад\n\n"
				"Управление светом:\n"
				"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
				"R - установить камеру и свет в начальное положение\n"
				"F - переместить свет в точку камеры\n"
				"B - переместить свет в 'край'(-x;+y,+z)\n\n"
				"Special:\n"
				"Движение:\n"
				"  XY: WASD / Стрелки(диагональное)\n"
				"  Z: PgDn,PgUp\n"
				"  Ускорение: Shift\n"
				"Бурение:\n"
				"  Вкл/Выкл луч: Q"
				"Технические данные:\n"
				"  Количество отрисовки изображения/100000 :%f \n\n"
				"Данные буровой платформы:\n"
				"Количество материалов : %f \n"
				"Ускорение: Выкл", n_otrisovki, KOLVO_MATERIALOV);
		}

		if(FLAG_TOCHNOST_PEREMECH == 0)
		{
			sprintf_s(c, "...............\n"
				"H - назад\n\n"
				"Управление светом:\n"
				"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
				"R - установить камеру и свет в начальное положение\n"
				"F - переместить свет в точку камеры\n"
				"B - переместить свет в 'край'(-x;+y,+z)\n\n"
				"Special:\n"
				"Движение:\n"
				"  XY: WASD / Стрелки(диагональное)\n"
				"  Z: PgDn,PgUp\n"
				"  Ускорение: Shift\n"
				"Бурение:\n"
				"  Вкл/Выкл луч: Q"
				"Технические данные:\n"
				"  Количество отрисовки изображения/100000 :%f \n\n"
				"Данные буровой платформы:\n"
				"Количество материалов : %f \n"
				"Ускорение: Вкл", n_otrisovki, KOLVO_MATERIALOV);
		}


	}
		ogl->message = std::string(c);

}   //конец тела функции

