// luweiqi513.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <gl/glut.h>
#include <gl/glaux.h>
#include <gl/SOIL.h>
#include "fstream"
#include "iostream"
#include "string"
#include <math.h>
#include <gl/math3d.h>
using namespace std;
 
//读文件
int num_texture_file;
 
string*texture_file_name;
 
int num_texture_quality;
 
struct texture_quality
{
	GLfloat ambient[4];//环境光
	GLfloat diffuse[4];//漫反射
	GLfloat specular[4];//镜面反射
	GLfloat emission[4];//辐射
	GLfloat shiness;//光斑
	int texture_index;
};
texture_quality * texture_quality_list;
 
int num_vertexs;//顶点个数
struct vertex//顶点数组
{
	GLfloat xyz[4];
};
vertex*vertex_list;
 
int num_map;
struct map//贴图
{
	GLfloat xy[2];
};
map*map_list;
 
int num_normal;
struct normal//法线
{
	GLfloat xyz[3];
};
normal*normal_list;
 
int num_sub_model;
 
GLfloat scale[3];
 
struct triangle
{
	int abc[9];
};
 
struct sub_model
{
	int triangle_num;
	triangle * triangle_list;
	int quality_index;
};
sub_model* sub_model_list;
//读文件结束
AUX_RGBImageRec **TextureImage;//多个bmp文件
GLuint*  texture;           // 存储多个纹理
 
BITMAP BMP;
 
GLdouble translateX, translateY, translateZ;//平移
 
GLdouble rotateAngle, rotateX, rotateY, rotateZ;//旋转
 
GLdouble scaleX = 1.0, scaleY = 1.0, scaleZ = 1.0;//缩放
 
GLdouble fovy = 0, aspect = 0, zFar = 0, zNear = 0;//投影
 
GLdouble eyex = 0.1, eyey = 0.2, eyez = 0, centerx = 0, centery = 0, centerz = 0, upx = 0, upy = 0, upz = 1.0;//三维观察
 
GLdouble* indexA = NULL, *indexB = NULL, *indexC = NULL, *indexD = NULL;
//////////////////////////////////////////
bool LoadTexture(LPCSTR szFileName) // Creates Texture From A Bitmap File
{
	GLuint *texid=new GLuint;
	HBITMAP hBMP; // Handle Of The Bitmap
	BMP; // Bitmap Structure
 
	glGenTextures(1, texid); // Create The Texture
	hBMP = (HBITMAP)LoadImageA(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
 
	if (!hBMP) // Does The Bitmap Exist?
	{
		cout<<"load bit map error"<<endl;
		return FALSE; // If Not Return False
	}
 
 
	GetObject(hBMP, sizeof(BMP), &BMP); 
 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
 
	glBindTexture(GL_TEXTURE_2D, *texid); // Bind To The Texture ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Mag Filter
	glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
 
	DeleteObject(hBMP); // Delete The Object
 
	return TRUE; // Loading Was Successful
}
//////////////////////////////////////////
 
AUX_RGBImageRec *load_BMP(string file_name)
{
	cout << file_name;
	ifstream file(file_name);
	if (!file)
	{
		cout << "cannot open bmp file:" << file_name << endl;
		return NULL;
	}
	return auxDIBImageLoadA(file_name.c_str());
}
//////////////////////////////////////////////
 
 
//////////////////////////////////////////////
int load_textures(string* file_name_list, int num_texture_file)
{
	int Status = FALSE;                         // 状态指示器
	TextureImage = new AUX_RGBImageRec *[num_texture_file];         // 创建纹理的存储空间
	memset(TextureImage, 0, sizeof(void *)*num_texture_file);       // 将指针设为 NULL
 
	for (int i = 0; i < num_texture_file; i++)
	{
		//LoadTexture(file_name_list[i].c_str(), texture[i]);
		
		/*if (TextureImage[i] = load_BMP(file_name_list[i]))
		{
			Status = TRUE;                          // 将 Status 设为 TRUE
		}*/
	}
 
	return Status;
}
 
void readFile(void)
{
	string s;
 
	string file_name = "luweiqi.txt";
	ifstream fin(file_name);
	if (!fin) { cout << "Error opening " << file_name << " for input" << endl; exit(-1); }
	//读纹理文件名
	fin >> s;
	num_texture_file = atoi(s.c_str());
	cout << num_texture_file << endl;
	texture_file_name = new string[num_texture_file];
	texture = new GLuint[num_texture_file];
	for (int i = 0; i < num_texture_file; i++)
	{
		fin >> texture_file_name[i];
		//texture[i] = i;
	}
 
	//读纹理文件的光照属性
	fin >> s;
	num_texture_quality = atoi(s.c_str());
	cout << num_texture_quality << endl;
	texture_quality_list = new texture_quality[num_texture_quality];
	for (int i = 0; i < num_texture_quality; i++)
	{
		for (int j = 0; j < 4; j++) {
			fin >> s;
			texture_quality_list[i].ambient[j] = (GLfloat)atof(s.c_str());
		}
		for (int j = 0; j < 4; j++) {
			fin >> s;
			texture_quality_list[i].diffuse[j] = (GLfloat)atof(s.c_str());
		}
		for (int j = 0; j < 4; j++) {
			fin >> s;
			texture_quality_list[i].specular[j] = (GLfloat)atof(s.c_str());;
		}
		for (int j = 0; j < 4; j++) {
			fin >> s;
			texture_quality_list[i].emission[j] = (GLfloat)atof(s.c_str());
		}
		fin >> s;
		texture_quality_list[i].shiness = (GLfloat)atof(s.c_str());
		fin >> s;
		texture_quality_list[i].texture_index = atoi(s.c_str())-1;
	}
 
	//读顶点
	fin >> s;
	num_vertexs = atoi(s.c_str());
	cout << num_vertexs << endl;
	vertex_list = new vertex[num_vertexs];
	for (int i = 0; i < num_vertexs; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fin >> s;
			vertex_list[i].xyz[j] = (GLfloat)atof(s.c_str());
		}
		vertex_list[i].xyz[3] = 1.0;
	}
 
	//读贴图
	fin >> s;
	num_map = atoi(s.c_str());
	cout << num_map << endl;
	map_list = new map[num_map];
	for (int i = 0; i < num_map; i++)
	{
		fin >> s;
		map_list[i].xy[0] = (GLfloat)atof(s.c_str());
		fin >> s;
		map_list[i].xy[1] = (GLfloat)atof(s.c_str());
		//cout <<map_list[i].xy[0]<< map_list[i].xy[1] <<endl;
	}
 
	//读法线
	fin >> s;
	num_normal = atoi(s.c_str());
	cout << num_normal <<endl;
	normal_list = new normal[num_normal];
	for (int i = 0; i < num_normal; i++)
	{
		fin >> s;
		normal_list[i].xyz[0] = (GLfloat)atof(s.c_str());
		fin >> s;
		normal_list[i].xyz[1] = (GLfloat)atof(s.c_str());
		fin >> s;
		normal_list[i].xyz[2] = (GLfloat)atof(s.c_str());
	}
 
	//子模型数量
	fin >> s;
	num_sub_model = atoi(s.c_str());
	cout << num_sub_model << endl;
	sub_model_list = new sub_model[num_sub_model];
 
	//缩放
	for (int i = 0; i < 3; i++)
	{
		fin >> s;
		scale[i] = (GLfloat)atof(s.c_str());
	}
 
	//子模型
	for (int i = 0; i < num_sub_model; i++)
	{
		fin >> s;
		sub_model_list[i].triangle_num = atoi(s.c_str());
		cout << sub_model_list[i].triangle_num << endl;
		sub_model_list[i].triangle_list = new triangle[sub_model_list[i].triangle_num];
 
		fin >> s;
		sub_model_list[i].quality_index = atoi(s.c_str())-1;
 
		for (int j = 0; j < sub_model_list[i].triangle_num; j++)
		{
			for (int k = 0; k < 9; k++)
			{
				fin >> s;
				//cout << s << " ";
				sub_model_list[i].triangle_list[j].abc[k] = atoi(s.c_str());
			}//triangle
			//cout << endl;
		}//triangle_num
	}//sub_model
}
 
 
void display(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);                           //当前背景色填充窗口
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//在display中设置投影和观察视角，这样才可以在每次修改参数后显示
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluPerspective(fovy * 20, aspect, zFar, zNear);
 
	gluLookAt(eyex, eyey, eyez,
		centerx, centery, centerz,
		upx, upy, upz);//观察正向
 
	glTranslatef((GLdouble)translateX, (GLdouble)translateY, (GLdouble)translateZ);
 
	glRotatef((GLdouble)rotateAngle * 20, (GLdouble)rotateX, (GLdouble)rotateY, (GLdouble)rotateZ);
 
	glScalef((GLdouble)scaleX, (GLdouble)scaleY, (GLdouble)scaleZ);
 
	for (size_t i = 0; i <num_sub_model; i++)//
	{
		//LoadTexture(texture_file_name[i].c_str(), texture[i]);
		glMaterialfv(GL_FRONT, GL_AMBIENT, texture_quality_list[sub_model_list[i].quality_index].ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, texture_quality_list[sub_model_list[i].quality_index].diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, texture_quality_list[sub_model_list[i].quality_index].specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, texture_quality_list[sub_model_list[i].quality_index].emission);
		glMaterialf(GL_FRONT, GL_SHININESS, texture_quality_list[sub_model_list[i].quality_index].shiness);
 
 
		//纹理
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 线形滤波
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 线形滤波
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
 
		//LoadTexture(texture_file_name[i].c_str());
 
		int index = texture_quality_list[sub_model_list[i].quality_index].texture_index;
		
		/*texture[i] = SOIL_load_OGL_texture
		(
			texture_file_name[index].c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y
		);
		if (!texture[i]) {
			cout<<"load error"<<endl;
			exit(-1);
		}*/
		//glBindTexture(GL_TEXTURE_2D, texture[i]);
		
		//三角形
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[index]->sizeX, TextureImage[index]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[index]->data);		
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < sub_model_list[i].triangle_num; j++)
		{
			glNormal3fv(normal_list[sub_model_list[i].triangle_list[j].abc[2] - 1].xyz);
			glTexCoord2f(map_list[sub_model_list[i].triangle_list[j].abc[1] - 1].xy[0], 1-map_list[sub_model_list[i].triangle_list[j].abc[1] - 1].xy[1]);
			glVertex3fv(vertex_list[sub_model_list[i].triangle_list[j].abc[0] - 1].xyz);
			
			glNormal3fv(normal_list[sub_model_list[i].triangle_list[j].abc[5] - 1].xyz);
			glTexCoord2f(map_list[sub_model_list[i].triangle_list[j].abc[4] - 1].xy[0], 1-map_list[sub_model_list[i].triangle_list[j].abc[4] - 1].xy[1]);
			glVertex3fv(vertex_list[sub_model_list[i].triangle_list[j].abc[3] - 1].xyz);
 
			glNormal3fv(normal_list[sub_model_list[i].triangle_list[j].abc[8] - 1].xyz);
			glTexCoord2f(map_list[sub_model_list[i].triangle_list[j].abc[7] - 1].xy[0], 1-map_list[sub_model_list[i].triangle_list[j].abc[7] - 1].xy[1]);
			glVertex3fv(vertex_list[sub_model_list[i].triangle_list[j].abc[6] - 1].xyz);
		}
		glEnd();
	}
 
	//画出坐标轴
	glBegin(GL_LINES);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, -1.0);
	glEnd();
 
	glFlush();                                              //输出缓冲区 
}
 
void keyBoardFunc(unsigned char key, int x, int y)
{
	if (key == 'q')
		if (indexA)*indexA += 0.05;
	if (key == 'a')
		if (indexA)*indexA -= 0.05;
	if (key == 'w')
		if (indexB)*indexB += 0.05;
	if (key == 's')
		if (indexB)*indexB -= 0.05;
	if (key == 'e')
		if (indexC)*indexC += 0.05;
	if (key == 'd')
		if (indexC)*indexC -= 0.05;
	if (key == 'r')
		if (indexD)*indexD += 0.05;
	if (key == 'f')
		if (indexD)*indexD -= 0.05;
 
	//输出参数方便调整
	cout << "glScale:\t" << scaleX << '\t' << scaleY << '\t' << scaleZ << '\t' << endl;
	cout << "glRotate:\t" << rotateAngle * 20 << '\t' << rotateX << '\t' << rotateY << '\t' << rotateZ << endl;
	cout << "glTranslate:\t" << translateX << '\t' << translateY << '\t' << translateZ << endl;
	cout << "glLookAt eye:\t" << eyex << '\t' << eyey << '\t' << eyez << endl;
	cout << "glLookAtCenter:\t" << centerx << '\t' << centery << '\t' << centerz << endl;
	cout << "glLookAt up:\t" << upx << '\t' << upy << '\t' << upz << endl;
	cout << "glperspective:\t" << fovy * 20 << '\t' << aspect << '\t' << zFar << '\t' << zNear << '\t' << endl << endl;
	display();
}
 
//根据菜单中选中的值设置修改哪些属性值
void chooseMode(GLint menuIteemNum)
{
	switch (menuIteemNum)
	{
	case 0:
		indexA = &translateX; indexB = &translateY; indexC = &translateZ; indexD = NULL; break;
	case 1:
		indexA = &scaleX; indexB = &scaleY; indexC = &scaleZ; indexD = NULL; break;
	case 2:
		indexA = &rotateAngle; indexB = &rotateX; indexC = &rotateY; indexD = &rotateZ; break;
	case 3:
		indexA = &fovy; indexB = &aspect; indexC = &zFar; indexD = &zNear; break;
	case 4:
		indexA = &eyex; indexB = &eyey; indexC = &eyez; indexD = NULL; break;
	case 5:
		indexA = ¢erx; indexB = ¢ery; indexC = ¢erz; indexD = NULL; break;
	case 6:
		indexA = &upx; indexB = &upy; indexC = &upz; indexD = NULL; break;
	default:
		indexA = NULL; indexB = NULL; indexC = NULL; indexD = NULL; break;
	}
}
 
//
void mouseFunc(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	glutCreateMenu(chooseMode);
	glutAddMenuEntry("translate", 0);
	glutAddMenuEntry("scale", 1);
	glutAddMenuEntry("rotate", 2);
	glutAddMenuEntry("perspective", 3);
	glutAddMenuEntry("glLookAt eye", 4);
	glutAddMenuEntry("glLookAt center", 5);
	glutAddMenuEntry("glLookAt up", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
 
void init()
{
	readFile();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
 
	//load_textures(texture_file_name, num_texture_file);
	//创建光源
	glEnable(GL_LIGHT0);
 
	float fAmbientColor[] = { 0.2f, 0.0f,0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, fAmbientColor);
 
	float fDiffuseColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fDiffuseColor);
 
	float fSpecularColor[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, fSpecularColor);
 
	float fPosition[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, fPosition);
 
	/*
	float fspotDirection[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, fspotDirection);
	float f[] = { 0.1f };
	glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, f);
	*/
	/*
	GLfloat ambient_lightModel[] = { 10.0f, 10.0f, 1.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_lightModel);
	*/
	glEnable(GL_LIGHTING);
 
	display();
}
 
int main(int argc, char *argv[])                            //主函数: 参数数量&参数值
{
	glutInit(&argc, argv);                                  //初始化glut: 接收主函数的参数
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);            //显示模式：颜色&缓冲
	glutInitWindowPosition(0, 0);                           //窗口相对屏幕位置
	glutInitWindowSize(720, 720);                           //窗口大小
	glutCreateWindow("luweiqi");							//创建窗口: 标题
 
	init();
 
	glutDisplayFunc(&display);								//显示函数
 
	glutKeyboardFunc(keyBoardFunc);
	glutMouseFunc(mouseFunc);
 
	glutMainLoop();											//主循环
 
	return 0;
}
