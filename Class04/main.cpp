#include <cstdio>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

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

    /* Cylinder sizes */
    float cRadius;              // radius of the cylinder
    float cHeight;              // height of the cylinder
    int cSides;                 // number of sides of cylinder

    /* VBOs */
    GLuint buffer[2]{};         // to save the array of points
    GLuint indexes[1]{};        // to save the array of indexes
    int n_points_VBO;           // to save the number of points of the array without indexes
    int n_points_Index;         // to save the number of points of the array with index
    int n_indexes;              // to save the number ot indexes written to the array

    /* Drawing Mode */
    int draw_mode;              // what way to draw the Cylinder: 0 - direct; 1 - fan & stripes; 2 - VBO; 3 - with Indexes;

    /* FPS counter */
    int time;
    int timebase;
    int frame;                  // number of frames
    float fps;                  // fps value after calculating
    char print[40]{};           // ready to print value of fps

    Global() {
        camera_radius = 5.0f;
        camera_alpha = 0.0f;
        camera_beta = 0.0f;
        camX = 0.0f;
        camY = 0.0f;
        camZ = 0.0f;

        cRadius = 1.0f;
        cHeight = 2.0f;
        cSides = 25;

        n_points_VBO = 4 * cSides * 3 * 3;
        n_points_Index = (cSides + 1) * 2 * 3;
        n_indexes = 4 * cSides * 3;

        draw_mode = 0;

        time = 0;
        timebase = 0;
        frame = 0;
        fps = 0.0f;
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

void spherical2Cartesian() {

	global.camX = global.camera_radius * cosf(global.camera_beta) * sinf(global.camera_alpha);
    global.camY = global.camera_radius * sinf(global.camera_beta);
    global.camZ = global.camera_radius * cosf(global.camera_beta) * cosf(global.camera_alpha);
}

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


/*-----------------------------------------------------------------------------------
	Draw Cylinder with strips and fans

	  parameters: radius, height, sides

-----------------------------------------------------------------------------------*/


void cylinder0(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0f/sides;

    glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0, height * 0.5f,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cosf(i * step * static_cast<float>(M_PI)/180.0f) * radius, height * 0.5f, -sinf(i * step * static_cast<float>(M_PI)/180.0f) * radius);
		}
	glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0,-height*0.5f,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cosf(i * step * static_cast<float>(M_PI)/180.0f) * radius, -height * 0.5f,sinf(i * step * static_cast<float>(M_PI)/180.0f) * radius);
		}
	glEnd();

    glColor3f(0.0f, 0.5f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);

		for (i=0; i <= sides; i++) {
			glVertex3f(cosf(i * step * static_cast<float>(M_PI)/180.0f) * radius, height * 0.5f,-sinf(i * step * static_cast<float>(M_PI)/180.0f) * radius);
			glVertex3f(cosf(i * step * static_cast<float>(M_PI)/180.0f) * radius,-height * 0.5f,-sinf(i * step * static_cast<float>(M_PI)/180.0f) * radius);
		}
	glEnd();
}


/*-----------------------------------------------------------------------------------
	Draw Cylinder in Direct Mode

		parameters: radius, height, sides

-----------------------------------------------------------------------------------*/


void cylinder(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0f / sides;

	glBegin(GL_TRIANGLES);

	// top
    glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < sides; i++) {
		glVertex3f(0, height * 0.5f, 0);
		glVertex3f(cosf(i * step * static_cast<float>(M_PI) / 180.0f) * radius, height * 0.5f, -sinf(i * step * static_cast<float>(M_PI) / 180.0f) * radius);
		glVertex3f(cosf((i+1) * step * static_cast<float>(M_PI) / 180.0f) * radius, height * 0.5f, -sinf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius);
	}

	// bottom
    glColor3f(0.0f, 1.0f, 0.0f);
	for (i = 0; i < sides; i++) {
		glVertex3f(0, -height * 0.5f, 0);
		glVertex3f(cosf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius, -height * 0.5f, -sinf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius);
		glVertex3f(cosf(i * step * static_cast<float>(M_PI) / 180.0f) * radius, -height * 0.5f, -sinf(i * step * static_cast<float>(M_PI) / 180.0f) * radius);
	}

	// body
    glColor3f(0.0f, 0.5f, 1.0f);
	for (i = 0; i <= sides; i++) {

		glVertex3f(cosf(i * step * static_cast<float>(M_PI) / 180.0f) * radius, height * 0.5f, -sinf(i * step * static_cast<float>(M_PI) / 180.0f) * radius);
		glVertex3f(cosf(i * step * static_cast<float>(M_PI) / 180.0f) * radius, -height * 0.5f, -sinf(i * step * static_cast<float>(M_PI) / 180.0f) * radius);
		glVertex3f(cosf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius, height * 0.5f, -sinf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius);

		glVertex3f(cosf(i * step * static_cast<float>(M_PI) / 180.0f) * radius, -height * 0.5f, -sinf(i * step * static_cast<float>(M_PI) / 180.0f) * radius);
		glVertex3f(cosf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius, -height * 0.5f, -sinf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius);
		glVertex3f(cosf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius, height * 0.5f, -sinf((i + 1) * step * static_cast<float>(M_PI) / 180.0f) * radius);
	}
	glEnd();
}

/*-----------------------------------------------------------------------------------
	Draw Cylinder with VBO without indices

	  parameters: radius, height, sides

-----------------------------------------------------------------------------------*/

void cylinderVBO(float radius, float height, int sides) {
    float* vertex = nullptr;
    float divAngle = 360.0f / (1.0f * sides);
    float angles[sides + 1], * res01, * res02;
    int i, indice = 0;

    angles[0] = angles[sides] = 0.0f;
    for(i = 1; i < sides; i++) {
        angles[i] = angles[i - 1] + divAngle;
    }

    vertex = (float*) malloc(global.n_points_VBO * sizeof(float));

    for(i = 0; i < sides; i++) {
        res01 = polarToCartesian(angles[i + 0], radius);
        res02 = polarToCartesian(angles[i + 1], radius);
        /* Top of Cylinder */
        vertex[indice + 0] = 0.0f;      vertex[indice + 1] = height / 2.0f;     vertex[indice + 2] = 0.0f;
        vertex[indice + 3] = res02[0];  vertex[indice + 4] = height / 2.0f;     vertex[indice + 5] = res02[1];
        vertex[indice + 6] = res01[0];  vertex[indice + 7] = height / 2.0f;     vertex[indice + 8] = res01[1];
        indice += 9;

        /* Bottom of Cylinder */
        vertex[indice + 0] = 0.0f;      vertex[indice + 1] = -height / 2.0f;     vertex[indice + 2] = 0.0f;
        vertex[indice + 3] = res01[0];  vertex[indice + 4] = -height / 2.0f;     vertex[indice + 5] = res01[1];
        vertex[indice + 6] = res02[0];  vertex[indice + 7] = -height / 2.0f;     vertex[indice + 8] = res02[1];
        indice += 9;

        /* Side of Cylinder */
        vertex[indice + 0] = res01[0];  vertex[indice + 1] = height / 2.0f;     vertex[indice + 2] = res01[1];
        vertex[indice + 3] = res02[0];  vertex[indice + 4] = height / 2.0f;     vertex[indice + 5] = res02[1];
        vertex[indice + 6] = res02[0];  vertex[indice + 7] = -height / 2.0f;     vertex[indice + 8] = res02[1];
        indice += 9;
        vertex[indice + 0] = res02[0];  vertex[indice + 1] = -height / 2.0f;     vertex[indice + 2] = res02[1];
        vertex[indice + 3] = res01[0];  vertex[indice + 4] = -height / 2.0f;     vertex[indice + 5] = res01[1];
        vertex[indice + 6] = res01[0];  vertex[indice + 7] = height / 2.0f;     vertex[indice + 8] = res01[1];
        indice += 9;
    }

    /* Binding buffer */
    glBindBuffer(GL_ARRAY_BUFFER, global.buffer[0]);

    /* Copying data */
    glBufferData(GL_ARRAY_BUFFER, global.n_points_VBO * sizeof(float), vertex, GL_STATIC_DRAW);

    free(vertex);
}

/*-----------------------------------------------------------------------------------
	Draw Cylinder with VBO with indices

	  parameters: radius, height, sides

-----------------------------------------------------------------------------------*/

void cylinderIndex(float radius, float height, int sides) {
    float *vertex = nullptr;
    float divAngle = 360.0f / (1.0f * sides);
    float angles[sides + 1], * res;
    int i, indice = 0, indi = 0;
    unsigned int *indices, ii = 0;

    angles[0] = angles[sides] = 0.0f;
    for(i = 1; i < sides; i++) {
        angles[i] = angles[i - 1] + divAngle;
    }

    vertex = (float*) malloc(global.n_points_Index * sizeof(float));
    indices = (unsigned int *) malloc(global.n_indexes * sizeof(unsigned int));

    vertex[indice + 0] = 0.0f;      vertex[indice + 1] = height / 2.0f;     vertex[indice + 2] = 0.0f;  // center top: index 0
    vertex[indice + 3] = 0.0f;      vertex[indice + 4] =-height / 2.0f;     vertex[indice + 5] = 0.0f;  // center bottom: index 1
    indice += 6;
    res = polarToCartesian(angles[0], radius);
    /* First 2 points */
    vertex[indice + 0] = res[0];    vertex[indice + 1] = height / 2.0f;     vertex[indice + 2] = res[1];
    vertex[indice + 3] = res[0];    vertex[indice + 4] =-height / 2.0f;     vertex[indice + 5] = res[1];
    indice += 6;
    free(res);
    for(i = 1; i < sides; i++) {
        res = polarToCartesian(angles[i], radius);
        vertex[indice + 0] = res[0];    vertex[indice + 1] = height / 2.0f;     vertex[indice + 2] = res[1];
        vertex[indice + 3] = res[0];    vertex[indice + 4] =-height / 2.0f;     vertex[indice + 5] = res[1];
        indice += 6;

        ii = static_cast<unsigned int>(i);
        /* Top triangle */
        indices[indi + 0] = 0;   indices[indi + 1] = (ii * 2) + 2;    indices[indi + 2] = (ii * 2);
        /* Bottom triangle */
        indices[indi + 3] = 1;   indices[indi + 4] = (ii * 2) + 1;    indices[indi + 5] = (ii * 2) + 3;
        /* Side square */
        indices[indi + 6] = (ii * 2);    indices[indi + 7] = (ii * 2) + 2;    indices[indi + 8] = (ii * 2) + 3;
        indices[indi + 9] = (ii * 2) + 3;    indices[indi + 10] = (ii * 2) + 1;    indices[indi + 11] = (ii * 2);
        indi += 12;
        free(res);
    }
    ii++;
    indices[indi + 0] = 0;   indices[indi + 1] = 2;    indices[indi + 2] = (ii * 2);
    /* Bottom triangle */
    indices[indi + 3] = 1;   indices[indi + 4] = (ii * 2) + 1;    indices[indi + 5] = 3;
    /* Side square */
    indices[indi + 6] = (ii * 2);    indices[indi + 7] = 2;    indices[indi + 8] = 3;
    indices[indi + 9] = 3;    indices[indi + 10] = (ii * 2) + 1;    indices[indi + 11] = (ii * 2);

    /* Binding buffer */
    glBindBuffer(GL_ARRAY_BUFFER, global.buffer[1]);
    /* Copying data */
    glBufferData(GL_ARRAY_BUFFER, global.n_points_Index * sizeof(float), vertex, GL_STATIC_DRAW);

    /* Binding indexes */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexes[0]);
    /* Copying data */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, global.n_indexes * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    free(vertex);
    free(indices);
}

void drawVBO() {
    glColor3f(0.0f, 0.5f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, global.buffer[0]);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, global.n_points_VBO);
}

void drawIndex() {
    glColor3f(1.0f, 0.4f, 0.0f);
    glBindBuffer(GL_ARRAY_BUFFER, global.buffer[1]);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexes[0]);
    glDrawElements(GL_TRIANGLES, global.n_indexes, GL_UNSIGNED_INT, nullptr);
}

void fps_update() {
    global.frame++;
    global.time = glutGet(GLUT_ELAPSED_TIME);
    if(global.time - global.timebase > 1000) {
        global.fps = global.frame * 1000.0f / (global.time - global.timebase);
        global.timebase = global.time;
        global.frame = 0;
        sprintf(global.print,"CG@DI - Assessment class 4 - %4f",global.fps);
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

	printReference(global.cRadius + 1.0f);

	if(global.draw_mode == 0)
    	cylinder(global.cRadius,global.cHeight,global.cSides);
	if(global.draw_mode == 1)
	    cylinder0(global.cRadius,global.cHeight,global.cSides);
	if(global.draw_mode == 2)
	    drawVBO();
	if(global.draw_mode == 3)
	    drawIndex();

	fps_update();
    glutSetWindowTitle(global.print);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

    // put code to process regular keys in here
    switch (c) {
        case 'f':
            global.draw_mode = (global.draw_mode + 1) % 4;
            /* Just to print a guiding message */
            if(global.draw_mode == 0)
                printf("Drawing in direct mode\n");
            if(global.draw_mode == 1)
                printf("Drawing with fans and strips\n");
            if(global.draw_mode == 2)
                printf("Drawing in VBO mode\n");
            if(global.draw_mode == 3)
                printf("Drawing in VBO with indexes mode\n");
            break;
        default:
            break;
    }

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
            global.camera_radius -= 0.1f;
            if (global.camera_radius < 0.1f)
                global.camera_radius = 0.1f;
            break;

        case GLUT_KEY_PAGE_UP:
            global.camera_radius += 0.1f;
            break;

        default:
            break;
	}
	spherical2Cartesian();
	glutPostRedisplay();
}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Home and End control the distance from the camera to the origin\n");
	printf("F key changes the drawing mode: Direct, Strips and Fans, VBO, VBO with Indexes\n");
}


int main(int argc, char **argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1400,800);
	glutCreateWindow("CG@DI - Assessment class 4");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	
	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

#ifndef __APPLE__
	glewInit();
#endif

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

    /* Generating buffers */
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenBuffers(2, global.buffer);
    glGenBuffers(1, global.indexes);


	spherical2Cartesian();  // setting camera start measures
    /* Building VBOs without and with indexes */
    cylinderVBO(global.cRadius, global.cHeight, global.cSides);
    cylinderIndex(global.cRadius, global.cHeight, global.cSides);

	printInfo();

	// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
