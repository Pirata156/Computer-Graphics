#include <cstdio>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <cmath>

using namespace std;

class Global {
public:
	/* Camera variables */
	float camera_radius;        // distance from the center
	float camera_alpha;         // angle for the XZ positions
	float camera_beta;          // angle for the height
	float camX;                 // position XX of the camera
	float camY;                 // position YY of the camera
	float camZ;                 // position ZZ of the camera

    /* Random seed */
    unsigned int random_seed;   // seed for the pseudo-random generator

    /* Map sizes */
    float radius;               // radius of the no trees area
    float outter_radius;        // outer radius - for the indies
    float inner_radius;         // inner radius - for the cowboys
    float tree_size;            // height of the trees
    int number_trees;           // how many trees you want to draw

	Global() {
		camera_radius = 100.0f;
		camera_alpha = 0.0f;
		camera_beta = 0.5f;
		camX = 0.0f;
		camY = 0.0f;
		camZ = 0.0f;

		random_seed = 4;

		radius = 50.0f;
		outter_radius = 35.0f;
		inner_radius = 15.0f;
		tree_size = 5.0f;
		number_trees = 250;
	}
};

Global global;

void spherical2Cartesian() {

	global.camX = global.camera_radius * cosf(global.camera_beta) * sinf(global.camera_alpha);
	global.camY = global.camera_radius * sinf(global.camera_beta);
	global.camZ = global.camera_radius * cosf(global.camera_beta) * cosf(global.camera_alpha);
}


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

/* Drawing */
void drawFloor() {
	glColor3f(0.2f, 0.8f, 0.2f);
	glBegin(GL_TRIANGLES);
		glVertex3f(100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, 100.0f);

		glVertex3f(100.0f, 0, -100.0f);
		glVertex3f(-100.0f, 0, 100.0f);
		glVertex3f(100.0f, 0, 100.0f);
	glEnd();
}

/* Draw a tree */
void drawTree(float size) {
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	glColor3f(0.7f, 0.3f, 0.1f);
	glutSolidCone(0.1f * size, size, 20, 10);

	glTranslatef(0.0f, 0.0f, 0.25f * size);
	glColor3f(0.0f, 0.6f, 0.2f);
	glutSolidCone(0.25f * size, 0.75f * size, 20, 10);
	glPopMatrix();
}

void plantTrees(int num_trees) {
    float x, z;
    srand(global.random_seed);

    while(num_trees > 0) {
        x = (rand() % 199) - 100;
        z = (rand() % 199) - 100;

        if(sqrtf(x * x + z * z) > global.radius) {
            num_trees--;
            glPushMatrix();
            glTranslatef(x, 0.0f, z);
            drawTree(global.tree_size);
            glPopMatrix();
        }
    }
}

void cowboysProtecting() {
    float angle = 360.0f / 8.0f;
    int i;
    glColor3f(0.0f, 0.3f, 1.0f);
    for(i = 0; i < 8; i++) {
        glPushMatrix();
        glRotatef(i * angle, 0.0f, 1.0f, 0.0f);
        glTranslatef(global.inner_radius, 1.0f, 0.0f);
        glutSolidTeapot(1.0f);
        glPopMatrix();
    }
}

void indiansAttacking() {
    float angle = 360.0f / 16.0f;
    int i;
    glColor3f(1.0f, 0.3f, 0.0f);
    for(i = 0; i < 16; i++) {
        glPushMatrix();
        glRotatef((i * angle) - 90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 1.0f, global.outter_radius);
        glutSolidTeapot(1.0f);
        glPopMatrix();
    }
}

void renderScene() {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(global.camX, global.camY, global.camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);
    /* The floor - terrain */
	drawFloor();
	/* The central torus */
	glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidTorus(0.5f, 1.5f, 5, 20);
    /* The random trees */
	plantTrees(global.number_trees);
    /* Cowboys in the inner radius */
    cowboysProtecting();
    /* Indians in the outter radius */
    indiansAttacking();
	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {
	// put code to process regular keys in here
}


void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		global.camera_alpha -= 0.1;
		break;

	case GLUT_KEY_LEFT:
		global.camera_alpha += 0.1;
		break;

	case GLUT_KEY_UP:
		global.camera_beta += 0.1f;
		if (global.camera_beta > 1.5f)
			global.camera_beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		global.camera_beta -= 0.1f;
		if (global.camera_beta < -1.5f)
			global.camera_beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN:
		global.camera_radius -= 1.0f;
		if (global.camera_radius < 1.0f)
			global.camera_radius = 1.0f;
		break;

	case GLUT_KEY_PAGE_UP:
		global.camera_radius += 1.0f;
		break;

	default:
		break;
	}
	spherical2Cartesian();
	glutPostRedisplay();

}


void printInfo() {
	cout << "Vendor: " << glGetString(GL_VENDOR) << endl
        << "Renderer: " << glGetString(GL_RENDERER) << endl
	    << "Version: " << glGetString(GL_VERSION) << endl;

	cout << "\nUse Arrows to move the camera up/down and left/right\n"
	    << "Home and End control the distance from the camera to the origin" << endl;
}


int main(int argc, char **argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1400,800);
	glutCreateWindow("CG@DI - Assessment class 5");
		
	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	spherical2Cartesian();

	printInfo();

	// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
