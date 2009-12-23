#include <gl/glew.h>
#include <gl/glut.h>
#include <cstdlib>
#include <queue>
#include <windows.h>
#include <Space-Time/Vector2D.h>

#define windowSide 1000
#define margin 20.0f
#define levels 8


struct Triangle
{
	Vector2D<float> v1;
	Vector2D<float> v2;
	Vector2D<float> v3;

	Triangle(){}

	Triangle(Vector2D<float> &av1,Vector2D<float> &av2,Vector2D<float> &av3)
		:v1(av1),v2(av2),v3(av3){}
}; // end sturct Triangle

static std::queue<Triangle> triangles;
static float image[windowSide][windowSide][3];

void display()
{
static int level = 0;
static int nCurrentLevel = 1;
static int nNextLevel = 0;

	glDrawPixels(windowSide,windowSide,GL_RGB,GL_FLOAT,image);

	if(level < levels)
	{
		while(nCurrentLevel--)	// foreach triangle in this level
		{
			// calculate center triangle's vertices
			Triangle &t = triangles.front();	// does not remove the head element of the queue
			Vector2D<float> c1 = (t.v1+t.v2)/2;
			Vector2D<float> c2 = (t.v2+t.v3)/2;
			Vector2D<float> c3 = (t.v3+t.v1)/2;

			// draw center triangle
			glBegin(GL_TRIANGLES);
				glVertex2fv(c1);
				glVertex2fv(c2);
				glVertex2fv(c3);
			glEnd();

			// put smaller triangles in the queue
			triangles.push(Triangle(t.v1,c1,c3));
			triangles.push(Triangle(c1,t.v2,c2));
			triangles.push(Triangle(c3,c2,t.v3));

			// discard used triangle
			triangles.pop();

			nNextLevel += 3;
		} // end while
		nCurrentLevel = nNextLevel;
		nNextLevel = 0;

		++level;
		glReadPixels(0,0,windowSide,windowSide,GL_RGB,GL_FLOAT,image);
	} // end if

	glutSwapBuffers();
} // end function display


void always()
{
	Sleep(333);
	glutPostRedisplay();
} // end function always


void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:	// escape key
		exit(0);
	} // end switch
} // end function keyboard


void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
} // end function reshape


int main(int argc, char **argv)
{
	// glut initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(windowSide,windowSide);
	glutInitWindowPosition(1270-windowSide,20);
	glutCreateWindow("Sierpinski Triangle");

	// glew initialization
	glewInit();

	// OpenGL initialization
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glColor3f(1.0,0.75,0.0);
	glWindowPos2i(0,0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT,GL_LINE);
	glLineWidth(0.5);

	// application initialization
	{
		float triangleSide = windowSide - 2*margin;
		float h = 0.8660254038f*triangleSide;	// sqrt(3)/2 * triangleSide
		float vMargin = 0.5f*(windowSide-h);
		Triangle t;

		t.v1.x = 0.5*windowSide;
		t.v1.y = vMargin+h;
		t.v2.x = margin;
		t.v2.y = vMargin;
		t.v3.x = margin+triangleSide;
		t.v3.y = vMargin;
		triangles.push(t);

		glViewport(0, 0, (GLsizei) windowSide, (GLsizei) windowSide);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, (GLdouble) windowSide, 0.0, (GLdouble) windowSide);

		glBegin(GL_TRIANGLES);
			glVertex2fv(t.v1);
			glVertex2fv(t.v2);
			glVertex2fv(t.v3);
		glEnd();
		glReadPixels(0,0,windowSide,windowSide,GL_RGB,GL_FLOAT,image);
		glutSwapBuffers();
	}

	// event handling initialization
	glutIdleFunc(always);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
} // end function main
