//	Copyright (C) 2009, 2011, 2012 Vaptistis Anogeianakis <nomad@cornercase.gr>
/*
 *	This file is part of Sierpinski Triangle.
 *
 *	Sierpinski Triangle is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	Sierpinski Triangle is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with Sierpinski Triangle.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdlib>
#include <deque>
#include <vector>
#include <algorithm>
#include <Space-Time/Vector2D.h>

#define initialWindowOffset 20
#define minMargin 20.0f
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

static std::deque<Triangle> triangles;
static unsigned windowHeight;
static unsigned windowWidth;
struct Float3 {float data[3];};	// a workaround because arrays can't be...
static std::vector<Float3> image;	// ...assigned to each other.
static std::vector<Triangle> vertices;
static std::vector<Triangle> directions;

static int level = 0;
static int nCurrentLevel = 1;
static int nNextLevel = 0;
static float t = 0.0;


void simpleDisplay()
{
	glDrawPixels(windowWidth,windowHeight,GL_RGB,GL_FLOAT,image.data());
	glutSwapBuffers();
} // end function simpleDisplay


void display()
{
	glDrawPixels(windowWidth,windowHeight,GL_RGB,GL_FLOAT,image.data());

	if(t<=1.0)
	{
		glBegin(GL_LINES);
			for(unsigned int c = 0 ; c < vertices.size() ; ++c)
			{
				glVertex2fv(vertices[c].v1);
				glVertex2fv(vertices[c].v1 + t*directions[c].v1);
				glVertex2fv(vertices[c].v2);
				glVertex2fv(vertices[c].v2 + t*directions[c].v2);
				glVertex2fv(vertices[c].v3);
				glVertex2fv(vertices[c].v3 + t*directions[c].v3);
			} // end for
		glEnd();
		t += 0.030f;
	}
	else
	{
		t = 0.0;
		glBegin(GL_TRIANGLES);
			for(unsigned int c = 0 ; c < vertices.size() ; ++c)
			{
				glVertex2fv(vertices[c].v1);
				glVertex2fv(vertices[c].v2);
				glVertex2fv(vertices[c].v3);
			} // end for
		glEnd();
		vertices.clear();
		directions.clear();

		// start new level
		while(nCurrentLevel--)	// foreach triangle in this level
		{
			// calculate center triangle's vertices
			Triangle &t = triangles.front();	// does not remove the head element of the queue
			Vector2D<float> c1 = (t.v1+t.v2)/2;
			Vector2D<float> c2 = (t.v2+t.v3)/2;
			Vector2D<float> c3 = (t.v3+t.v1)/2;

			// register center triangle
			vertices.push_back(Triangle(c1,c2,c3));
			directions.push_back(Triangle(c2-c1,c3-c2,c1-c3));

			// put smaller triangles in the queue
			triangles.push_back(Triangle(t.v1,c1,c3));
			triangles.push_back(Triangle(c1,t.v2,c2));
			triangles.push_back(Triangle(c3,c2,t.v3));

			// discard used triangle
			triangles.pop_front();

			nNextLevel += 3;
		} // end while
		nCurrentLevel = nNextLevel;
		nNextLevel = 0;

		++level;
		glReadPixels(0,0,windowWidth,windowHeight,GL_RGB,GL_FLOAT,image.data());
		if(level > levels)
			glutDisplayFunc(simpleDisplay);
	} // end else

	glutSwapBuffers();
} // end function display


void always()
{
	glutPostRedisplay();
} // end function always


void initializeAnimation()
{
	/*	triangleSide should have the largest value with triangleSide <= windowWidth - 2*minMargin	*
	 *	and an h <= windowHeight - 2*minMargin. (h = sqrt(3)/2 * triangleSide)						*/
	float triangleSide = std::min(windowWidth-2*minMargin , 1.1547005383f/* 2/sqrt(3) */*(windowHeight-2*minMargin));
	float h = 0.8660254038f/* sqrt(3)/2 */*triangleSide;
	float hMargin = 0.5f*(windowWidth-triangleSide);
	float vMargin = 0.5f*(windowHeight-h);
	Triangle t;

	t.v1.x = 0.5*windowWidth;
	t.v1.y = vMargin+h;
	t.v2.x = hMargin;
	t.v2.y = vMargin;
	t.v3.x = hMargin+triangleSide;
	t.v3.y = vMargin;
	triangles.push_back(t);

	vertices.push_back(t);
	directions.push_back(Triangle(t.v2-t.v1,t.v3-t.v2,t.v1-t.v3));
} // end function initialize


void clearAnimation()
{
	level = 0;
	nCurrentLevel = 1;
	nNextLevel = 0;
	t = 0.0;

	triangles.clear();
	vertices.clear();
	directions.clear();
} // end function clearCurrentAnimation


void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case '\r':	// enter key
		// reset
		clearAnimation();
		memset(image.data(),0,windowWidth*windowHeight*sizeof(Float3));	// clear image to black
		initializeAnimation();
		glutDisplayFunc(display);
		break;
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

	windowWidth = w;
	windowHeight = h;
	image.resize(windowWidth*windowHeight);
	clearAnimation();
	memset(image.data(),0,windowWidth*windowHeight*sizeof(Float3));	// clear image to black
	initializeAnimation();
	glutDisplayFunc(display);
} // end function reshape


int main(int argc, char **argv)
{
	// glut initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH)/2,glutGet(GLUT_SCREEN_HEIGHT)/2);
	glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH)-initialWindowOffset-glutGet(GLUT_SCREEN_WIDTH)/2,initialWindowOffset);
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

	// event handling initialization
	glutIdleFunc(always);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
} // end function main
