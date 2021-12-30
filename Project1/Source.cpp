#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream> 
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include "glm.c"
//#include "glm.h"
#include "materials.h"
#include "sgi.h"

#define WIDTH 1280
#define HEIGHT 720
#define MAX_CHAR 128

#define GAME_STATE_SHOOTDART -1
#define READY_STATE 0
#define ROLLING_STATE 1
#define MOVING_STATE 2
#define EVENT_STATE 4
#define END_STATE 5

#define GAME_TYPE_POKER 0

#define GAME_TYPE_SHOOTDART 1

#define ANGLEMODE 0
#define STRENGTHMODE 1
#define SHOOTMODE 2
#define SHOWRESULT 3
#define XMOVEMODE 4
#define OUTPUTOUTCOME 5

#define SHOOT_GENERALMODE 0
#define SHOOT_CHEATMODE 1

#define CORNER_START 0
#define CORNER_GOPRISON 1
#define CORNER_FREEPARK 2
#define CORNER_PRISON 3

#define CARD_0 0
#define CARD_1 1

using namespace std;
// Game Stuff
static float save_px = 50.0;
int GamePoint;
// Gravity
static float gra = 9.8;
static float gra_speed = 0.0;
static bool ON_JUMPING = FALSE;
// Screen Size
static GLfloat ScreenWidth = WIDTH;
static GLfloat ScreenHeight = HEIGHT;
static int CC = 0;
// Camera
const static float PI = 3.14159;
const static float default_camPosx = 0.0;
const static float default_camPosy = 10.0;
const static float default_camPosz = 30.0; //30
static float camPosx = 0.0;
static float camPosy = 10.0;
static float camPosz = 30.0; //30
static float step = 0.5;

// Light
static GLfloat light_position[] = { 0.0, 50.0, -20.0 };
static GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat ambient[] = { 0.5, 0.5, 0.5, 0.5 };
static GLfloat diffuse[] = { 0.5, 0.5, 0.5, 0.3 };
void setMaterialv(const GLfloat* params);

// Objects
static GLfloat rock_material[] = { 0.3, 0.3, 0.3 };
static GLfloat default_material[] = { 0.2, 0.2, 0.2 };
static GLfloat density_material[] = { 1.0, 192.0 / 255.0, 203.0 / 255.0 };
static GLfloat chance_material[] = { 1.0, 97.0 / 255.0, 0.0 };
static GLfloat corner_material[] = { 176.0 / 255.0, 224.0 / 255.0, 230.0 / 25.0 };
static GLfloat game_material[] = { 1.0, 1.0, 0.0 };
static GLfloat black_material[] = { 0.0, 0.0, 0.0 , 1.0 };
static GLfloat player_material[3][3] = { {0.8, 0.2, 0.2}, {0.2, 0.8, 0.2}, {0.2, 0.2, 0.8} };
static GLfloat low_shininess[] = { 1.0 };
static GLfloat mat_dice[] = { 0.5, 0.5 ,0.5 };
static GLfloat mat_diffuse_dice[] = { 0.5, 0.5 ,0.50 };

class GameShootDart
{
public:
	float person_x = 50.0, person_y = 10.0, person_z = 10.0;
	float cam_x = person_x - 40.0, cam_y = person_y, cam_z = person_z - 10.0;
	//target
	GLuint target_init;
	GLfloat target_x = person_x;
	GLfloat target_y = person_y;
	GLfloat target_z = person_z - 30.0;
	//shoot
	GLint shoot_mode = XMOVEMODE;
	GLfloat shoot_angle = 0.0;
	GLfloat shoot_speed = 5.0;
	GLfloat gravity = 0.1;
	GLfloat fly_time = 0.0;
	GLfloat fly_height = 0.0;
	GLfloat fly_way = 0.0;
	//arrow
	GLuint arrow_list;
	//animation
	int player_mode = SHOOT_GENERALMODE;
	int angle_animate = 0;
	int strength_animate = 0;
	int xmove_animate = 0;
	//outcome
	int point = 0;
	void init()
	{
		//target
		target_init = glGenLists(1);
		glNewList(target_init, GL_COMPILE);
		target();
		glEndList();
		//arrow
		arrow_list = glGenLists(1);
		glNewList(arrow_list, GL_COMPILE);
		arrow_obj();
		glEndList();
		//dicepoint
	}
	void GameMouseContorl(int button, int state, int x, int y);
	void GameKeyboardControl(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 'w':
		case 'W':
			if (player_mode == SHOOT_CHEATMODE) {
				person_z -= 0.1;
				glutPostRedisplay();
			}
			break;
		case 's':
		case 'S':
			if (player_mode == SHOOT_CHEATMODE) {
				person_z += 0.1;
				glutPostRedisplay();
			}
			break;
		case 'a':
		case 'A':
			if (player_mode == SHOOT_CHEATMODE) {
				person_x -= 0.1;
				glutPostRedisplay();
			}
			break;
		case 'd':
		case 'D':
			if (player_mode == SHOOT_CHEATMODE) {
				person_x += 0.1;
				glutPostRedisplay();
			}
			break;
		case '+':
			if (player_mode == SHOOT_CHEATMODE && shoot_mode == STRENGTHMODE) {
				shoot_speed += 0.1;
			}
			else if (player_mode == SHOOT_CHEATMODE && shoot_mode == ANGLEMODE) {
				shoot_angle += 1.;
			}
			glutPostRedisplay();
			break;
		case '-':
			if (player_mode == SHOOT_CHEATMODE && shoot_mode == STRENGTHMODE) {
				shoot_speed -= 0.1;
			}
			else if (player_mode == SHOOT_CHEATMODE && shoot_mode == ANGLEMODE) {
				shoot_angle -= 1.;
			}
			glutPostRedisplay();
			break;
		case 'm':
		case 'M':
			if (player_mode == SHOOT_GENERALMODE) {
				player_mode = SHOOT_CHEATMODE;
			}
			else {
				player_mode = SHOOT_GENERALMODE;
			}
			break;
		default:
			break;
		}
	}
	void idle() {
		if (player_mode == SHOOT_GENERALMODE) {
			if (shoot_mode == XMOVEMODE) {
				if (xmove_animate == 0) {
					person_x += 0.6;
					if (person_x > 10.0 + save_px) {
						xmove_animate = 1;
					}
				}
				if (xmove_animate == 1) {
					person_x -= 0.6;
					if (person_x < -10.0 + save_px) {
						xmove_animate = 0;
					}
				}
			}
			if (shoot_mode == ANGLEMODE) {
				if (angle_animate == 0) {
					shoot_angle += 0.6;
					if (shoot_angle > 85.0) {
						angle_animate = 1;
					}
				}
				if (angle_animate == 1) {
					shoot_angle -= 0.6;
					if (shoot_angle < 0.0) {
						angle_animate = 0;
					}
				}
				//printf("%f \n", (shoot_angle));
			}
			if (shoot_mode == STRENGTHMODE) {
				if (strength_animate == 0) {
					shoot_speed -= 0.05;
					if (shoot_speed <= 0.0) {
						strength_animate = 1;
					}
				}
				if (strength_animate == 1) {
					shoot_speed += 0.05;
					if (shoot_speed >= 5.0) {
						strength_animate = 0;
					}
				}
				//printf("%f \n", (shoot_speed));
			}
		}

		if (shoot_mode == SHOOTMODE) {
			float y_speed = sin(PI / 180.0 * shoot_angle) * shoot_speed;
			float z_speed = cos(PI / 180.0 * shoot_angle) * shoot_speed;
			fly_height = y_speed * fly_time - gravity * fly_time * fly_time / 2;
			fly_way = z_speed * fly_time;
			fly_time += 0.2;
			if (person_y + fly_height <= 0. || person_z - fly_way <= target_z) {
				count_point();
				shoot_mode = SHOWRESULT;
			}
		}
		glutPostRedisplay();
	}
	void arrow_obj() {
		GLMmodel* pmodel = NULL;
		if (!pmodel) {
			char filename[] = "objects/11750_throwing_dart_v1_L3.obj";
			pmodel = glmReadOBJ(filename);
			if (!pmodel) exit(0);
			glmUnitize(pmodel);
			glmFacetNormals(pmodel);
			glmVertexNormals(pmodel, 90.0);
		}
		setMaterialv(rock_material);
		glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
	}
	void target() {
		glBegin(GL_POLYGON);
		int n = 720;
		glColor3f(1.0, 1.f, 1.f);
		for (int i = 0; i < n; i++) {
			glVertex3f(10 * cos(2 * PI / n * i), 10 * sin(2 * PI / n * i), 0.f);
		}
		glEnd();
		glBegin(GL_POLYGON);
		glColor3f(0.0, 0.f, 0.f);
		for (int i = 0; i < n; i++) {
			glVertex3f(8 * cos(2 * PI / n * i), 8 * sin(2 * PI / n * i), 0.01f);
		}
		glEnd();
		glBegin(GL_POLYGON);
		glColor3f(0.0, 0.4, 0.7);
		for (int i = 0; i < n; i++) {
			glVertex3f(6 * cos(2 * PI / n * i), 6 * sin(2 * PI / n * i), 0.02f);
		}
		glEnd();
		glBegin(GL_POLYGON);
		glColor3f(0.9, 0.f, 0.f);
		for (int i = 0; i < n; i++) {
			glVertex3f(4 * cos(2 * PI / n * i), 4 * sin(2 * PI / n * i), 0.03f);
		}
		glEnd();
		glBegin(GL_POLYGON);
		glColor3f(0.8, 0.8, 0.f);
		for (int i = 0; i < n; i++) {
			glVertex3f(2 * cos(2 * PI / n * i), 2 * sin(2 * PI / n * i), 0.04f);
		}
		glEnd();
		for (int j = 1; j <= 10; j++) {
			glBegin(GL_LINE_STRIP);
			glColor3f(0.f, 0.f, 0.f);
			for (int i = 0; i < n; i++) {
				glVertex3f(j * cos(2 * PI / n * i), j * sin(2 * PI / n * i), 0.05f);
			}
			glEnd();
		}
	}
	void throw_line() {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x00FF);
		glColor3f(1.0, 1.0, 1.0);

		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, sin(PI / 180.0 * shoot_angle), -cos(PI / 180.0 * shoot_angle));
		glEnd();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glDisable(GL_LINE_STIPPLE);
	}
	void count_point() {
		//printf("target pos: %f %f %f\n", person_x, person_y + fly_height, person_z - fly_way);
		if (person_x >= -1.0 + target_x && person_x <= 1.0 + target_x && person_y + fly_height >= -1.0 + target_y && person_y + fly_height <= 1.0 + target_y) {
			point = 10;
		}
		else if (person_x >= -2.0 + target_x && person_x <= 2.0 + target_x && person_y + fly_height >= -2.0 + target_y && person_y + fly_height <= 2.0 + target_y) {
			point = 9;
		}
		else if (person_x >= -3.0 + target_x && person_x <= 3.0 + target_x && person_y + fly_height >= -3.0 + target_y && person_y + fly_height <= 3.0 + target_y) {
			point = 8;
		}
		else if (person_x >= -4.0 + target_x && person_x <= 4.0 + target_x && person_y + fly_height >= -4.0 + target_y && person_y + fly_height <= 4.0 + target_y) {
			point = 7;
		}
		else if (person_x >= -5.0 + target_x && person_x <= 5.0 + target_x && person_y + fly_height >= -5.0 + target_y && person_y + fly_height <= 5.0 + target_y) {
			point = 6;
		}
		else if (person_x >= -6.0 + target_x && person_x <= 6.0 + target_x && person_y + fly_height >= -6.0 + target_y && person_y + fly_height <= 6.0 + target_y) {
			point = 5;
		}
		else if (person_x >= -7.0 + target_x && person_x <= 7.0 + target_x && person_y + fly_height >= -7.0 + target_y && person_y + fly_height <= 7.0 + target_y) {
			point = 4;
		}
		else if (person_x >= -8.0 + target_x && person_x <= 8.0 + target_x && person_y + fly_height >= -8.0 + target_y && person_y + fly_height <= 8.0 + target_y) {
			point = 3;
		}
		else if (person_x >= -9.0 + target_x && person_x <= 9.0 + target_x && person_y + fly_height >= -9.0 + target_y && person_y + fly_height <= 9.0 + target_y) {
			point = 2;
		}
		else if (person_x >= -10.0 + target_x && person_x <= 10.0 + target_x && person_y + fly_height >= -10.0 + target_y && person_y + fly_height <= 10.0 + target_y) {
			point = 1;
		}
		//printf("point: %d\n", point);
	}
	void strength_render(float nyhealth_rm) {
		if (shoot_mode == SHOWRESULT || shoot_mode == OUTPUTOUTCOME)
			return;
		float remain_rate = nyhealth_rm / 5.0 * 1.0 - 0.5;
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);

		glTranslatef(person_x + 0.6, person_y + 0.7, person_z + 0.0);
		glScalef(0.5, 0.5, 0.5);
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		glVertex3f(-0.5, -0.1, -0.09);
		glVertex3f(remain_rate, -0.1, -0.09);
		glVertex3f(remain_rate, 0.1, -0.09);
		glVertex3f(-0.5, 0.1, -0.09);
		glEnd();
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		glVertex3f(-0.5, -0.1, -0.1);
		glVertex3f(0.5, -0.1, -0.1);
		glVertex3f(0.5, 0.1, -0.1);
		glVertex3f(-0.5, 0.1, -0.1);
		glEnd();
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}
	void shoot_reset() {
		shoot_mode = XMOVEMODE;
		angle_animate = 0;
		strength_animate = 0;
		xmove_animate = 0;
		person_x = 0.0;
		person_y = 10.0;
		person_z = 10.0;
		fly_height = 0.0;
		fly_way = 0.0;
		fly_time = 0.0;
		shoot_speed = 5.0;
		point = 0;
	}
	void cameraSet()
	{
		if (shoot_mode == ANGLEMODE || shoot_mode == STRENGTHMODE)
			gluLookAt(person_x - 0.5, person_y, person_z + 1.0, person_x, person_y, person_z - 100.0, 0.0, 1.0, 0.0);
		else if (shoot_mode == XMOVEMODE)
			gluLookAt(person_x, person_y, person_z + 1.0, person_x, person_y, person_z - 100.0, 0.0, 1.0, 0.0);
		else if (shoot_mode == OUTPUTOUTCOME)
			gluLookAt(cam_x, cam_y, cam_z, cam_x, cam_y, cam_z - 10.0, 0.0, 1.0, 0.0);
		else
			gluLookAt(cam_x, cam_y, cam_z, cam_x + 10.0, cam_y, cam_z, 0.0, 1.0, 0.0);
	}
	void render()
	{
		if (shoot_mode == OUTPUTOUTCOME)
		{
			camPosx = cam_x;
			camPosy = cam_y;
			camPosz = cam_z;
			glColor3fv(game_material);
			OutcomeInterface();
			return;
		}
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glPushMatrix();
		glTranslatef(target_x, target_y, target_z);
		glCallList(target_init);
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glPushMatrix();
		glTranslatef(person_x, person_y + fly_height, person_z - fly_way);
		throw_line();
		glCallList(arrow_list);
		glPopMatrix();
		glPushMatrix();
		if (shoot_mode != ANGLEMODE) {
			//glPushMatrix();
			strength_render(shoot_speed);
			//glPopMatrix();
		}
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	void OutcomeInterface();

};
class Dice
{
public:
	int num = 0;
	GLint dice_rotate = 1;
	GLint dice_rotate_mode = 0;
	GLfloat dice_rotate_angle0 = 0.0;
	GLfloat dice_rotate_angle1 = 0.0;
	GLfloat dice_rotate_angle2 = 0.0;
	GLint dice_scala_open = 0;
	GLint dice_scala_big = 0;
	GLfloat dice_scalaf = 1.0;
	GLuint dice_init;
	bool can_roll = 0;
	int count;
	Dice();
	int dice_point();
	void dice_point_1() {
		glPushMatrix();
		glTranslatef(0.f, 0.f, 2.f);
		glColor3f(1.0, 0.0, 0.0);
		draw_circle(0.5);
		glPopMatrix();
	}
	void dice_point_2() {
		glPushMatrix();
		glColor3f(0.0, 0.0, 0.0);
		glRotatef(90.0, -1.0, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPopMatrix();
	}
	void dice_point_3() {
		glPushMatrix();
		glColor3f(0.0, 0.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslatef(1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.f, 0.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPopMatrix();
	}
	void dice_point_4() {
		glPushMatrix();
		glColor3f(1.0, 0.0, 0.0);
		glRotatef(90.0, 0.0, -1.0, 0.0);
		glPushMatrix();
		glTranslatef(1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPopMatrix();
	}
	void dice_point_5() {
		glPushMatrix();
		glColor3f(0.0, 0.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.f, 0.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPopMatrix();
	}
	void dice_point_6() {
		glPushMatrix();
		glColor3f(0.0, 0.0, 0.0);
		glRotatef(180.0, 0.0, -1.0, 0.0);
		glPushMatrix();
		glTranslatef(1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(1.f, 1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, -1.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(1.f, 0.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1.f, 0.f, 2.f);
		draw_circle(0.5);
		glPopMatrix();
		glPopMatrix();
	}
	void dice_init_point() {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glPushMatrix();
		dice_point_1();
		dice_point_2();
		dice_point_3();
		dice_point_4();
		dice_point_5();
		dice_point_6();
		glPopMatrix();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	void listRender();
	void draw_circle(float R) {
		glBegin(GL_POLYGON);
		int n = 720;
		for (int i = 0; i < n; i++)
			glVertex3f(R * cos(2 * PI / n * i), R * sin(2 * PI / n * i), 0.01f);//計算坐標
		glEnd();
		glFlush();//強制刷新緩衝，保證命令被執行
	}
	void dice_jump() {
		glScalef(dice_scalaf, dice_scalaf, dice_scalaf);
		switch (num) {
		case 1:
			break;
		case 2:
			glRotatef(90, 1.0, 0.0, 0.0);

			break;
		case 3:
			glRotatef(90, 0.0, -1.0, 0.0);
			break;
		case 4:
			glRotatef(90, 0.0, 1.0, 0.0);
			break;
		case 5:
			glRotatef(90, -1.0, 0.0, 0.0);
			break;
		case 6:
			glRotatef(180, -1.0, 0.0, 0.0);
			break;
		default:
			break;
		}
		glCallList(dice_init);
	}
	void render();
	void idle();
	void init() {
		dice_init = glGenLists(1);
		glNewList(dice_init, GL_COMPILE);
		render();
		glEndList();
	}
};
class SObject
{
public:
	struct position
	{
		float x;
		float y;
		float z;
	};
	GLMmodel* pmodel = NULL;
	position pos;
	GLfloat size;
	GLuint list;
	bool drawFlag;
	GLfloat user_defined_matrial[3];
	SObject() {};
	SObject(const char* filename, float x, float y, float z, GLfloat size);
	SObject(const char* filename, float x, float y, float z, GLfloat size, float m1, float m2, float m3);
	SObject(const char* filename, float x, float y, float z, GLfloat size, GLfloat* prama);
	void Draw();
	void init();
};
class Block
{
public:
	const static int BLOCK_SIZE = 10;
	int bid;
	string BlockType;
	float x;
	float z;
	float roate;
	Block* next;

	int MouseControl = -1;

	int setup;
	// land:price
	float* color = NULL;
	int owner = -1;
	int bnum = 0;
	// game:game type
	// density or chance: 0 or 1
	// corner : 0[start] 1[go_prison] 2[free park] 3[prison(nothing)]


	Block(string type, int bid, float x, float z, float roate, int setup);
	void render();
	void BlockEvent();
	void Interface();
	void LandInterface();
	void GameInterface();
	void CornerInterface();
	void CardInterface();
	void UserEventMouseControl(int button, int state, int x, int y);
};
class Player
{
public:
	int pid;
	long money;
	float x, y, z;
	float offset_x;
	float offset_z;
	int RoundStall = 0;
	// 第幾個格子
	Block* blockId;
	SObject model;

	// Animation
	bool MOVING_FLAG = FALSE;
	bool ON_MOVING = FALSE;
	int moving_setp = 0;

	Player(int pid, float offset_x, float offset_z, Block* start);
	void render();
	void idle();
	void gravity();
	Block* getNext();

};
class MonopolyGame
{
public:
	const static int MAP_SIZE = 5;
	const static int PlayerNum = 2;
	const static int blockNum = (MAP_SIZE - 2) * 4 + 4;

	int state;

	bool HUD_Display_FLAG = TRUE;
	int now_player;
	Player* players;
	Block* blocks;

	SObject* decor;

	MonopolyGame();
	void init();
	void Render();
	void HUD(int PlayerID);
	Player* getNowPlayer();
	Block* getNext();
	void nextPlayer();
	void toREADY_STATE();
	void toROLLING_STATE();
	void toMOVING_STATE();
	void toEVENT_STATE();
	void toEND_STATE();
	void UserEventMouseControl(int button, int state, int x, int y);
	void detectPlayer();
	void GameOver();
	void toGAME_STATE();
};



// Basic Test
void DrawCircle(float cx, float cy, float r, int num_segments);
void drawAxis();
void drawAxis(float locX, float LocY, float LocZ);
void drawAxis(float*);
// Draw Texts
void selectFont(int size, int charset, const char* face);
void drawCNString(const char* str);
void drawString(const char* str);

Dice dice = Dice();
GameShootDart shootDart = GameShootDart();
SObject test = SObject("objects/Player1.obj", 0.0, 0.0, 0.0, 4.0, 0.8, 0.2, 0.2);
SObject buildings[] = {
	SObject("objects/building01.obj", 0.0, 0.0, 0.0, 1.0, player_material[0]),
	SObject("objects/building01.obj", 0.0, 0.0, 0.0, 1.0, player_material[1]),
};
SObject decors[] = {
	SObject("objects/SM_Rock_01.obj", 0.0, 1.0, 0.0, 2.0, rock_material),
	SObject("objects/SM_Flat_ROck_02.obj", 3.0, 0.7, 7.0, 2.0, rock_material),
	SObject("objects/SM_Fern_02.obj", 4.0, 2.0, 0.0, 4.0, 107.0 / 255.0, 142.0 / 255.0,35.0 / 255.0),
	SObject("objects/SM_Big_Rock_01.obj", -13.0, 1.0, 0.0, 2.0, rock_material),
	SObject("objects/SM_Flat_ROck_02.obj", -3.0, 0.7, 7.0, 2.0, rock_material),
	SObject("objects/SM_Fern_02.obj", 6.0, 2.5, -2.0, 5.0, 107.0 / 255.0, 142.0 / 255.0,35.0 / 255.0),
};

static string CARD_Discriptor[2][10][2] = {
	{
		{"濫用身分證幫同學付車票錢，違反個資法，去坐牢"		, "坐牢"},
		{"成圖作業有壓縮，獲得總統表揚，獎勵1000元"			, "1000"},
		{"路邊違規停車，罰600元"							, "-600"},
		{"UNIX作業有壓縮，獲得教授表揚，獎勵400元"			, "400"},
		{"我有一隻小毛驢，但我從來都沒在騎，休息1回合"		, "0"},
		{"成圖期中考有壓縮，獲得[圖靈獎]，獎勵1500元"		, "1500"},
		{"沒去上成圖，點名被抓到，罰800元"					, "-800"},
		{"成圖期末Project沒做完，受到全世界唾棄，罰1000元"	, "-1000"},
		{"成圖上課都在睡覺，當不起老師的小寶貝，罰500元"	, "-500"},
		{"扛不住成圖作業，選擇退課，罰400元"				, "-400"},
	},
	{
		{"成圖作業弄不出來，抄襲被抓到，去坐牢"												, "坐牢"},
		{"成圖作業弄不出來，沒有繳交，罰600元"												, "-600"},
		{"成圖寫完忘記交，跑去同學房間哈拉，罰800元"										, "-800"},
		{"UNIX被當還來上成圖，休息1回合"													, "0"},
		{"成圖上課都在玩手機，罰650元"														, "-650"},
		{"成圖作業跟UNIX作業都有壓縮，獲得[諾貝爾壓縮獎]，獎勵2500元"						, "2500"},
		{"修完成圖，功德圓滿，獲得如來佛封法號[成圖勝佛]，獎勵2000元"						, "2000"},
		{"對發票中獎，獎勵800元"															,  "800"},
		{"修了成圖，歷經九九八十一難後修成正果，獲得如來佛封法號[成圖如來宗]，獎勵2000元"	, "2000"},
		{"修了得過[未來科技獎]的黃春融教授的課，名貫千家，譽滿萬戶，獎勵1500元"				, "1500"},
	}
};

MonopolyGame monopoly = MonopolyGame();

void init(void)
{
	/* selcet clearing color*/
	glClearColor(0.0, 0.0, 0.0, 0.0); //channel: RGBA
	glShadeModel(GL_SMOOTH);

	{

		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 400.0);
		//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 15.0);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	}

	for (int i = 0; i < sizeof(buildings) / sizeof(SObject); i++)
		buildings[i].init();

	monopoly.init();
	dice.init();
	shootDart.init();

	// Anti-Alias
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	// Depth
	glEnable(GL_DEPTH_TEST);
	// load object

}

void mouse(int button, int state, int x, int y)
{
	if (monopoly.state == READY_STATE)
	{
		switch (button)
		{
			// Wheel reports as button 3(scroll up) and button 4(scroll down)
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
			{
				monopoly.HUD_Display_FLAG = !monopoly.HUD_Display_FLAG;
				glutPostRedisplay();
			}
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN)
				for (int i = 0; i < monopoly.PlayerNum; i++)
					if (monopoly.now_player != i)
						monopoly.players[i].RoundStall++;
			glutPostRedisplay();
			break;
		case 3:
			camPosy += step;
			glutPostRedisplay();
			break;
		case 4:
			camPosy -= step;
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}
	else if (monopoly.state == GAME_STATE_SHOOTDART)
	{
		shootDart.GameMouseContorl(button, state, x, y);
	}
	else if (monopoly.state == EVENT_STATE)
	{
		monopoly.UserEventMouseControl(button, state, x, y);
	}
	else if (monopoly.state == END_STATE)
	{
		switch (button)
		{

		case 3:
			camPosy += step;
			glutPostRedisplay();
			break;
		case 4:
			camPosy -= step;
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}

}

void keyboard(unsigned char key, int x, int y)
{
	if (monopoly.state == READY_STATE)
	{
		switch (key)
		{
		case 'w':
			camPosz -= step;
			glutPostRedisplay();
			break;
		case 's':
			camPosz += step;
			glutPostRedisplay();
			break;
		case 'a':
			camPosx -= step;
			glutPostRedisplay();
			break;
		case 'd':
			camPosx += step;
			glutPostRedisplay();
			break;
		case ' ':
			monopoly.toROLLING_STATE();
			glutPostRedisplay();
			break;
		case '+':
			monopoly.getNowPlayer()->money += 1000;
			glutPostRedisplay();
			break;
		case '-':
			monopoly.getNowPlayer()->money -= 1000;
			glutPostRedisplay();
			break;

		}
	}
	else if (monopoly.state == ROLLING_STATE)
	{
		switch (key)
		{
		case 27:
			monopoly.state = READY_STATE;
			glutPostRedisplay();
			break;
		case ' ':
			if (!dice.can_roll)
				break;
			if (dice.dice_rotate == 1)
			{
				dice.dice_rotate = 0;
				dice.dice_scala_open = 1;
				dice.dice_scala_big = 1;
				dice.num = dice.dice_point();
			}
			else {
				dice.dice_rotate = 1;
			}
			dice.can_roll = FALSE;
			glutPostRedisplay();
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (!dice.can_roll)
				break;
			dice.num = key - '0';
			dice.count = dice.num;
			dice.dice_rotate = FALSE;
			dice.can_roll = FALSE;
			monopoly.getNowPlayer()->MOVING_FLAG = TRUE;
			monopoly.state = MOVING_STATE;
			glutPostRedisplay();
			break;
		}
	}
	else if (monopoly.state == MOVING_STATE)
	{
		switch (key)
		{
		case 27:
			monopoly.state = ROLLING_STATE;
			glutPostRedisplay();
			break;
		case ' ':
			glutPostRedisplay();
			break;
		}
	}
	else if (monopoly.state == GAME_STATE_SHOOTDART)
	{
		shootDart.GameKeyboardControl(key, x, y);
	}
	else if (monopoly.state == EVENT_STATE)
	{
		switch (key)
		{
		case 27:
			monopoly.state = ROLLING_STATE;
			glutPostRedisplay();
			break;
		case 'j':
			monopoly.toREADY_STATE();
			glutPostRedisplay();
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (monopoly.getNowPlayer()->blockId->BlockType == "card")
				monopoly.getNowPlayer()->blockId->owner = key - '0';
			glutPostRedisplay();
			break;
		}
	}
	else if (monopoly.state == END_STATE)
	{
		switch (key)
		{
		case 'w':
			camPosz -= step;
			glutPostRedisplay();
			break;
		case 's':
			camPosz += step;
			glutPostRedisplay();
			break;
		case 'a':
			camPosx -= step;
			glutPostRedisplay();
			break;
		case 'd':
			camPosx += step;
			glutPostRedisplay();
			break;
		case 27:
			exit(0);
			break;
		}
	}

}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	if (monopoly.state == READY_STATE || monopoly.state == END_STATE || monopoly.state == EVENT_STATE)
		gluLookAt(camPosx, camPosy, camPosz, camPosx, camPosy, camPosz - 5.0, 0.0, 1.0, 0.0);
	else if (monopoly.state == ROLLING_STATE || monopoly.state == MOVING_STATE)
	{
		gluLookAt(monopoly.players[monopoly.now_player].x + monopoly.players[monopoly.now_player].offset_x, 6.0, monopoly.players[monopoly.now_player].z + monopoly.players[monopoly.now_player].offset_z + 8.0,
			monopoly.players[monopoly.now_player].x + monopoly.players[monopoly.now_player].offset_x, 6.0, monopoly.players[monopoly.now_player].z + monopoly.players[monopoly.now_player].offset_z - 5.0,
			0.0, 1.0, 0.0);
	}
	else if (monopoly.state == GAME_STATE_SHOOTDART)
	{
		shootDart.cameraSet();
	}

	if (monopoly.state == GAME_STATE_SHOOTDART)
		shootDart.render();
	else
	{
		dice.listRender();
		//test.Draw();
		monopoly.Render();
		// render HUD
		if (monopoly.HUD_Display_FLAG)
			monopoly.HUD(0);
	}


	glutSwapBuffers();
	glFlush();

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100.0, (GLfloat)w / (GLfloat)h, 0.1, 220.0);
	ScreenHeight = h; ScreenWidth = w;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camPosx, camPosy, camPosz, camPosx, camPosy, camPosz - 100.0, 0.0, 1.0, 0.0);
}

void idle()
{
	dice.idle();
	monopoly.getNowPlayer()->idle();
	monopoly.getNowPlayer()->gravity();
	monopoly.detectPlayer();
	if (monopoly.state == GAME_STATE_SHOOTDART)
		shootDart.idle();
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{

	// 	set seed
	srand(time(NULL));
	// Initialize GLUT
	glutInit(&argc, argv);
	// Set up some memory buffers for our display
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// Set the window size
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	// Create the window with the title "Hello,GL"
	glutCreateWindow("成圖大富翁");

	init();
	glewInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}


void drawAxis() {
	//x-axis
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(600.0, 0.0, 0.0);
	glEnd();
	//y-axis
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 600.0, 0.0);
	glEnd();
	//z-axis
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 600.0);
	glEnd();
}
void DrawCircle(float cx, float cy, float r, int num_segments)
{
	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0 * 3.1415926 * float(ii) / float(num_segments);//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex3f(x + cx, y + cy, 0.0);//output vertex

	}
	glEnd();
}
void setMaterialv(const GLfloat* params)
{
	glMaterialfv(GL_FRONT, GL_SPECULAR, params);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, params);
}
void drawAxis(float LocX, float LocY, float LocZ)
{
	glPushMatrix();
	glTranslatef(LocX, LocY, LocZ);
	drawAxis();
	glPopMatrix();
}
void drawAxis(float* Locs)
{
	drawAxis(Locs[0], Locs[1], Locs[2]);
}

void selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}
void drawString(const char* str)
{
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall)
	{
		isFirstCall = 0;
		lists = glGenLists(MAX_CHAR);
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}

	for (; *str != '\0'; ++str)
		glCallList(lists + *str);

}
void drawCNString(const char* str)
{
	int len, i;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	len = 0;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}

	wstring = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	for (i = 0; i < len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}

	free(wstring);
	glDeleteLists(list, 1);
}

SObject::SObject(const char* filename, float x, float y, float z, GLfloat size)
{
	char name[50];
	strcpy(name, filename);
	this->pmodel = glmReadOBJ(name);
	if (!pmodel)
		exit(1);
	glmUnitize(pmodel);
	glmFacetNormals(pmodel);
	glmVertexNormals(pmodel, 90.0);

	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
	this->size = size;
	this->drawFlag = FALSE;
}
void SObject::init()
{
	if (!drawFlag)
	{
		this->list = glGenLists(1);
		glNewList(this->list, GL_COMPILE);
		glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
		glEndList();
		return;
	}
	this->list = glGenLists(1);
	glNewList(this->list, GL_COMPILE);
	setMaterialv(user_defined_matrial);
	glmDraw(pmodel, GLM_SMOOTH);
	glEndList();
}
void SObject::Draw()
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(size, size, size);
	glCallList(list);
	glPopMatrix();
}
SObject::SObject(const char* filename, float x, float y, float z, GLfloat size, float m1, float m2, float m3)
{
	char name[50];
	strcpy(name, filename);
	this->pmodel = glmReadOBJ(name);
	if (!pmodel)
		exit(1);
	glmUnitize(pmodel);
	glmFacetNormals(pmodel);
	glmVertexNormals(pmodel, 90.0);

	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
	this->size = size;
	this->drawFlag = TRUE;

	this->user_defined_matrial[0] = m1;
	this->user_defined_matrial[1] = m2;
	this->user_defined_matrial[2] = m3;
}
SObject::SObject(const char* filename, float x, float y, float z, GLfloat size, GLfloat* parama)
{
	char name[50];
	strcpy(name, filename);
	this->pmodel = glmReadOBJ(name);
	if (!pmodel)
		exit(1);
	glmUnitize(pmodel);
	glmFacetNormals(pmodel);
	glmVertexNormals(pmodel, 90.0);

	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
	this->size = size;
	this->drawFlag = TRUE;

	this->user_defined_matrial[0] = parama[0];
	this->user_defined_matrial[1] = parama[1];
	this->user_defined_matrial[2] = parama[2];
}

Dice::Dice()
{
	srand(time(NULL));
	num = 0;
}
int Dice::dice_point()
{
	return rand() % 6 + 1;
}
void Dice::render() {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_dice);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_dice);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black_material);
	glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, black_material);
	glutSolidCube(4.0);
	glPopMatrix();
	glPushMatrix();
	dice_init_point();
	glPopMatrix();
}
void Dice::listRender()
{

	glPushMatrix();
	glTranslatef(monopoly.players[monopoly.now_player].x + monopoly.players[monopoly.now_player].offset_x,
		10.0, monopoly.players[monopoly.now_player].z + monopoly.players[monopoly.now_player].offset_z + 0.0);
	glRotatef(30.0, 1.0, 0.0, 0.0);
	if (dice_rotate == 1)
	{
		glRotatef(dice_rotate_angle0, 1.f, 1.f, 0.f);
		glRotatef(dice_rotate_angle1, 0.f, 1.f, 1.f);
		glRotatef(dice_rotate_angle2, 1.f, 0.f, 1.f);
		glScalef(0.4, 0.4, 0.4);
		glCallList(dice_init);
	}
	else
	{
		glScalef(0.4, 0.4, 0.4);
		dice_jump();
	}
	glPopMatrix();

}
void Dice::idle() {
	if (dice_rotate == 1) {
		switch (dice_rotate_mode) {
		case 0:
			dice_rotate_angle0 += 20.f;
			if (dice_rotate_angle0 == 200.f) {
				dice_rotate_mode = 1;
				//printf("0->1\n");
			}
			break;
		case 1:
			dice_rotate_angle1 += 24.f;
			if (dice_rotate_angle1 == 264.f) {
				dice_rotate_mode = 2;
				//printf("1->2\n");
			}
			break;
		case 2:
			dice_rotate_angle2 += 12.f;
			if (dice_rotate_angle2 == 228.f) {
				dice_rotate_mode = 0;
				//printf("2->0\n");
			}
			break;
		}
		if (dice_rotate_angle0 == 360.f) {
			dice_rotate_angle0 = 0.f;
		}
		if (dice_rotate_angle1 == 360.f) {
			dice_rotate_angle1 = 0.f;
		}
		if (dice_rotate_angle2 == 360.f) {
			dice_rotate_angle2 = 0.f;
		}
	}
	if (dice_scala_open == 1) {
		if (dice_scala_big == 1) {
			if (dice_scalaf < 1.3f) {
				dice_scalaf += 0.01f;
			}
			else {
				dice_scala_big = 0;
			}
		}
		else {
			if (dice_scalaf > 1.0f) {
				dice_scalaf -= 0.01f;
			}
			else {
				dice_scala_open = 0;
				dice_scalaf = 1.f;
				monopoly.toMOVING_STATE();
			}
		}
	}
	glutPostRedisplay();
}

Block::Block(string type, int bid, float x, float z, float roate, int setup)
{
	this->BlockType = type;
	this->bid = bid;
	this->x = x;
	this->z = z;
	Block* next = NULL;
	this->MouseControl = -1;

	this->setup = setup;
	if (type == "land")
	{
		this->color = default_material;
		this->bnum = 0;
		this->owner = -1;
	}
	else if (type == "corner")
	{
		this->color = corner_material;
	}
	else if (type == "game")
	{
		this->color = game_material;
	}
	else if (type == "card")
	{
		if (setup == CARD_0) // density
			this->color = density_material;
		if (setup == CARD_1)
			this->color = chance_material;
	}

	if (!this->color)
		exit(2);
}
void Block::render()
{
	glPushMatrix();
	glTranslatef(x, 0.0, z);
	float pos = BLOCK_SIZE / 2 - 0.2;
	//base on type
	//setMaterialv(color);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glColor3fv(color);
	glBegin(GL_POLYGON);
	glVertex3f(BLOCK_SIZE / 2, 0.01, BLOCK_SIZE / 2);
	glVertex3f(BLOCK_SIZE / 2, 0.01, -BLOCK_SIZE / 2);
	glVertex3f(-BLOCK_SIZE / 2, 0.01, -BLOCK_SIZE / 2);
	glVertex3f(-BLOCK_SIZE / 2, 0.01, BLOCK_SIZE / 2);
	glEnd();
	//外框
	//setMaterialv(black_material);
	glColor3fv(black_material);
	glBegin(GL_POLYGON);
	glVertex3f(-BLOCK_SIZE / 2, 0.02, BLOCK_SIZE / 2);
	glVertex3f(-BLOCK_SIZE / 2, 0.02, pos);
	glVertex3f(BLOCK_SIZE / 2, 0.02, pos);
	glVertex3f(BLOCK_SIZE / 2, 0.02, BLOCK_SIZE / 2);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(pos, 0.02, BLOCK_SIZE / 2);
	glVertex3f(pos, 0.02, -BLOCK_SIZE / 2);
	glVertex3f(BLOCK_SIZE / 2, 0.02, -BLOCK_SIZE / 2);
	glVertex3f(BLOCK_SIZE / 2, 0.02, BLOCK_SIZE / 2);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(BLOCK_SIZE / 2, 0.02, -BLOCK_SIZE / 2);
	glVertex3f(BLOCK_SIZE / 2, 0.02, -pos);
	glVertex3f(-BLOCK_SIZE / 2, 0.02, -pos);
	glVertex3f(-BLOCK_SIZE / 2, 0.02, -BLOCK_SIZE / 2);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-BLOCK_SIZE / 2, 0.02, -BLOCK_SIZE / 2);
	glVertex3f(-pos, 0.02, -BLOCK_SIZE / 2);
	glVertex3f(-pos, 0.02, BLOCK_SIZE / 2);
	glVertex3f(-BLOCK_SIZE / 2, 0.02, BLOCK_SIZE / 2);
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glRotatef(roate, 0.0, 1.0, 0.0);
	if (BlockType == "land")
	{
		if (bnum > 3)
		{
			// render 旅館
			glTranslatef(0.0, 1.6, 0.25 * BLOCK_SIZE);
			glScalef(2.0, 2.0, 2.0);
			buildings[0].Draw();
		}
		else
		{
			// render 普通房子
			glTranslatef(-(BLOCK_SIZE * 0.35) * 2, 0.8, 0.35 * BLOCK_SIZE);
			for (int i = 0; i < bnum; i++)
			{
				glTranslatef(0.35 * BLOCK_SIZE, 0.0, 0.0);
				buildings[1].Draw();
			}
		}
	}
	glPopMatrix();

}
void Block::BlockEvent()
{
	if (MouseControl == -1)
		Interface();
	else
	{
		if (BlockType == "land")
		{
			if (owner == -1)//無主地 可買
			{

				if (MouseControl == 1)
				{
					if (monopoly.getNowPlayer()->money > setup)
					{
						owner = monopoly.now_player;
						color = player_material[owner];
						monopoly.getNowPlayer()->money -= setup;
					}
				}
			}
			else if (owner != monopoly.now_player) //他地，付過路費
			{

				long payment = setup * (bnum + 1) / 3;
				long temp = monopoly.getNowPlayer()->money;

				monopoly.getNowPlayer()->money -= payment;
				monopoly.players[owner].money += payment;


			}
			else if (bnum < 3)
			{
				int payment = setup / 4 * (bnum + 1) * 4 / 3;
				if (MouseControl == 1)
				{
					if (monopoly.getNowPlayer()->money > payment)
					{
						bnum++;
						monopoly.getNowPlayer()->money -= payment;
					}
				}
			}
			else if (bnum == 3)
			{
				int payment = setup / 4 * (bnum + 4) * 4 / 3;
				if (MouseControl == 1)
				{
					if (monopoly.getNowPlayer()->money > payment)
					{
						bnum = 5;
						monopoly.getNowPlayer()->money -= payment;
					}
				}
			}
			else
			{
				MouseControl = 0;
			}

		}
		else if (BlockType == "corner")
		{
			if (setup == CORNER_START)
			{
				monopoly.getNowPlayer()->money += 2000;
			}
			else if (setup == CORNER_GOPRISON)
			{
				//target : monopoly.blocks[12];
				Player* target = monopoly.getNowPlayer();
				target->blockId = &monopoly.blocks[12];
				target->x = monopoly.blocks[12].x;
				target->z = monopoly.blocks[12].z;
				target->RoundStall = 2;
			}
			else if (setup == CORNER_FREEPARK)
			{
				Player* target = monopoly.getNowPlayer();
				target->RoundStall = 1;
			}
		}
		else if (BlockType == "game")
		{

		}
		else if (BlockType == "card")
		{
			Player* target = monopoly.getNowPlayer();
			if (CARD_Discriptor[setup][owner][1] == "坐牢")
			{
				//target : monopoly.blocks[12];
				target->blockId = &monopoly.blocks[12];
				target->x = monopoly.blocks[12].x;
				target->z = monopoly.blocks[12].z;
				target->RoundStall = 2;
			}
			else if (CARD_Discriptor[setup][owner][1] == "0")
			{
				// +1 round stall
				target->RoundStall = 1;
			}
			else
			{
				stringstream ss;
				ss << CARD_Discriptor[setup][owner][1];
				int payment = 0;
				ss >> payment;
				printf("%d\n", payment);
				target->money += payment;
			}
		}
		MouseControl == -1;
		if (BlockType != "game")
			monopoly.toREADY_STATE();
		else
			monopoly.toGAME_STATE();
	}

}
void Block::Interface()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	float ScreenRate = ScreenWidth / ScreenHeight;
	glPushMatrix();

	glColor3fv(color);
	selectFont(24, DEFAULT_CHARSET, "華文仿宋");
	//glRasterPos3f(camPosx * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);

	if (BlockType == "land")
	{
		LandInterface();
	}
	else if (BlockType == "game")
	{
		camPosx = 50.0; camPosy = 30.0; camPosz = 0.0;
		GameInterface();
	}
	else if (BlockType == "card")
	{
		camPosy += 0.1;
		if (camPosy > default_camPosy + 15.0)
			camPosy = default_camPosy + 15.0;
		glRasterPos3f(camPosx * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
		CardInterface();
	}
	else if (BlockType == "corner")
	{
		CornerInterface();
	}

	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
void Block::LandInterface()
{
	float ScreenRate = ScreenWidth / ScreenHeight;
	string str;
	stringstream ss;
	if (owner == -1)
	{
		//無主地
		ss << this->setup;
		str = "這地方看起來不錯，價格為";
		str.append(ss.str());
		str.append("，要買嗎 ? ");
		glColor3f(1.0, 1.0, 1.0);

		glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
		str = "是(左鍵)            否(右鍵)";
		glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
	}
	else if (owner != monopoly.now_player)
	{
		ss << owner + 1;
		str = "此地屬於玩家";
		str.append(ss.str());
		str.append("，");
		if (monopoly.players[owner].blockId->bid == 12 && monopoly.players[owner].RoundStall > 0)
		{
			str.append("因該玩家正在坐牢，故此次過路費免費!");
		}
		else
		{
			int payment = setup * (bnum + 1) / 3;
			if (bnum > 3)
				str.append("有1棟旅館");
			else
			{
				ss.str("");
				ss << bnum;
				str.append("有");
				str.append(ss.str());
				str.append("棟房地產");
			}
			str.append("，需付");
			ss.str("");
			ss << payment;
			str.append(ss.str());
			str.append("當作過路費");
		}

		glRasterPos3f(camPosx + 0.2 - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
		str = "確認(點擊任意處)";
		glRasterPos3f(camPosx + 0.2 - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
	}
	else if (bnum < 3)
	{
		int payment = setup / 4 * (bnum + 1) * 4 / 3;
		str = "可以加蓋房子，費用為";
		ss << payment;
		str.append(ss.str());
		str.append("要建造嗎?");
		glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
		str = "是(左鍵)            否(右鍵)";
		glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
	}
	else if (bnum == 3)
	{
		int payment = setup / 4 * (bnum + 4) * 4 / 3;
		str = "可以升級為旅館，費用為";
		ss << payment;
		str.append(ss.str());
		str.append("要建造嗎?");
		glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
		str = "是(左鍵)            否(右鍵)";
		glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());
	}

}
void Block::GameInterface()
{
	float ScreenRate = ScreenWidth / ScreenHeight;
	string str = "小遊戲: ";

	if (setup == GAME_TYPE_POKER)
		str.append("撲克牌猜大小");
	else if (setup == GAME_TYPE_SHOOTDART)
		str.append("射飛鏢");

	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
	str = "確認(點擊任意處)";
	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
}
void Block::CornerInterface()
{
	float ScreenRate = ScreenWidth / ScreenHeight;
	string str;

	if (setup == CORNER_FREEPARK)
		str = "路過免費停車場，休息1回合";
	else if (setup == CORNER_GOPRISON)
		str = "交了成圖作業但沒有壓縮，去坐牢";
	else if (setup == CORNER_PRISON)
		str = "路過監獄，無事發生";
	else if (setup == CORNER_START)
		str = "回到起點，獎勵2000元";

	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
	str = "確認(點擊任意處)";
	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
}
void Block::CardInterface()
{
	float ScreenRate = ScreenWidth / ScreenHeight;
	string str;

	
	if (setup == CARD_0)
		str = "命運:";
	else if (setup == CARD_1)
		str = "機會:";
	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy * 1.01 + 0.15 * ScreenRate, camPosz - 0.55);
	drawCNString(str.c_str());

	str = CARD_Discriptor[setup][owner][0];
	glRasterPos3f(camPosx - 0.01 * (str.length() / 2) * ScreenRate, camPosy * 1.01 + 0.1 * ScreenRate, camPosz - 0.55);
	drawCNString(str.c_str());
	str = "確認(點擊任意處)";
	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy * 1.01 + 0.05 * ScreenRate, camPosz - 0.55);
	drawCNString(str.c_str());
}
void Block::UserEventMouseControl(int button, int state, int x, int y)
{
	switch (button)
	{
		// Wheel reports as button 3(scroll up) and button 4(scroll down)
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			MouseControl = 1;
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			MouseControl = 0;
		break;
	default:
		MouseControl = -1;
		break;
	}
}

Player::Player(int pid, float offset_x, float offset_z, Block* start)
{
	this->pid = pid;
	this->offset_x = offset_x;
	this->offset_z = offset_z;
	this->money = 12000;
	this->RoundStall = 0;
	this->model = SObject("objects/Player1.obj", 0.0, 1.5, 0.0, 2.0, player_material[pid]);
	this->blockId = start;
	this->x = blockId->x;
	this->y = 0.0;
	this->z = blockId->z;
	this->moving_setp = 0;
}
void Player::render()
{
	glPushMatrix();
	glTranslatef(x, y + 0.1, z);
	glTranslatef(offset_x, 0.0, offset_z);
	glRotatef(blockId->roate, 0.0, 1.0, 0.0);
	model.Draw();
	glPopMatrix();
}
Block* Player::getNext()
{
	return blockId->next;
}
void Player::idle()
{
	if (MOVING_FLAG)
	{
		if (!ON_JUMPING)
		{
			gra_speed = 12.0;
			ON_JUMPING = TRUE;
		}

		static float speed = 0.02;

		float x = getNext()->x;
		float z = getNext()->z;

		this->x += (x - this->blockId->x) * speed;
		if (abs(this->x - x) <= 0.05)
			this->x = x;
		this->z += (z - this->blockId->z) * speed;
		if (abs(this->z - z) <= 0.05)
			this->z = z;

		if (this->x == x && this->z == z)
		{
			this->blockId = getNext();
			this->x = blockId->x;
			this->z = blockId->z;
			dice.count--;
		}

		if (dice.count == 0)
		{
			MOVING_FLAG = FALSE;
			this->x = blockId->x;
			this->z = blockId->z;
			monopoly.toEVENT_STATE();
		}
	}


}
void Player::gravity()
{
	// gravity

	y += gra_speed / 40.0;
	gra_speed -= gra / 20.0;
	if (y < 0.0)
	{
		y = 0.0;
		gra_speed = 0.0;
		ON_JUMPING = FALSE;
	}
}

MonopolyGame::MonopolyGame()
{
	this->state = READY_STATE;
	this->HUD_Display_FLAG = TRUE;
	this->now_player = 0;
	//this->init();
}
void MonopolyGame::Render()
{
	glPushMatrix();
	//glTranslatef(0.0, -10.0, -20.0);
	//glRotatef(30.0, 1.0, 0.0, 0.0);
	// render maps
	static float map_mat[] = { 0.2, 0.2, 0.2, 1.0 };
	static float pos = MAP_SIZE * Block::BLOCK_SIZE / 2.0;
	setMaterialv(map_mat);
	glBegin(GL_POLYGON);
	glVertex3f(-pos, 0.0, pos);
	glVertex3f(-pos, 0.0, -pos);
	glVertex3f(pos, 0.0, -pos);
	glVertex3f(pos, 0.0, pos);
	glEnd();
	// render decorlatives
	for (int i = 0; i < sizeof(decors) / sizeof(SObject); i++)
		decor[i].Draw();
	// render blocks
	for (int i = 0; i < 16; i++)
		blocks[i].render();
	// render players
	for (int i = 0; i < PlayerNum; i++)
		players[i].render();
	glPopMatrix();
	// render Event
	if (state == EVENT_STATE)
		players[now_player].blockId->BlockEvent();
	// render End
	if (state == END_STATE)
		GameOver();

}
void MonopolyGame::init()
{
	// blocks	
	static Block block[] = {
		Block("corner",  0, -20,  20, 0.0, CORNER_START),
		Block("land"  ,  1, -10,  20, 0.0, 1400),
		Block("land"  ,  2,   0,  20, 0.0, 1800),
		Block("game"  ,  3,  10,  20, 0.0, GAME_TYPE_POKER),
		Block("corner",  4,  20,  20, 0.0, CORNER_GOPRISON),
		Block("land"  ,  5,  20,  10, 0.0, 2200),
		Block("card"  ,  6,  20,   0, 0.0, CARD_0),
		//Block("land"  ,  6,  20,   0, 0.0, 1900),
		Block("land"  ,  7,  20, -10, 0.0, 1600),
		Block("corner",  8,  20, -20, 0.0, CORNER_FREEPARK),
		Block("land"  ,  9,  10, -20, 0.0, 2500),
		Block("game"  , 10,   0, -20, 0.0, GAME_TYPE_SHOOTDART),
		Block("land"  , 11, -10, -20, 0.0, 3000),
		Block("corner", 12, -20, -20, 0.0, CORNER_PRISON),
		Block("land"  , 13, -20, -10, 0.0, 3300),
		Block("card"  , 14, -20,   0, 0.0, CARD_1),
		//Block("land"  , 14, -20,   0, 0.0, 3600),
		Block("land"  , 15, -20,  10, 0.0, 4000)
	};
	blocks = block;
	for (int i = 0; i < 15; i++)
		blocks[i].next = blocks + i + 1;
	blocks[15].next = blocks;
	// players
	static Player player[] = {
		Player(0, -1.0, -1.0, blocks),
		Player(1,  1.0,  1.0, blocks)
	};
	players = player;
	for (int i = 0; i < PlayerNum; i++)
		players[i].model.init();
	// decoratives
	decor = decors;
	for (int i = 0; i < sizeof(decors) / sizeof(SObject); i++)
		decor[i].init();

}
void MonopolyGame::nextPlayer()
{
	now_player++;
	now_player %= PlayerNum;
}
void MonopolyGame::HUD(int PlayerID)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	// Wid 0.118, Hei 0.06
	float ScreenRate = ScreenWidth / ScreenHeight;
	glPushMatrix();
	//glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);

	selectFont(24, DEFAULT_CHARSET, "華文仿宋");
	for (int i = 0; i < PlayerNum; i++)
	{
		stringstream ss;
		string str = "玩家";
		ss << i + 1;
		str.append(ss.str());
		str.append(":");

		glRasterPos3f(camPosx - 0.5 * ScreenRate, camPosy + (0.2 - 0.2 * i) * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());

		ss.str("");
		str = "財產:";
		// player money
		ss << players[i].money;
		str.append(ss.str());
		glRasterPos3f(camPosx - 0.5 * ScreenRate, camPosy + (0.2 - 0.2 * i - 0.03) * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());

		ss.str("");
		str = "土地數:";
		int estimates = 0;
		int meta_estimates = 0;
		int lands = 0;
		for (int j = 0; j < monopoly.blockNum; j++)
			if (monopoly.blocks[j].owner == i && monopoly.blocks[j].BlockType == "land")
			{
				lands++;
				if (monopoly.blocks[j].bnum != 5)
					estimates += monopoly.blocks[j].bnum;
				else if (monopoly.blocks[j].bnum == 5)
					meta_estimates++;
			}

		ss << lands;
		str.append(ss.str());
		glRasterPos3f(camPosx - 0.5 * ScreenRate, camPosy + (0.2 - 0.2 * i - 0.06) * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());

		ss.str("");
		str = "房地產數:";
		ss << estimates;
		str.append(ss.str());
		glRasterPos3f(camPosx - 0.5 * ScreenRate, camPosy + (0.2 - 0.2 * i - 0.09) * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());

		ss.str("");
		str = "旅館數:";
		ss << meta_estimates;
		str.append(ss.str());
		glRasterPos3f(camPosx - 0.5 * ScreenRate, camPosy + (0.2 - 0.2 * i - 0.12) * ScreenRate, camPosz - 0.5);
		drawCNString(str.c_str());

		if (monopoly.players[i].RoundStall > 0)
		{
			ss.str("");
			str = "休息回合剩餘:";
			ss << monopoly.players[i].RoundStall;
			str.append(ss.str());
			glRasterPos3f(camPosx - 0.5 * ScreenRate, camPosy + (0.2 - 0.2 * i - 0.15) * ScreenRate, camPosz - 0.5);
			drawCNString(str.c_str());
		}
	}
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
Player* MonopolyGame::getNowPlayer()
{
	return players + now_player;
}
void MonopolyGame::toREADY_STATE()
{
	camPosx = default_camPosx;
	camPosy = default_camPosy;
	camPosz = default_camPosz;
	nextPlayer();
	while (getNowPlayer()->RoundStall > 0)
	{
		getNowPlayer()->RoundStall--;
		nextPlayer();
	}
	HUD_Display_FLAG = TRUE;
	state = READY_STATE;
}
void MonopolyGame::toROLLING_STATE()
{
	HUD_Display_FLAG = FALSE;
	dice.can_roll = TRUE;
	dice.dice_rotate = TRUE;
	state = ROLLING_STATE;
}
void MonopolyGame::toMOVING_STATE()
{
	HUD_Display_FLAG = FALSE;
	dice.count = dice.num;
	getNowPlayer()->MOVING_FLAG = TRUE;
	state = MOVING_STATE;
}
void MonopolyGame::toEVENT_STATE()
{
	//camPosx = 50.0; camPosy = 30.0; camPosz = 0.0;
	players[now_player].blockId->MouseControl = -1;
	if (getNowPlayer()->blockId->BlockType == "game")
		HUD_Display_FLAG = FALSE;
	else if (getNowPlayer()->blockId->BlockType == "card")
	{
		HUD_Display_FLAG = TRUE;
		getNowPlayer()->blockId->owner = rand() % sizeof(CARD_Discriptor[0]) / sizeof(string) / 2;
	}else
		HUD_Display_FLAG = TRUE;
	state = EVENT_STATE;
}
void MonopolyGame::UserEventMouseControl(int button, int state, int x, int y)
{
	players[now_player].blockId->UserEventMouseControl(button, state, x, y);
}
void MonopolyGame::detectPlayer()
{
	for (int i = 0; i < PlayerNum; i++)
		if (players[i].money < 0)
			toEND_STATE();
}
void MonopolyGame::GameOver()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	// Wid 0.118, Hei 0.06
	float ScreenRate = ScreenWidth / ScreenHeight;

	selectFont(32, DEFAULT_CHARSET, "華文仿宋");
	glColor4f(1.0, 1.0, 1.0, 0.5);
	string str = "遊戲結束";
	glRasterPos3f(camPosx + 0.08 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
	str = "確認(點擊任意處)";
	glRasterPos3f(camPosx + 0.08 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
void MonopolyGame::toEND_STATE()
{
	HUD_Display_FLAG = TRUE;
	state = END_STATE;
}
void MonopolyGame::toGAME_STATE()
{
	shootDart.shoot_reset();
	if (getNowPlayer()->blockId->setup == GAME_TYPE_SHOOTDART)
		state = GAME_STATE_SHOOTDART;
	else
		toREADY_STATE();
}

void GameShootDart::GameMouseContorl(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			if (shoot_mode == XMOVEMODE) {
				shoot_mode = ANGLEMODE;
			}
			else if (shoot_mode == ANGLEMODE) {
				shoot_mode = STRENGTHMODE;
			}
			else if (shoot_mode == STRENGTHMODE) {
				shoot_mode = SHOOTMODE;
			}
			else if (shoot_mode == SHOOTMODE) {
				shoot_mode = SHOWRESULT;
			}
			else if (shoot_mode == SHOWRESULT) {
				GamePoint = point;
				shoot_reset();
				shoot_mode = OUTPUTOUTCOME;
			}
			else if (shoot_mode == OUTPUTOUTCOME)
			{
				monopoly.getNowPlayer()->money += GamePoint * 100;
				monopoly.toREADY_STATE();
			}
		}
		break;
	default:
		break;
	}

}
void GameShootDart::OutcomeInterface()
{
	float ScreenRate = ScreenWidth / ScreenHeight;
	string str = "射飛鏢結果: 得分";
	stringstream ss;

	ss << GamePoint;
	str.append(ss.str());

	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.1 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
	str = "獎勵: ";
	ss.str("");
	ss << GamePoint * 100;
	str.append(ss.str());
	str.append("元");
	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy + 0.05 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
	str = "確認(點擊任意處)";
	glRasterPos3f(camPosx - 0.02 * (str.length() / 2) * ScreenRate, camPosy - 0.05 * ScreenRate, camPosz - 0.5);
	drawCNString(str.c_str());
}
