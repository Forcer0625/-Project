/***************************
4108056036 洪郁修 第6-VI次作業12/22
***************************/
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

#define WIDTH 1280
#define HEIGHT 720
#define MAX_CHAR 128

using namespace std;
static GLfloat light_position[] = {50.0, 50.0, 50.0};
static GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
// Screen Size
static GLfloat ScreenWidth = WIDTH;
static GLfloat ScreenHeight = HEIGHT;

const static float PI = 3.14159;
static float camPosz = 30.0; //30
static float camPosx = 0.0;
static float camPosy = 10.0;

void setMaterialv(const GLfloat* params);
void DrawCircle(float cx, float cy, float r, int num_segments);
void drawAxis();
void drawAxis(float locX, float LocY, float LocZ);
void drawAxis(float*);
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

void init(void)
{
	/* selcet clearing color*/
	glClearColor(0.0, 0.0, 0.0, 0.0); //channel: RGBA
	glShadeModel(GL_SMOOTH);

    {
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
        glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 400.0);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 15.0);
        //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
        //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
    }
    
	{
		GLfloat fogColor[4] = { 0.2, 0.2, 0.2, 0.5 };

		glFogi(GL_FOG_MODE, GL_EXP);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 0.03);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glFogf(GL_FOG_START, 1.0);
		glFogf(GL_FOG_END, 5.0);
	}
	glClearColor(0.2, 0.2, 0.2, 0.5);  /* fog color */
    // Anti-Alias
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    // Depth
    glEnable(GL_DEPTH_TEST);
    // load object
	
}

void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			glutPostRedisplay();
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			glutPostRedisplay();
		break;
	default:
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(0);
	}
}
void HUD(int PlayerID);
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
    gluLookAt(camPosx, camPosy, camPosz, camPosx, camPosy, camPosz - 100.0, 0.0, 1.0, 0.0);
	
	HUD(0);
    
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
	gluLookAt(camPosx, camPosy, camPosz, camPosx, camPosy, camPosz -100.0, 0.0, 1.0, 0.0);	
}

void idle()
{
	
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
	//glutInitWindowPosition(-200,60);
	// Create the window with the title "Hello,GL"
	glutCreateWindow("Game");

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

			glVertex3f(x + cx, y + cy,0.0);//output vertex

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
void HUD(int PlayerID)
{
	// Wid 0.118, Hei 0.0625
	float ScreenRate = ScreenWidth / ScreenHeight;
	glRasterPos3f(camPosx, camPosy, camPosz-0.1);
	glRasterPos3f(camPosx-0.118*ScreenRate, camPosy-0.0625*ScreenRate, camPosz - 0.1);
	drawString("Player:126");
}
