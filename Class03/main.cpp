#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// #define _USE_MATH_DEFINES
#include <cmath>

#define MOTION 0.2f

/* Setting all global variables inside a structure as to understand where they are global or local variables */
class Global {
public:
    float camera_radius;        // distance from the center
    float camera_alpha;         // angle for the XZ positions
    float camera_beta;          // angle for the height

    GLenum polygon_mode;        // drawing option - lines by default

    /* mouse click and motion */
    GLenum left_button;         // state of the mouse left button
    GLenum right_button;        // state of the mouse right button
    int screenX;                // x position of the mouse in the screen
    int screenY;                // y position of the mouse in the screen
    float motionX;              // how much the mouse moved in horizontal since pressed down
    float motionY;              // how much the mouse moved in vertical since pressed down

    Global() {
        camera_radius = 5.0f;
        camera_alpha = 90.0f;
        camera_beta = 20.0f;

        polygon_mode = GL_LINE;

        left_button = GLUT_UP;
        right_button = GLUT_UP;
        screenX = 0;
        screenY = 0;
        motionX = 0.0f;
        motionY = 0.0f;
    }
};

Global global;

/* Takes Polar Coordinates (degrees) and returns the resulting Cartesian Coordinates in a 2 element array */
float* polarToCartesian(float alpha, float radius) {
    auto res = (float*) malloc(2 * sizeof(float));
    /* passing degrees to radians */
    float a = alpha * static_cast<float>(M_PI_2) / 90.0f;
    res[0] = radius * cosf(a);
    res[1] = radius * sinf(a);
    return res;
}

/* Takes Spherical Coordinates (degrees) and returns the resulting Cartesian Coordinates in a 3 element array.
 * beta must be between -90º and 90º (there's no verification) */
float* sphericalToCartesian(float alpha, float beta, float radius) {
    auto res = (float*) malloc(3 * sizeof(float));
    /* passing degrees to radians
     * radians = degrees * (PI / 2) / 90º */
    float b = beta * static_cast<float>(M_PI_2) / 90.0f;
    float* p2c = polarToCartesian(alpha, radius * cosf(b));
    res[0] = p2c[0];                // == radius * cosf(b) * sinf(a);
    res[1] = radius * sinf(b);
    res[2] = p2c[1];                // == radius * cosf(b) * cosf(a);
    free(p2c);
    return res;
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

void drawCylinder(float radius, float height, int slices) {
    int i = 0;
    float centerTop[3];
    float centerBottom[3];
    float divAngle = 360.0f / (1.0f * slices);
    float * pos1 = nullptr, * pos2 = nullptr;

    // put code to draw cylinder in here
    centerTop[0] = 0.0f; centerTop[1] = height / 2.0f; centerTop[2] = 0.0f;
    centerBottom[0] = 0.0f; centerBottom[1] = -1.0f * height / 2.0f; centerBottom[2] = 0.0f;

    glBegin(GL_TRIANGLES);
    for(i = 0; i < slices; i++) {
        pos1 = polarToCartesian(i * divAngle, radius);
        pos2 = polarToCartesian((i + 1) * divAngle, radius);
        /* Top triangle */
        glColor3f(0.0f, 0.0f, 1.0f);        // Top in blue
        glVertex3f(centerTop[0], centerTop[1], centerTop[2]);
        glVertex3f(pos2[0], centerTop[1], pos2[1]);
        glVertex3f(pos1[0], centerTop[1], pos1[1]);

        /* Side square */
        glColor3f(0.8f, 0.8f, 0.0f);        // Half square in dark yellow
        glVertex3f(pos1[0], centerTop[1], pos1[1]);
        glVertex3f(pos2[0], centerTop[1], pos2[1]);
        glVertex3f(pos2[0], centerBottom[1], pos2[1]);
        glColor3f(1.0f, 1.0f, 0.0f);        // Half square in yellow
        glVertex3f(pos2[0], centerBottom[1], pos2[1]);
        glVertex3f(pos1[0], centerBottom[1], pos1[1]);
        glVertex3f(pos1[0], centerTop[1], pos1[1]);

        /* Bottom triangle */
        glColor3f(0.0f, 1.0f, 0.0f);        // Bottom in green
        glVertex3f(centerTop[0], centerBottom[1], centerTop[2]);
        glVertex3f(pos1[0], centerBottom[1], pos1[1]);
        glVertex3f(pos2[0], centerBottom[1], pos2[1]);
        free(pos1);
        free(pos2);
    }
    glEnd();
}


void renderScene() {
    float* pos = nullptr;
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
    if(global.left_button)
        pos = sphericalToCartesian(global.camera_alpha + global.motionX, global.camera_beta + global.motionY, global.camera_radius);
    else {
        if(global.right_button) {
            pos = sphericalToCartesian(global.camera_alpha, global.camera_beta, global.camera_radius + global.motionY);
        } else {
            pos = sphericalToCartesian(global.camera_alpha, global.camera_beta, global.camera_radius);
        }
    }
	glLoadIdentity();
	gluLookAt(pos[0],pos[1],pos[2],
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);
    free(pos);

    glPolygonMode(GL_FRONT_AND_BACK, global.polygon_mode);

    /* Printing reference lines in the center of the world */
    printReference(2.0f);

	drawCylinder(1,2,10);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

	// put code to process regular keys in here
    switch (c) {
        /* Camera horizontal movement */
        case 'a':
            global.camera_alpha -= 4.5f; break;
        case 'd':
            global.camera_alpha += 4.5f; break;
            /* Camera vertical movement */
        case 'w':
            global.camera_beta += 3.5f; break;
        case 's':
            global.camera_beta -= 3.5f; break;
            /* Drawing options */
        case 'f':
            global.polygon_mode = GL_FILL; break;
        case 'g':
            global.polygon_mode = GL_LINE; break;
        case 'h':
            global.polygon_mode = GL_POINT; break;

        default:
            break;
    }
    /* Redrawing scene after changing values */
    glutPostRedisplay();

}

/* Special keys */
void processSpecialKeys(int key, int xx, int yy) {

	// put code to process special keys in here
    switch (key) {
        case GLUT_KEY_UP:
            /* Zoom in */
            global.camera_radius -= 0.1f; break;
        case GLUT_KEY_DOWN:
            /* Zoom out */
            global.camera_radius += 0.1f; break;

        default:
            break;
    }
    /* Redrawing scene after changing values */
    glutPostRedisplay();
}

/* Mouse buttons actions */
void mouse(int button, int state, int xx, int yy) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN) {
                /* Left button pressed down, saving where was clicked */
                global.left_button = 1;
                global.screenX = xx;
                global.screenY = yy;
            } else {
                //GLUT_UP
                /* Left button released, so keeping the camera in current position */
                global.left_button = 0;
                global.camera_alpha += global.motionX;
                global.camera_beta += global.motionY;
                global.motionX = 0.0f;
                global.motionY = 0.0f;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN) {
                /* Right button pressed down, saving where was clicked */
                global.right_button = 1;
                global.screenX = xx;
                global.screenY = yy;
            } else {
                //GLUT_UP
                /* Right button released, changing the camera radius to the current one */
                global.right_button = 0;
                global.camera_radius += global.motionY;
                global.motionY = 0.0f;
            }
            break;
        default:
            break;
    }

}

/* Mouse motion control */
void motion(int xx, int yy) {
    float test;
    if(global.left_button) {
        if(global.screenX != xx)
            global.motionX = (xx - global.screenX) * MOTION;
        if(global.screenY != yy) {
            /* Needing to check that beta angle always between [-90, 90] */
            test = (yy - global.screenY) * MOTION;
            if (((test + global.camera_beta) > -90) && ((test + global.camera_beta) < 90))
                global.motionY = test;
        }
    } else {
        if(global.right_button) {
            if(global.screenY != yy) {
                /* Needing to check that radius is always bigger then 0 */
                test = (yy - global.screenY) * (MOTION / 5.0f);         // Just to move slower
                if((test + global.camera_radius) > 0)
                    global.motionY = test;
            }
        }
    }
    glutPostRedisplay();
}


int main(int argc, char **argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1400,800);
	glutCreateWindow("CG@DI - Assessment class 3");
		
	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}