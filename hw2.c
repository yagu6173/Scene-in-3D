/*
 *  Projections
 *
 *  HW2: Draw some objects to demonstrate orthogonal, prespective and first-person projections
 *
 *  Key bindings:
 *  m/M          Toggle between orthogonal, perspective and first-person views 
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

int axes=1;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int step=0;       //  Step counter for first person movement
int t = 0;        //  Step direction for first person movement
int alpha=90;     //  First person view angle
int fov=55;      //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
const char* text[] = {"Orthogonal","Perspective","First Person"};
double FV_Ex = 0.0;
double FV_Ey = -3.0;
double FV_Ez = 0.5;

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.14159265/180*(th))
#define Sin(th) sin(3.14159265/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode == 0)
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Orthogonal projection
   else if (mode == 1)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  First person view
   else if  (mode == 2)
      gluPerspective(fov,asp,dim/4,4*dim);

   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

// Draw a land of the world at the center of the world with dimensions (dx,dy,dz)

static void land(double dx, double dy, double dz)
{
   // Draw Grass
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(0,0,0);
   // glRotated(th,0,0,1);
   glScaled(dx,dy,dz);

   //  Grass
   glColor3f(0.0,0.7,0.1);
   glBegin(GL_QUADS);
   // Draw vertexes
   glVertex3f(-1,-1,0);
   glVertex3f(+1,-1,0);
   glVertex3f(+1,+1,0);
   glVertex3f(-1,+1,0);
   //  End
   glEnd();
   //  Undo transofrmations

   //  River
   glDisable(GL_DEPTH_TEST);
   glBegin(GL_QUADS);
   // set color to light blue
   glColor3f(0,0.6,1);
   // Draw vertexes
   glVertex3f(-1,-0.1,0);
   glVertex3f(+1,-0.1,0);
   glVertex3f(+1,+0.1,0);
   glVertex3f(-1,+0.1,0);
   //  End
   glEnd();

   // Water over the river
   glBegin(GL_LINES);
   // set color to white
   glColor3f(1,1,1);
   glVertex3f(-0.5,0.05,0);
   glVertex3f(-0.4,0.05,0);
   glVertex3f(-0.9,-0.05,0);
   glVertex3f(-0.8,-0.05,0);
   glVertex3f(0.5,0.05,0);
   glVertex3f(0.4,0.05,0);
   glVertex3f(0.9,-0.05,0);
   glVertex3f(0.8,-0.05,0);
   glEnd();

   //  Road
   glDisable(GL_DEPTH_TEST);
   glBegin(GL_QUADS);
   // set color to grey
   glColor3f(0.5,0.5,0.5);
   // Draw vertexes
   glVertex3f(-0.1,-1,0);
   glVertex3f(+0.1,-1,0);
   glVertex3f(+0.1,+1,0);
   glVertex3f(-0.1,+1,0);
   //  End
   glEnd();

   // Draw double yellow lines to separate lanes
   glBegin(GL_LINES);
   // set color to yellow
   glColor3f(1,1,0);
   glVertex3f(-0.02,-1,0);
   glVertex3f(-0.02,1,0);
   glVertex3f(0.02,-1,0);
   glVertex3f(0.02,1,0);
   glEnd();
   
   glEnable(GL_DEPTH_TEST);
   //  Undo transofrmations
   glPopMatrix();

}

static void house(double x,double y,double z, double dx,double dy,double dz, double th) {
   //  Save transformation
   glPushMatrix();

   //  Transform house
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);

   //  Draw Walls
   glBegin(GL_QUADS);
   //  Right - yellow
   glColor3f(1,1,0);
   glVertex3f(+1,-1,2);
   glVertex3f(+1,-1,0);
   glVertex3f(+1,+1,0);
   glVertex3f(+1,+1,2);
   //  Left - dark green
   glColor3f(0,0.3,0);
   glVertex3f(-1,-1,0);
   glVertex3f(-1,-1,2);
   glVertex3f(-1,+1,2);
   glVertex3f(-1,+1,0);
   //  Back - cyan
   glColor3f(0,1,1);
   glVertex3f(-1,+1,2);
   glVertex3f(+1,+1,2);
   glVertex3f(+1,+1,0);
   glVertex3f(-1,+1,0);

   //  Front - magenta
   glColor3f(1,0,1);
   glVertex3f(-1,-1,2);
   glVertex3f(+1,-1,2);
   glVertex3f(+1,-1,0);
   glVertex3f(-1,-1,0);

   glEnd();

   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1, -1); 
   // Draw a door - white
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glVertex3f(-0.6,-1,1);
   glVertex3f(0.1,-1,1);
   glVertex3f(0.1,-1,0);
   glVertex3f(-0.6,-1,0);
   glEnd();

   // Draw a window with - light blue
   glBegin(GL_QUADS);
   glColor3f(0.5,0.8,1);
   glVertex3f(0.3,-1,1.6);
   glVertex3f(0.7,-1,1.6);
   glVertex3f(0.7,-1,1.2);
   glVertex3f(0.3,-1,1.2);
   glEnd();
   
   glDisable(GL_POLYGON_OFFSET_FILL);

   //  Draw Roof
   glBegin(GL_TRIANGLES);
   // Right - yellow
   glColor3f(1,1,0);
   glVertex3f(0,0,3);
   glVertex3f(1,1,2);
   glVertex3f(1,-1,2);

   // Left - dark green
   glColor3f(0,0.3,0);
   glVertex3f(0,0,3);
   glVertex3f(-1,1,2);
   glVertex3f(-1,-1,2);

   // Back - cyan
   glColor3f(0,1,1);
   glColor3f(1,0,1);
   glVertex3f(0,0,3);
   glVertex3f(1,-1,2);
   glVertex3f(-1,-1,2);

   // Front - magenta
   glColor3f(0,1,1);
   glVertex3f(0,0,3);
   glVertex3f(1,1,2);
   glVertex3f(-1,1,2);

   glEnd();


   //  Undo transformations
   glPopMatrix();
}


// Draw a Tree
static void tree(double x, double y, double z, double s)
{
   //  Save current transforms
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(s,s,s);

   // Draw Trunk
   // glDisable(GL_DEPTH_TEST);

   glColor3f(0.5,0.25,0);
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=360;th+=20)
   {
      glVertex3d(0.1*Cos(th), 0.1*Sin(th),0.0);  // bottom circle
      glVertex3d(0.1*Cos(th), 0.1*Sin(th),1.0);  // top circle
   }
   glEnd();

   // glLoadIdentity();
   glTranslated(0, 0,1);   // move to top of trunk
   glColor3f(0,1,0);

   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0, 0, 1);     // tip
   for (int th=0;th<=360;th+=20)
   {
      glVertex3d(0.5*Cos(th), 0.5*Sin(th),0);  // base circle
   }
   glEnd();

   glPopMatrix();
}

// // Draw a car
// static void car(double x, double y, double z, double s)
// {
// }

/*
 *  Draw solid airplane
 *    at (x,y,z)
 *    nose towards (dx,dy,dz)
 *    up towards (ux,uy,uz)
 *    size
 */
static void SolidPlane(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz,
                       double s)
{
   // Dimensions used to size airplane
   const double wid=0.05;
   const double nose=+0.50;
   const double cone= 0.20;
   const double wing= 0.00;
   const double strk=-0.20;
   const double tail=-0.50;
   //  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double mat[16];
   mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
   mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
   mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
   mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

   //  Save current transforms
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glMultMatrixd(mat);
   glScaled(s,s,s);
   //  Nose
   glColor3f(0,0,1);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(nose, 0.0, 0.0);
   for (int th=0;th<=360;th+=30)
      glVertex3d(cone,wid*Cos(th),wid*Sin(th));
   glEnd();
   //  Fuselage
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=360;th+=30)
   {
      glVertex3d(cone,wid*Cos(th),wid*Sin(th));
      glVertex3d(tail,wid*Cos(th),wid*Sin(th));
   }
   glEnd();
   // Tailpipe
   glColor3f(1,0.8,0);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(tail, 0.0, 0.0);
   for (int th=0;th<=360;th+=30)
      glVertex3d(tail,wid*Cos(th),wid*Sin(th));
   glEnd();
   // //  Canopy
   // glPushMatrix();
   // glTranslated(0.15,wid,0);
   // glScaled(0.05,0.03,0.03);
   // glColor3f(1,1,1);
   // for (int ph=-30;ph<90;ph+=30)
   // {
   //    glBegin(GL_QUAD_STRIP);
   //    for (int th=0;th<=360;th+=30)
   //    {
   //       Canopy(th,ph);
   //       Canopy(th,ph+30);
   //    }
   //    glEnd();
   // }
   // glPopMatrix();
   //  Wings
   glColor3f(1,1,0);
   glBegin(GL_TRIANGLES);
   glVertex3d(wing, 0.0, wid);
   glVertex3d(tail, 0.0, wid);
   glVertex3d(tail, 0.0, 0.5);

   glVertex3d(wing, 0.0,-wid);
   glVertex3d(tail, 0.0,-wid);
   glVertex3d(tail, 0.0,-0.5);
   glEnd();
   //  Vertical tail
   glColor3f(1,0,0);
   glBegin(GL_TRIANGLES);
   glVertex3d(strk, wid, 0.0);
   glVertex3d(tail, 0.3, 0.0);
   glVertex3d(tail, wid, 0.0);
   glEnd();
   //  Undo transformations
   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal - set world orientation
   if (mode==0)
   {
      glRotatef(ph,1,0,0); // Rotate around x-axis (elevation)
      glRotatef(th,0,1,0); // Rotate around y-axis (azimuth)
   }
   //  Perspective - set eye position
   else if (mode==1)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   // First person - set eye position and orientation
   else if (mode==2)
   {
      if (step !=0 && t!=0){
         double dx = t*Cos(alpha)*0.1;
         double dy = t*Sin(alpha)*0.1;
         FV_Ex += dx;
         FV_Ey += dy;
         t = 0;
      }
      gluLookAt(FV_Ex,FV_Ey,FV_Ez, FV_Ex+Cos(alpha), FV_Ey+Sin(alpha), FV_Ez+Sin(ph), 0, 0, 1);     
   }

   // Draw a land
   land(3,3,3);

   // Draw a solid airplane
   SolidPlane(-1,1,3 , 1,0,0 , 0,0,1 , 1);

   // Draw houses
   house(1,1,0 , 0.5,0.5,0.5 , 0);
   house(-2,-1,0 , 0.3,0.2,0.5,90);

   // Draw trees
   tree(-2,-2,0, 0.5);
   tree(1,2,0, 0.4);
   tree(1,-2,0, 0.8);
   tree(-1,1,0, 0.2);

   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      const double len=4.5;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5,5);
   if (mode == 2)
      Print("Angle=%d,%d  FOV=%d Projection=%s",ph,alpha,fov,text[mode]);
   else
      Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,text[mode]);

      //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT){
      th += 5;
      alpha -= 5;}
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT){
      th -= 5;
      alpha += 5;}
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;

   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   alpha %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = step = FV_Ex = FV_Ey = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == 'm' || ch == 'M')
      mode = (mode+1)%3;
   else if (ch == 'f' || ch == 'F'){
      t = +1;
      step = 1;
   }
   else if (ch == 'b' || ch == 'B'){
      t = -1;
      step = 1;
   }

   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("HW2: Yan Gu");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
