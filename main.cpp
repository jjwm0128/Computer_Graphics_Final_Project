#define _CRT_SECURE_NO_WARNINGS
#include <gl/glut.h>
#include <gl/glut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include<Windows.h>
#include<stdlib.h>
#include "ObjParser.h"
#include "bmpfuncs.h"

#define WINDOW_WIDTH 800
#define	WINDOW_HEIGHT 600
#define PI 3.141592

bool antialiase_on = TRUE;

double radius = 13;
double theta = 90, phi = 0;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, 1, 0 };
double Baseball_Angle = 0;

//이제 idle에서 +=하는만큼이 구속이됨
double ball_speed_y = 0;
double ball_speed_z = 0;
double ball_pos_x = 0;
double ball_pos_y = 0;
double ball_pos_z = 0;
bool ball_flying = FALSE;
//공의 타격여부 && 땅에 닿지 않았는지 여부

bool batting = FALSE;
//배트를 휘둘렀는지
char hit_time = 0;
//0이면 못침 1이면 빠르게침 2면 정타 3이면 느리게침
double bat_angle_y = 0;
double g = 0.000001;

double y_a_1 = 2;
double y_a_2 = 2.5;
double y_a_3 = 2;
int a = 0;
bool b = FALSE;
//a랑 b는 임시변수 a가 일정범위면 b를 true로 바꾸고 true면 공이나감


GLuint* baseball = new GLuint(1);
GLUquadricObj* qobj = gluNewQuadric();

// user-defined function
void init(void);
void light_default();
void add_menu();
void get_resource(const char* str);
void mouse(int, int, int, int);
void mouseWheel(int, int, int, int);
void motion(int, int);
void passiveMotion(int, int);
void keyboard(unsigned char, int, int);
void special_keyboard(int, int, int);
void draw(void);
void resize(int, int);
void idle();

void baseball_TextureMapping();
void bat_TextureMapping();
void draw_baseball();

ObjParser *baseball_bat;
GLuint texture_bat;

//bool antialiase_on = true;


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(250, 250);
	glutCreateWindow("CG_FINAL_PROJECT");

	init();

	add_menu();

	get_resource("baseball_bat.obj");

	/* Create a single window with a keyboard and display callback */
	glutMouseFunc(&mouse);
	glutMouseWheelFunc(&mouseWheel);
	glutMotionFunc(&motion);
	glutPassiveMotionFunc(&passiveMotion);
	glutKeyboardFunc(&keyboard);
	glutSpecialFunc(&special_keyboard);
	glutDisplayFunc(&draw);
	glutReshapeFunc(&resize);

	glutIdleFunc(&idle);

	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	/* Run the GLUT event loop */
	glutMainLoop();

	return EXIT_SUCCESS;
}

void light_default() {
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	/* Light0 조명 관련 설정 */
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.9f, 1.0f };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/* Light1 조명 관련 설정 */
	GLfloat ambientLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuseLight1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specularLight1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat light_position1[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	/************* spot position setting *************/
	/*GLfloat spot_direction[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);*/

	GLfloat specularMaterial[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	/************* Material  setting *************/
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);


	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glDisable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	/* DEPTH TEST ENABLE */
	glFrontFace(GL_CW);	// CW CCW바꿔보면서 front face 변경해보기!
}

/*
void setTextureMapping() {
	int imgWidth, imgHeight, channels;
	uchar* img = readImageData("bat_uv_map.bmp", &imgWidth, &imgHeight, &channels);

	int texNum = 1;
	glGenTextures(texNum, &baseball_bat);
	glBindTexture(GL_TEXTURE_2D, baseball_bat);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
}
*/
void init()
{
	printf("init func called\n");
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	// set blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// set antialiasing
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	glEnable(GL_LINE_SMOOTH);

	/* TEXTURE MAPPING SET */
	gluQuadricTexture(qobj, GL_TRUE);

	baseball_TextureMapping();
	bat_TextureMapping();

	glEnable(GL_TEXTURE_2D);
	//setTextureMapping();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //GL_REPLACE : polygon의 원래 색상은 무시하고 texture로 덮음

	/*
	light_default();
	*/
}

void special_keyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) {
		phi -= 5;
		if (phi < 0) phi = 355;
	}
	else if (key == GLUT_KEY_RIGHT) {
		phi += 5;
		if (phi >= 360) phi = 0;
	}
	else if (key == GLUT_KEY_UP) {
		if(theta > 10) theta -= 5;
	}
	else if (key == GLUT_KEY_DOWN) {
		if(theta < 170) theta += 5;
	}

	std::cout << "theta : " << theta << ", phi : " << phi << "\n";
	glutPostRedisplay();
}

/* Keyboard callback function */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		/* Exit on escape key press */
	case '\x1B':
	{
		exit(EXIT_SUCCESS);
		break;
	}
	case 'a':
	{
		if (antialiase_on) {
			antialiase_on = false;
			glDisable(GL_POLYGON_SMOOTH);
		}
		else
		{
			antialiase_on = true;
			glEnable(GL_POLYGON_SMOOTH);
		}
		break;
	}
	case 'h':
	{
		if (!batting)
		{
			batting = TRUE;
		}
	}
	}
	glutPostRedisplay();
}

void draw_axis(void)
{
	glLineWidth(1.5f);
	glBegin(GL_LINES);

	glColor4f(1.f, 0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(4.f, 0.f, 0.f);

	glColor4f(0.f, 1.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 4.f, 0.f);

	glColor4f(0.f, 0.f, 1.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 4.f);

	glEnd();
	glLineWidth(1);
}

void draw_obj(ObjParser *objParser)
{
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n+=3) {
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
}


void draw_obj_with_texture(ObjParser *objParser)
{
	glDisable(GL_BLEND);
	//glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D, texture_bat);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glTexCoord2f(objParser->textures[objParser->textureIdx[n] - 1].x,
			objParser->textures[objParser->textureIdx[n] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 1] - 1].x,
			objParser->textures[objParser->textureIdx[n + 1] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 2] - 1].x,
			objParser->textures[objParser->textureIdx[n + 2] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
	glEnable(GL_BLEND);
}

/* Display callback function */
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cam[0] = radius * sin(theta * PI / 180) * sin(phi * PI / 180);
	cam[1] = radius * cos(theta * PI / 180) + 1.5;
	cam[2] = radius * sin(theta * PI / 180) * cos(phi * PI / 180);

	if (hit_time==0)
	{
		gluLookAt(cam[0], cam[1], cam[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	}
	if (hit_time!=0)
	{
		gluLookAt(cam[0]+ball_pos_x, cam[1], cam[2]+ball_pos_z, center[0]+ball_pos_x, center[1], center[2]+ball_pos_z, up[0], up[1], up[2]);
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	draw_axis();
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	//공 구현 포수(처음 시점)가 처음 radius 즉 0,0,10 공은 0,0,0
	if (b&&!ball_flying)
	{
		glPushMatrix();
		glTranslatef(0, 4.2 - ball_speed_y, ball_speed_z);
		glRotatef(Baseball_Angle, 1, 0, 0);
		draw_baseball();
		//glutSolidSphere(0.1, 50, 50);
		glPopMatrix();
	}
	if (b && ball_flying)
	{
		glPushMatrix();
		glTranslatef(ball_pos_x, ball_pos_y, ball_pos_z);
		glRotatef(Baseball_Angle, 1, 0, 0);
		draw_baseball();
		//glutSolidSphere(0.1, 50, 50);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(-1.2, 0.8, 8);
	if (batting)
	{
		glRotatef(10 + bat_angle_y, 0, 1, 0);
		glRotatef(-20, 1, 0, 0);
	}
	else
	{
		glRotatef(-90, 1, 0, 0);
	}
	glColor3f(1, 1, 1);
	draw_obj_with_texture(baseball_bat);
	//draw_obj(baseball_bat);
	//gluCylinder(qobj, 0.12, 0.12, 1.6, 16, 4);
	glPopMatrix();
	/*
	glDisable(GL_LIGHT1);
	GLfloat light_position[] = { cam[0],cam[1],cam[2],1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	*/


	glutSwapBuffers();
	glFlush();
}

void mouse(int button, int state, int x, int y)
{
	if (state) {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	else {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		if (radius > 1) radius -= 0.5;
	}
	else {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		if (radius < 100) radius += 0.5;
	}
	glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void motion(int x, int y)
{
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void main_menu(int option)
{
	if (option == 99) exit(0);
	else if (option == 1) {
		radius = 10;
		theta = 45; phi = 45;
	}
	glutPostRedisplay();
}

void sub_menu(int option)
{
	printf("Submenu %d has been selected\n", option);
}

void add_menu()
{
	int mainmenu1 = glutCreateMenu(&main_menu);
	glutAddMenuEntry("Init", 1);
	glutAddMenuEntry("Quit", 99);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void idle()
{
	if (b && ! ball_flying)
	{
		ball_speed_y += 0.00075;
		ball_speed_z += 0.002;
	}
	if (batting && bat_angle_y < 150)
	{
		bat_angle_y += 0.14;
		if (bat_angle_y > 140)
		{
			batting = FALSE;
		}
	}
	if (bat_angle_y < 90.03 && bat_angle_y>90.01 || ball_flying)
		//배트가 중앙에 왔을때
	{
		if (!ball_flying)
		{
			ball_pos_x = 0;
			ball_pos_y = 4.2 - ball_speed_y;
			ball_pos_z = ball_speed_z;
			printf("%f\n", ball_pos_z);
		}
		if (ball_speed_z < 7.77 && ball_speed_z>7.37)
		{
			hit_time = 1;
		}
		if (ball_speed_z < 8.07 && ball_speed_z>7.77)
		{
			hit_time = 2;
		}
		if (ball_speed_z < 8.47 && ball_speed_z>8.07)
		{
			hit_time = 3;
		}
		if(hit_time==1)
		{
			ball_flying = TRUE;
			g = g - 0.000001;
			y_a_1 = y_a_1 + g;
			ball_pos_x -= 0.01;
			ball_pos_y = 2 + y_a_1;
			ball_pos_z -= 0.01;
			if (ball_pos_y < 0.01)
			{
				if (a % 10000 < 5000)
				{
					int temp = 5000 - a;
					a += temp;
				}
				ball_flying = FALSE;
				hit_time = 0;
			}
		}
		if (hit_time==2) 
		{
			ball_flying = TRUE;
			g = g - 0.000001;
			y_a_2 = y_a_2 + g;
			ball_pos_x = 0;
			ball_pos_y = 2 + y_a_2;
			ball_pos_z -= 0.01;
			if (ball_pos_y < 0.01)
			{
				if (a % 10000 < 5000)
				{
					int temp = 5000 - a;
					a += temp;
				}
				ball_flying = FALSE;
				hit_time = 0;
			}
		}
		if (hit_time==3)  
		{
			ball_flying = TRUE;
			g = g - 0.000001;
			y_a_3 = y_a_3 + g;
			ball_pos_x += 0.01;
			ball_pos_y = 2 + y_a_3;
			ball_pos_z -= 0.01;
			if (ball_pos_y < 0.01)
			{
				if (a % 10000 < 5000)
				{
					int temp = 5000 - a;
					a += temp;
				}
				ball_flying = FALSE;
				hit_time = 0;
			}
		}
	}
	if (bat_angle_y > 140)
	{
		bat_angle_y = 0;
	}
	if (!ball_flying)
	{
		a++;
	}
	if (a % 10000 < 5000 && a>10000)
	{
		b = TRUE;
	}
	if(a%10000>=5000)
	{
		b = FALSE;
		g = 0.000001;
		y_a_1 = 2;
		y_a_2 = 2.5;
		y_a_3 = 2;
		ball_speed_y = 0;
		ball_speed_z = 0;
		//printf("%f\n", y_a_2);
	}
	Baseball_Angle += 0.35;
	glutPostRedisplay();
}

void resize(int width, int height)
{
	printf("resize func called\n");
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, 500, 0, 500);
	gluPerspective(60, (double)width / (double)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void baseball_TextureMapping()
{
	glGenTextures(1, baseball);
	int imgWidth, imgHeight, channels;
	glBindTexture(GL_TEXTURE_2D, baseball[0]);
	uchar* img = readImageData("baseball.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
void bat_TextureMapping()
{
	int imgWidth, imgHeight, channels;
	glBindTexture(GL_TEXTURE_2D, texture_bat);
	uchar* img = readImageData("bat_uv_map.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void draw_baseball()
{
	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, baseball[0]);
	gluSphere(qobj, 0.1, 50, 50);
}

void get_resource(const char* str)
{
	baseball_bat = new ObjParser(str);
}




