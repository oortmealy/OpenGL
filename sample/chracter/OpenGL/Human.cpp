#include<stdio.h>

#include<stdlib.h>

#include<Windows.h>

#include<gl/glut.h>

#include<gl/GL.h>

#include<gl/GLU.h>



void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1, 2, 2, // 눈의 위치
		0, 0, 0, // 바라보는 목표점의 위치
		0, 1, 0); // 카메라의 up 벡터

	glDisable(GL_LIGHTING);
	//DrawAxis(1, 0, 0); // x 축
	//DrawAxis(0, 1, 0); // y 축
	//DrawAxis(0, 0, 1); // z 축 
	glEnable(GL_LIGHTING); //조명을 켜는것
	glutSolidSphere(0.5, 150, 150); // 법선벡터찾는게 다 들어가있다!
	glutSwapBuffers();



}

void reshape(int w, int h) {
	float asp = float(w) / float(h); // aspect ratio (종횡비)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(60, asp, 0.1, 100);
	glutPostRedisplay();
}


void main(int argc, char **argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);

	glutInitWindowPosition(100, 100);

	glutInitWindowSize(320, 320);

	glutCreateWindow("3D Tech- GLUT Tutorial");

	glutDisplayFunc(renderScene);

	glutIdleFunc(renderScene);
	glutReshapeFunc(reshape);

	glutMainLoop();
	

}