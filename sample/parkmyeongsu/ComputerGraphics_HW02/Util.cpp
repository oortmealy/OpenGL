#include "StdAfx.h"
GLvoid DrawNet(GLfloat size, GLint LinesX, GLint LinesZ)
{
	glBegin(GL_LINES);
	for (int xc = 0; xc < LinesX; xc++)
	{
		glVertex3f(	-size / 2.0 + xc / (GLfloat)(LinesX-1)*size,
					0.0,
					size / 2.0);
		glVertex3f(	-size / 2.0 + xc / (GLfloat)(LinesX-1)*size,
					0.0,
					size / -2.0);
	}
	for (int zc = 0; zc < LinesX; zc++)
	{
		glVertex3f(	size / 2.0,
					0.0,
					-size / 2.0 + zc / (GLfloat)(LinesZ-1)*size);
		glVertex3f(	size / -2.0,
					0.0,
					-size / 2.0 + zc / (GLfloat)(LinesZ-1)*size);
	}
	glEnd();
}

GLvoid DrawGuide()
{
	glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);

	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,1.0f);
	glEnd();
}

GLuint* GetTexturePool()
{
	static GLuint toTextures[20];
	return toTextures;
}

GLvoid DrawMan()
{
    glEnable(GL_DEPTH_TEST);	
    glFrontFace(GL_CCW);		
    glEnable(GL_CULL_FACE);		
	glPushMatrix();
		//얼굴
	    glBindTexture(GL_TEXTURE_2D, GetTexturePool()[FACE]);
		glTranslatef(0.0f,1.1f,0.0f);
		glScalef(0.5,0.65,0.55);
		mySphere(1,1,1);
	glPopMatrix();
	glPushMatrix();
		//목
		glBindTexture(GL_TEXTURE_2D, GetTexturePool()[NECK]);
		glTranslatef(0.0f,0.1f,0.0f);
		myCapsule(0.23f,0.1f,0.25f,0.8f);
	glPopMatrix();

	glPushMatrix();
		//셔츠
		glBindTexture(GL_TEXTURE_2D, GetTexturePool()[SHIRTS]);
		glTranslatef(0.0f,-2.0f,0.0f);
		myCapsule(0.9f,0.3f,0.45f,2.0f);

		glPushMatrix();
			glTranslatef(-1.1f,1.7f,0.0f);
			glRotatef(-60,0,0,1);
			mySphere(0.35,1.0,0.3);
			glTranslatef(-0.4f,-1.2f,0.0f);
			glRotatef(-50,0,0,1);
			mySphere(0.25,0.7,0.2);
			glTranslatef(0.0f,-0.8f,0.0f);
			//손
			glBindTexture(GL_TEXTURE_2D, GetTexturePool()[HAND]);
			myCapsule(0.18,0.2,0.15,0.05f);
			//손가락
			glTranslatef(0.0f,-0.3f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(0.1f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(-0.05f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(-0.1f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(-0.05f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(-0.1f,0.2f,0.0f);
			myCapsule(0.04,0.1,0.04,0.02f);
		glPopMatrix();
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, GetTexturePool()[SHIRTS]);
			glTranslatef(1.1f,1.7f,0.0f);
			glRotatef(60,0,0,1);
			mySphere(0.35,1.0,0.3);
			glTranslatef(0.4f,-1.2f,0.0f);
			glRotatef(50,0,0,1);
			mySphere(0.25,0.7,0.2);
			glTranslatef(0.0f,-0.8f,0.0f);
			//손
			glBindTexture(GL_TEXTURE_2D, GetTexturePool()[HAND]);
			myCapsule(0.18,0.2,0.15,0.05f);
			//손가락
			glTranslatef(0.0f,-0.3f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(0.1f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(-0.05f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(-0.1f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(-0.05f,0.0f,0.0f);
			myCapsule(0.02,0.1,0.02,0.05f);
			glTranslatef(0.3f,0.2f,0.0f);
			myCapsule(0.04,0.1,0.04,0.02f);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.5f,-0.5f,0.65f);
			glRotatef(-2,0,0,1);
			glRotatef(-50,1,0,0);
			//바지
			glBindTexture(GL_TEXTURE_2D, GetTexturePool()[PANTS]);
			mySphere(0.46,1.1,0.4);
			glTranslatef(0.0f,-1.2f,-0.15f);
			//glRotatef(3,0,0,1);
			glRotatef(25,1,0,0);
			mySphere(0.27,0.9,0.33);
			glTranslatef(0.0f,-0.90f,0.4f);
			glRotatef(-90,1,0,0);
			//발
			glBindTexture(GL_TEXTURE_2D, GetTexturePool()[FOOT]);
			myCapsule(0.18,0.2,0.15,0.4f);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.5f,-0.8f,0.0f);
			glRotatef(-2,0,0,1);
			//바지
			glBindTexture(GL_TEXTURE_2D, GetTexturePool()[PANTS]);
			mySphere(0.46,1.3,0.4);
			glTranslatef(0.0f,-1.2f,0.1f);
			glRotatef(3,0,0,1);
			mySphere(0.3,1.0,0.35);
			glTranslatef(0.0f,-0.90f,0.4f);
			glRotatef(-90,1,0,0);
			//발
			glBindTexture(GL_TEXTURE_2D, GetTexturePool()[FOOT]);
			myCapsule(0.18,0.2,0.15,0.4f);
		glPopMatrix();
	glPopMatrix();




}

GLvoid mySphere(double rx, double ry, double rz) 
{
   int i,j;
   int n;
   double theta1,theta2,theta3;
   Vector3 e,p;

   n = 20;
   j=0;
   
for (j=0;j<n/2;j++) 
   {
      theta1 = j * PI*2.0 / n - PI*0.5;
      theta2 = (j + 1) * PI*2.0 / n - PI*0.5;

      glBegin(GL_QUAD_STRIP);
      for (i=n;i>=0;i--) {
         theta3 = i * PI*2.0 / n - PI * 0.5;

         e.x = cos(theta2) * cos(theta3);
         e.y = sin(theta2);
         e.z = cos(theta2) * sin(theta3);
         p.x = rx * e.x;
         p.y = ry * e.y;
         p.z = rz * e.z;


         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(theta1) * cos(theta3);
         e.y = sin(theta1);
         e.z = cos(theta1) * sin(theta3);
         p.x = rx * e.x;
         p.y = ry * e.y;
         p.z = rz * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*j/(double)n);
         glVertex3f(p.x,p.y,p.z);
	  }
      glEnd();
   }
}

GLvoid myCapsule(double rx, double ry, double rz, double ty) 
{
   int i,j;
   int n;
   double theta1,theta2,theta3;
   Vector3 e,p;

   double tty = ry*2.f/(ty + ry*2.f);
   double invty = 1.0 - tty;
   double factor = 0.0f;
   double yfactor = 0.0f;
   n = 20;
   j=0;
   
	for (j=0;j<n/2;j++) 
   {
      theta1 = j * PI*2.0 / n - PI*0.5;
      theta2 = (j + 1) * PI*2.0 / n - PI*0.5;
	  
	  if( j == n/4)
	  {
			factor = ty;
			yfactor = ty;
		  glBegin(GL_QUAD_STRIP);
		  for (i=n;i>=0;i--) {
			  theta3 = i * PI*2.0 / n - PI * 0.5;

			  e.x = cos(theta2) * cos(theta3);
			  e.y = sin(theta2);
			  e.z = cos(theta2) * sin(theta3);
			  p.x = rx * e.x;
			  p.y = ry * e.y + yfactor;
			  p.z = rz * e.z;


			  glNormal3f(e.x,e.y,e.z);
			  glTexCoord2f(i/(double)n,2*(j+1)/(double)n * invty + factor);
			  glVertex3f(p.x,p.y,p.z);

			  e.x = cos(theta1) * cos(theta3);
			  e.y = sin(theta1);
			  e.z = cos(theta1) * sin(theta3);
			  p.x = rx * e.x;
			  p.y = ry * e.y;
			  p.z = rz * e.z;

			  glNormal3f(e.x,e.y,e.z);
			  glTexCoord2f(i/(double)n,2*j/(double)n * invty);
			  glVertex3f(p.x,p.y,p.z);
		  }
		  glEnd();
		  continue;
	  }

      glBegin(GL_QUAD_STRIP);
      for (i=n;i>=0;i--) {
         theta3 = i * PI*2.0 / n - PI * 0.5;

         e.x = cos(theta2) * cos(theta3);
         e.y = sin(theta2);
         e.z = cos(theta2) * sin(theta3);
         p.x = rx * e.x;
         p.y = ry * e.y + yfactor;
         p.z = rz * e.z;


         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n * invty + factor);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(theta1) * cos(theta3);
         e.y = sin(theta1);
         e.z = cos(theta1) * sin(theta3);
         p.x = rx * e.x;
         p.y = ry * e.y + yfactor;
         p.z = rz * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*j/(double)n * invty + factor);
         glVertex3f(p.x,p.y,p.z);
	  }
      glEnd();
   }
}

GLvoid myDrawCube(double rx, double ry, double rz) 
{ 
        glBegin(GL_QUADS); 
        // Front Face 
        glNormal3f( 0.0f, 0.0f, 0.5f);                   
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rx, -ry,  rz); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( rx, -ry,  rz); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f( rx,  ry,  rz); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rx,  ry,  rz); 
        // Back Face 
        glNormal3f( 0.0f, 0.0f,-0.5f);                   
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-rx, -ry, -rz); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-rx,  ry, -rz); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f( rx,  ry, -rz); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f( rx, -ry, -rz); 
        // Top Face 
        glNormal3f( 0.0f, 0.5f, 0.0f);                   
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rx,  ry, -rz); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rx,  ry,  rz); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( rx,  ry,  rz); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f( rx,  ry, -rz); 
        // Bottom Face 
        glNormal3f( 0.0f,-0.5f, 0.0f);                   
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-rx, -ry, -rz); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f( rx, -ry, -rz); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f( rx, -ry,  rz); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-rx, -ry,  rz); 
        // Right Face 
        glNormal3f( 0.5f, 0.0f, 0.0f);                   
        glTexCoord2f(1.0f, 0.0f); glVertex3f( rx, -ry, -rz); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f( rx,  ry, -rz); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f( rx,  ry,  rz); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f( rx, -ry,  rz); 
        // Left Face 
        glNormal3f(-0.5f, 0.0f, 0.0f);                   
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-rx, -ry, -rz); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-rx, -ry,  rz); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-rx,  ry,  rz); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-rx,  ry, -rz); 
    glEnd(); 
}