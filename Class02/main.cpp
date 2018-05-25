#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <iostream>

using namespace std;

/* Setting all global variables inside a structure as to understand where they are global or local variables */
class Global {
    public:
    float translation[3];     // ( X , Y , Z )
    float rotation[4];        // (angle, vX, vY, vZ)
    //float scale[3];         // ( sX, sY, sZ)
    float camera_height;

    int front;
    int back;
    int cull_face;
    int win;

    Global() {
        translation[0] = translation[1] = translation[2] = 0.0f;
        rotation[0] = rotation[1] = rotation[3] = 0.0f; rotation[2] = 1.0f;
        //scale[0] = scale[1] = scale[2] = 1.0f;
        camera_height = 2.0f;

        front = back = cull_face = 0;
        win = 0;
    }
};

Global global;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0f / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawPiramide() {
    /* Triangles must be drawn counter-clockwise from the direction we want them to be seen */
    glBegin(GL_TRIANGLES);
    /* Drawing the base: 1 x 1 square = 2 triangles
     * color: GREY*/
        glColor3f( 0.5f, 0.5f, 0.5f);
        glVertex3f(-1.0f, 0.0f,-1.0f);
        glVertex3f( 1.0f, 0.0f,-1.0f);
        glVertex3f( 1.0f, 0.0f, 1.0f);
        glVertex3f( 1.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f,-1.0f);
    /* Drawing the front side
     * color: RED */
        glColor3f( 1.0f, 0.0f, 0.0f);
        glVertex3f( 0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f, 1.0f);
        glVertex3f( 1.0f, 0.0f, 1.0f);
    /* Drawing the left side - right from front view
     * color: GREEN */
        glColor3f( 0.0f, 1.0f, 0.0f);
        glVertex3f( 0.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f, 0.0f, 1.0f);
        glVertex3f( 1.0f, 0.0f,-1.0f);
    /* Drawing the back side
     * color: BLUE */
        glColor3f( 0.0f, 0.0f, 1.0f);
        glVertex3f( 0.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f, 0.0f,-1.0f);
        glVertex3f(-1.0f, 0.0f,-1.0f);
    /* Drawing the right side - left from front view
     * color: YELLOW */
        glColor3f( 1.0f, 1.0f, 0.0f);
        glVertex3f( 0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f,-1.0f);
        glVertex3f(-1.0f, 0.0f, 1.0f);
    glEnd();
}

/* This funciton serves only to print the reference lines of the center of the world */
void printReference(float size) {
    glBegin(GL_LINES);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(size, 0.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, size, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, size);
    glEnd();
}

void renderScene() {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(0.0,global.camera_height,5.0,
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	/* Printing reference lines in the center of the world */
	printReference(2.0f);
	// put the geometric transformations here
    glTranslatef(global.translation[0], global.translation[1], global.translation[2]);
    glRotatef(global.rotation[0], global.rotation[1], global.rotation[2], global.rotation[3]);
    //glScalef(global.scale[0], global.scale[1], global.scale[2]);

	// put drawing instructions here
    drawPiramide();

	// End of frame
	glutSwapBuffers();
}



	// write function to process keyboard events
void processNormalKeys(unsigned char key, int x, int y){
    switch (key) {
        case 'a':
            global.rotation[0] -= 4.5f; break;
        case 'd':
            global.rotation[0] += 4.5f; break;
        case 'w':
            global.camera_height += 0.1f; break;
        case 's':
            global.camera_height -= 0.1f; break;

        default:
            break;
    }
    /* Redrawing scene after changing values */
    glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y){
    switch (key) {
        case GLUT_KEY_UP:
            global.translation[2] -= 0.1f; break;
        case GLUT_KEY_DOWN:
            global.translation[2] += 0.1f; break;
        case GLUT_KEY_LEFT:
            global.translation[0] -= 0.1f; break;
        case GLUT_KEY_RIGHT:
            global.translation[0] += 0.1f; break;

        default:
            break;
    }
    /* Redrawing scene after changing values */
    glutPostRedisplay();
}

void menu(int op){
    switch(op) {
        case 0:
            /* destroying the window and exiting */
            glutDestroyWindow(global.win);
            exit(0);
        case 1:
            glPolygonMode(GL_FRONT, GL_POINT); break;
        case 2:
            glPolygonMode(GL_FRONT, GL_LINE); break;
        case 3:
            glPolygonMode(GL_FRONT, GL_FILL); break;
        case 4:
            glPolygonMode(GL_BACK, GL_POINT); break;
        case 5:
            glPolygonMode(GL_BACK, GL_LINE); break;
        case 6:
            glPolygonMode(GL_BACK, GL_FILL); break;
        case 7:
            glEnable(GL_CULL_FACE); break;
        case 8:
            glDisable(GL_CULL_FACE); break;
        case 9:
            glCullFace(GL_FRONT); break;
        case 10:
            glCullFace(GL_BACK); break;

        default:
            break;
    }
    /* Redrawing scene after changing values */
    glutPostRedisplay();
}

void createMenu() {
    /* Create a submenu */
    global.front = glutCreateMenu(menu);

    /* Add submenu entries */
    glutAddMenuEntry("Point",1);
    glutAddMenuEntry("Line",2);
    glutAddMenuEntry("Fill",3);

    /* Create a submenu */
    global.back = glutCreateMenu(menu);

    /* Add submenu entries */
    glutAddMenuEntry("Point",4);
    glutAddMenuEntry("Line",5);
    glutAddMenuEntry("Fill",6);

    /* Create a submenu */
    global.cull_face = glutCreateMenu(menu);

    /* Add submenu entries */
    glutAddMenuEntry("On",7);
    glutAddMenuEntry("Off",8);
    glutAddMenuEntry("Front",9);
    glutAddMenuEntry("Back",10);

    /* Create a menu - becomes current menu */
    glutCreateMenu(menu);
    glutAddSubMenu("Front", global.front);
    glutAddSubMenu("Back", global.back);
    glutAddSubMenu("Cull Face", global.cull_face);
    glutAddMenuEntry("Exit",0);

    /* Let the menu responde to the right mouse button */
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1400,800);
	global.win = glutCreateWindow("CG@DI - Assessment class 2");
		
	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard callbacks
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

    /* Putting all the menu procedures in a single procedure */
    createMenu();

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
