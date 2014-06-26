#include <windows.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "extras/headers/3DObject.h"
#include "extras/headers/Texture.h"
#include "extras/headers/bitmap.h"
//#include "extras/headers/glm.h"

#define DEFAULT_CAMERA_HEIGHT 1.5f

#define DEFAULT_WIDTH 900
#define DEFAULT_HEIGHT 600

#define HORIZON 100

#define MAP_WIDTH 20
#define MAP_HEIGHT 20
#define MAP_LENGTH 20

#define FLOOR_WIDTH (MAP_WIDTH + 2)
#define FLOOR_LENGTH (MAP_LENGTH + 2)

#define ESC 27
#define SPACE_KEY 32
#define RUNNING_KEY 'r'


#define SMOOTH 0
#define SMOOTH_MATERIAL 1
#define SMOOTH_MATERIAL_TEXTURE 2

//INITs
void mainInit();
void modelInit();
void cameraInit();
void centerInit();
void upVectorInit();
void lightInit();
void textureInit();
void fogInit();
void enemyInit();
void characterInit();


//WINDOW FUNCTIONS
void onReshape(int x, int y);
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void setWindow();

//KEYBOARD FUNCTIONS
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);

//MOVEMENT FUNCTIONS
void walkForward();
void walkBackward();
void walkLeft();
void walkRight();
void jump();

//RENDER FUNCTIONS
void initTexture();
void mainRender();
void renderScene();
void renderFloor();
void renderSkyBox();
void updateCamera();
void updateLight();
void setTextureToOpengl();
void testDraw();

//MOUSE FUNCTIONS
void onMouseButton(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onMousePassiveMove(int x, int y);

//PENGUIN FUNCTIONS
void updatePenguinState();

/*COLLISION FUNCTIONS
bool collidesAt(Point3d* coordinate);

//ENEMIES FUNCTIONS
void updateEnemies();
void enemyWalk(Point3d* enemyPosition);
*/

// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture;         /* Texture object */

typedef struct Point3d {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Point3d;

Point3d* cameraPosition = (Point3d*) (malloc(sizeof(Point3d)));
Point3d* centerPosition = (Point3d*) (malloc(sizeof(Point3d)));
Point3d* upVector = (Point3d*) (malloc(sizeof(Point3d)));

Point3d* enemy = (Point3d*) (malloc(sizeof(Point3d)));


typedef struct Window {
    GLint id;
    GLsizei width;
    GLsizei height;
} Window;

Window* mainWindow = (Window*) (malloc(sizeof(Window)));

typedef struct Mouse {
    GLfloat x;
    GLfloat y;
} Mouse;

typedef enum { NOTHING, FLOWERS, DOLPHINS, BALL } OBJ_ENUM;

OBJ_ENUM *sceneMatrix;

Mouse* mouse = (Mouse*) (malloc(sizeof(Mouse)));

int sceneHeight, sceneWidth;

//MOVEMENT GLOBALS
bool walkingForward = false;
bool walkingBackward = false;
bool walkingLeft = false;
bool walkingRight = false;
bool isRunning = false;
bool isJumping = false;
const float WALK_SPEED = 0.3f; //range must be > 0
const float RUNNING_SPEED = 0.5f; //range must be > 0
const float ENEMY_SPEED = 0.02; // range must be > 0
const float JUMP_SPEED = 0.04f; //range must be > 0
GLfloat jump_period = 0.0f;


C3DObject objeto, ball, flower, dolph;

bool collisionMatrix[MAP_WIDTH][MAP_LENGTH] = {false};


//COLLISION FUNCTIONS
bool collidesAt(Point3d* coordinate);

//ENEMIES FUNCTIONS
void updateEnemies();
void enemyWalk(Point3d* enemyPosition);

int main(int argc, char *argv[])
{

    mainWindow->width = DEFAULT_WIDTH;
    mainWindow->height = DEFAULT_HEIGHT;

    glutInit(&argc, argv);
    glutInitWindowSize(mainWindow->width, mainWindow->height);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);

    mainWindow->id = glutCreateWindow("Pengo WARS");



    glutDisplayFunc(mainRender);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutReshapeFunc(onReshape);

    /**
	Register mouse events handlers
	*/
	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMove);
	glutPassiveMotionFunc(onMousePassiveMove);

    //glutIdleFunc(idle);

    mainInit();
    glutMainLoop();

    return EXIT_SUCCESS;
}

//INITs IMPLEMENTATION
void mainInit() {

    glClearColor(0.8f, 0.8f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setWindow();
    glViewport(0, 0, mainWindow->width , mainWindow->height);
    lightInit();
    fogInit();

    initTexture();

    glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	// habilita o z-buffer
	glEnable(GL_DEPTH_TEST);

    modelInit();
    enemyInit();
    cameraInit();
    centerInit();
    upVectorInit();
    textureInit();

}

void modelInit() {
	printf("Loading models.. \n");
	objeto.Init();
	objeto.Load("../res/models/ball.obj");
	flower.Init();
	flower.Load("../res/models/flowers.obj");
	dolph.Init();
	dolph.Load("../res/models/dolphins.obj");
	//modelAL = CModelAl();
	//modelAL.Init();
	printf("Models ok. \n \n \n");
}

void cameraInit() {
    cameraPosition->x = 0.0f;
    cameraPosition->y = DEFAULT_CAMERA_HEIGHT;
    cameraPosition->z = 0.0f;
}

void centerInit() {
    centerPosition->x = 0.0f;
    centerPosition->y = 0.0f;
    centerPosition->z = 0.0f;
}

void upVectorInit() {
    upVector->x = 0.0f;
    upVector->y = 1.0f;
    upVector->z = 0.0f;
}

void lightInit() {
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);

	GLfloat light0_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat light0_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat light0_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light0_position[] = {0.0f, 15.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

/*
    GLfloat light1_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light1_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat light1_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light1_position[] = {cameraPosition->x, cameraPosition->y, cameraPosition->z, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
*/

}

void textureInit() {
    printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../res/textures/snow.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}
    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
      type = GL_TEXTURE_1D;
    else
      type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(type, texture);

    // Create an RGBA image
    rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

    i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
    {
            rgbaptr[0] = ptr[2];     // windows BMP = BGR
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[0];
            rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
    }
    /*
	// Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );
    */

    printf("Textura %d\n", texture);
	printf("Textures ok.\n\n", texture);

}

void fogInit() {
    glEnable(GL_FOG);
    glFogf(GL_FOG_DENSITY, 0.005f);
}

void enemyInit() {
    enemy->x = 20.0f;
    enemy->y = 0.0f;
    enemy->z = 20.0f;
}

//WINDOW FUNCTIONS IMPLEMENTATION
void onReshape(int x, int y) {
    mainWindow->width = x;
    mainWindow->height = y;
	setWindow();
	glViewport(0, 0, mainWindow->width, mainWindow->height);
}

void setWindow() {
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)mainWindow->width/(GLfloat)mainWindow->height,
            0.1f, 100.0f);
}


//KEYBOARD FUNCTIONS IMPLEMENTATION
void onKeyDown(unsigned char key, int x, int y) {
    switch(key) {
        case 'w':
            walkingForward = true;
            break;

        case 's':
            walkingBackward = true;
            break;

        case 'a':
            walkingLeft = true;
            break;

        case 'd':
            walkingRight = true;
            break;

        case RUNNING_KEY:
            isRunning = true;
            break;

        case SPACE_KEY:
            isJumping = true;
            break;

        case ESC:
            exit(0);
            break;
    }
}

void onKeyUp(unsigned char key, int x, int y) {
    switch(key) {
        case 'w':
            walkingForward = false;
            break;

        case 's':
            walkingBackward = false;
            break;

        case 'a':
            walkingLeft = false;
            break;

        case 'd':
            walkingRight = false;
            break;

        case RUNNING_KEY:
            isRunning = false;
            break;

        case SPACE_KEY:
            ;//do nothing
            break;

        case ESC:
            exit(0);
            break;
    }
}

/**
Initialize the texture using the library bitmap
*/
void initTexture(void)
{
    printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("snow.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}
    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
      type = GL_TEXTURE_1D;
    else
      type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(type, texture);

    // Create an RGBA image
    rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

    i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
    {
            rgbaptr[0] = ptr[2];     // windows BMP = BGR
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[0];
            rgbaptr[3] = 255;
    }
    BITMAPINFO *sceneInfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO));
    GLubyte *sceneBmp = LoadDIBitmap("scene.bmp", &sceneInfo);
    if (sceneBmp == (GLubyte*) NULL) {
        printf("Could not load scene.bmp, please make sure it exists.");
        return;
    }
    printf("Recovering scene information from scene.bmp. The mapping is:\n\
           Red pixels are Balls\n\
           Green pixels are Flowers\n\
           Blue pixels are Dolphins\n\
           Black pixels (or anything else) are empty spaces.\n");
    int k = sceneInfo->bmiHeader.biWidth * sceneInfo->bmiHeader.biHeight;
    // save the scene's height and width for later positioning
    sceneHeight = sceneInfo->bmiHeader.biHeight;
    sceneWidth = sceneInfo->bmiHeader.biWidth;
    // alloc the matrix that contains what goes where
    sceneMatrix = (OBJ_ENUM*) malloc(k * sizeof(OBJ_ENUM));
    int aux = 0;
    int length = (sceneInfo->bmiHeader.biWidth * 3 + 3) & ~3;
    int x;
    for (int y = 0; y < (sceneInfo)->bmiHeader.biHeight; y ++)
        for (GLubyte *ptr = sceneBmp + y * length, x = sceneInfo->bmiHeader.biWidth;
            x > 0;
	     x --, ptr += 3)
        {
            const GLubyte r = ptr[2];
            const GLubyte g = ptr[1];
            const GLubyte b = ptr[0];
            long cor = (r << 16) + (g << 8) + b;
            if (cor == 0x000000){

                sceneMatrix[aux] = NOTHING;
            }
            else if (cor == 0xFF0000)
                sceneMatrix[aux] = BALL;
            else if (cor == 0x0000FF)
                sceneMatrix[aux] = DOLPHINS;
            else if (cor == 0x00FF00)
                sceneMatrix[aux] = FLOWERS;
            else sceneMatrix[aux] = NOTHING;
            //printf("%d ",sceneMatrix[aux]);
            aux++;
	    }


    free(sceneInfo);
    free(sceneBmp);
    // sceneInfo = NULL;
    /*
	// Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );

    */
    printf("Textura %d\n", texture);
	printf("Textures ok.\n\n", texture);

}

//MOVEMENT FUNCTIONS
void walkForward() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat deltaX = centerPosition->x - cameraPosition->x;
    GLfloat deltaZ = centerPosition->z - cameraPosition->z;

    GLfloat oldPositionX = cameraPosition->x;
    GLfloat oldPositionZ = cameraPosition->z;

    cameraPosition->x = cameraPosition->x + (deltaX * movementSpeed);
    cameraPosition->z = cameraPosition->z + (deltaZ * movementSpeed);


    if(cameraPosition->x < -MAP_WIDTH || cameraPosition->x > MAP_WIDTH) {
        cameraPosition->x = oldPositionX;
    }
    if(cameraPosition->z < -MAP_LENGTH || cameraPosition->z > MAP_LENGTH) {
        cameraPosition->z = oldPositionZ;
    }
}

void walkBackward() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat deltaX = centerPosition->x - cameraPosition->x;
    GLfloat deltaZ = centerPosition->z - cameraPosition->z;

    GLfloat oldPositionX = cameraPosition->x;
    GLfloat oldPositionZ = cameraPosition->z;

    cameraPosition->x = cameraPosition->x - (deltaX * movementSpeed);
    cameraPosition->z = cameraPosition->z - (deltaZ * movementSpeed);

    if(cameraPosition->x < -MAP_WIDTH || cameraPosition->x > MAP_WIDTH) {
        cameraPosition->x = oldPositionX;
    }
    if(cameraPosition->z < -MAP_LENGTH || cameraPosition->z > MAP_LENGTH) {
        cameraPosition->z = oldPositionZ;
    }
}

void walkLeft() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat oldPositionX = cameraPosition->x;
    GLfloat oldPositionZ = cameraPosition->z;

    GLfloat deltaX = centerPosition->x - cameraPosition->x;
    GLfloat deltaZ = centerPosition->z - cameraPosition->z;

    cameraPosition->x = cameraPosition->x + (deltaZ * movementSpeed);
    cameraPosition->z = cameraPosition->z - (deltaX * movementSpeed);

    if(cameraPosition->x < -MAP_WIDTH || cameraPosition->x > MAP_WIDTH) {
        cameraPosition->x = oldPositionX;
    }
    if(cameraPosition->z < -MAP_LENGTH || cameraPosition->z > MAP_LENGTH) {
        cameraPosition->z = oldPositionZ;
    }
}

void walkRight() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat oldPositionX = cameraPosition->x;
    GLfloat oldPositionZ = cameraPosition->z;

    GLfloat deltaX = centerPosition->x - cameraPosition->x;
    GLfloat deltaZ = centerPosition->z - cameraPosition->z;

    cameraPosition->x = cameraPosition->x - (deltaZ * movementSpeed);
    cameraPosition->z = cameraPosition->z + (deltaX * movementSpeed);

    if(cameraPosition->x < -MAP_WIDTH || cameraPosition->x > MAP_WIDTH) {
        cameraPosition->x = oldPositionX;
    }
    if(cameraPosition->z < -MAP_LENGTH || cameraPosition->z > MAP_LENGTH) {
        cameraPosition->z = oldPositionZ;
    }
}

void jump() {
    if(jump_period < 1.0f) {
        cameraPosition->y = DEFAULT_CAMERA_HEIGHT + (sin(jump_period * M_PI));
        jump_period += JUMP_SPEED;
    }
    else {
        jump_period = 0.0f;
        isJumping = false;
    }
}

//RENDER FUNCTIONS IMPLEMENTATION
void mainRender() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderScene();
	glFlush();
	glutPostRedisplay();
	Sleep(30);
}

void renderScene() {
	//printf("x: %f \t y:%f \t z:%f\n", cameraPosition->x, cameraPosition->y, cameraPosition->z);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    updateCamera();

    for (int i = 0; i < sceneHeight; ++i)
    {
        for (int j = 0; j < sceneWidth; ++j)
        {
            glPushMatrix();
            //glTranslatef(((double)8*i)/sceneHeight - 4.0, 1.0, ((double)8*j)/sceneWidth - 4.0);
            glTranslatef(4.0 - (8.0* i/sceneHeight), 1.0, 4.0 - (8.0* j/sceneWidth));

            switch (sceneMatrix[i*sceneWidth + j])
            {
            case FLOWERS:
                flower.Draw(SMOOTH_MATERIAL);
                break;
            case DOLPHINS:
                dolph.Draw(SMOOTH_MATERIAL);
                break;
            case BALL:
                ball.Draw(SMOOTH_MATERIAL_TEXTURE);
                break;
            case NOTHING:
            default:
                break;
            }
            glPopMatrix();
        }
    }

    // sets the bmp file already loaded to the OpenGL parameters
    setTextureToOpengl();

    renderFloor();
    //renderSkyBox();
    //updatePenguinState();
    updateLight();

    setTextureToOpengl();

    //testDraw();
    updateEnemies();

    if (!collidesAt(cameraPosition)) updatePenguinState();
}

void renderFloor(){
    glBindTexture(type, texture);

    // draws 1x1 quads from upper-left to bottom-right sides of the map
    for(GLint x = -FLOOR_WIDTH; x < FLOOR_WIDTH; x++) {
        for(GLint z = FLOOR_LENGTH; z > -FLOOR_LENGTH; z--) {
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x, 0 , z - 1);

                glTexCoord2f(1.0f, 0.0f);
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 1, 0 , z - 1);

                glTexCoord2f(1.0f, 1.0f);
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 1, 0 , z);

                glTexCoord2f(0.0f, 1.0f);
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x, 0 , z);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3f(x, 0 , z - 1);
                glVertex3f(x - 1, 0 , z - 1);
                glVertex3f(x - 1, 0 , z);
                glVertex3f(x, 0 , z);
            glEnd();
        }
    }


/*


    glPushMatrix();
    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(-FLOOR_WIDTH, 0.0f, FLOOR_LENGTH);

        glTexCoord2f(0.0f, 0.0f);
        glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(FLOOR_WIDTH, 0.0f, FLOOR_LENGTH);

        glTexCoord2f(0.0f, 1.0f);
        glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(FLOOR_WIDTH, 0.0f, -FLOOR_LENGTH);

        glTexCoord2f(1.0f, 1.0f);
        glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(-FLOOR_WIDTH, 0.0f, -FLOOR_LENGTH);
	glEnd();

    glPopMatrix();
    */
}

void renderSkyBox() {

    glBindTexture(type, texture);
    glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.6f, 0.6f, 1.0f);

    glDisable(GL_CULL_FACE);
    glPushMatrix();
        glBegin(GL_QUADS);
            //LEFT SIDE
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(MAP_WIDTH,0,-MAP_LENGTH);
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(MAP_WIDTH,0,MAP_LENGTH);
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(MAP_WIDTH,MAP_HEIGHT,MAP_LENGTH);
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(MAP_WIDTH,MAP_HEIGHT,-MAP_LENGTH);

            //RIGHT SIDE
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-MAP_WIDTH,0,-MAP_LENGTH);
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-MAP_WIDTH,0,MAP_LENGTH);
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-MAP_WIDTH,MAP_HEIGHT,MAP_LENGTH);
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-MAP_WIDTH,MAP_HEIGHT,-MAP_LENGTH);

            //FRONT SIDE
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(-MAP_WIDTH,0,MAP_LENGTH);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(MAP_WIDTH,0,MAP_LENGTH);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(MAP_WIDTH,MAP_HEIGHT,MAP_LENGTH);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(-MAP_WIDTH,MAP_HEIGHT,MAP_LENGTH);

            //BACK SIDE
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-MAP_WIDTH,0,-MAP_LENGTH);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(MAP_WIDTH,0,-MAP_LENGTH);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(MAP_WIDTH,MAP_HEIGHT,-MAP_LENGTH);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-MAP_WIDTH,MAP_HEIGHT,-MAP_LENGTH);

            //TOP
            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(-MAP_WIDTH, MAP_HEIGHT, MAP_LENGTH);
            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(MAP_WIDTH, MAP_HEIGHT, MAP_LENGTH);
            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(MAP_WIDTH, MAP_HEIGHT, -MAP_LENGTH);
            glTexCoord2f(1.0f, 1.0f);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(-MAP_WIDTH, MAP_HEIGHT, -MAP_LENGTH);
        glEnd();
    glPopMatrix();
}

void updateCamera() {

    centerPosition->y = cameraPosition->y + mouse->y;
    centerPosition->x = cameraPosition->x - sin(mouse->x * M_PI * 1.2f);
    centerPosition->z = cameraPosition->z + cos(mouse->x * M_PI * 1.2f);

    //printf("x: %f \t z: %f \n",centerPosition->x, centerPosition->z);

    gluLookAt(cameraPosition->x, cameraPosition->y, cameraPosition->z,
              centerPosition->x, centerPosition->y, centerPosition->z,
              upVector->x, upVector->y, upVector->z);
}

void updateLight() {
    GLfloat light_position[] = {15.0f, 20.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void setTextureToOpengl()
{
    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(type, texture);

	// Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );
}

void testDraw() {
    GLint drawType = 2;
    glPushMatrix();
        glTranslatef(10.0f, 0 ,0.0f);
        objeto.Draw(drawType);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-10.0f, 0,00.0f);
        objeto.Draw(drawType);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0,10.0f);
        objeto.Draw(drawType);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0,-10.0f);
        objeto.Draw(drawType);
    glPopMatrix();

}

//MOUSE FUNCTIONS IMPLEMENTATION
//Mouse button event handler
void onMouseButton(int button, int state, int x, int y) {
	glutPostRedisplay();
}

//Mouse move while button pressed event handler
void onMouseMove(int x, int y) {
    onMousePassiveMove(x, y);
	glutPostRedisplay();
}

//Mouse move with no button pressed event handler
void onMousePassiveMove(int x, int y) {
    //brings the (0,0) coordinate to the center of the window
	GLfloat xCoordinate = x - (mainWindow->width/2.0f);

	//multiplies the Y coordinate by -1 because glut reads the Y from top to bottom, contrary to the cartesian coordinates.
	GLfloat yCoordinate = (y - (mainWindow->height/2.0f)) * (-1);

    //do not question the choice of a reality factor for these calculi
    GLfloat realityFactor = 5.0f;

    //gives the % of variance between origin and maximum width/height
    mouse->x = xCoordinate / (float) mainWindow->width;
    //changes the range of delta x from [-0.5, 0.5] to [-1, 1] in order to improve updateCamera method calculi
    mouse->x = mouse->x * 2;

    mouse->y = yCoordinate * realityFactor / (float) mainWindow->height;

	//printf("mouse x: %f \t mouse y: %f\n", mouse->x, mouse->y);
}

void updatePenguinState(){
    if(walkingForward) walkForward();
    else if(walkingBackward) walkBackward();

    if(walkingLeft) walkLeft();
    else if(walkingRight) walkRight();

    if(isJumping) jump();
}


bool collidesAt(Point3d* coordinate) {
    GLint xAtCollisionMatrix = ((int) coordinate->x) + MAP_WIDTH;
    GLint zAtCollisionMatrix = ((int) coordinate->z) + MAP_LENGTH;

    bool isColliding = collisionMatrix[xAtCollisionMatrix][zAtCollisionMatrix];

    //printf("x: %i \t z: %i hit: %i\n",xAtCollisionMatrix, zAtCollisionMatrix, isColliding);

    if(isColliding) {
        printf("HIT\n");
        return true;
    } else
    printf("\n");
    return false;
}

void updateEnemies() {
    glPushMatrix();
        glTranslatef(enemy->x, enemy->y, enemy->z);
        objeto.Draw(2);
    glPopMatrix();

    // clears the old position
    GLint enemyCollisionX = ((int) enemy->x) + MAP_WIDTH;
    GLint enemyCollisionZ = ((int) enemy->z) + MAP_LENGTH;
    collisionMatrix[enemyCollisionX][enemyCollisionZ] = false;

    enemyWalk(enemy);

    // sets true to the new enemy position in the collision matrix
    enemyCollisionX = ((int) enemy->x) + MAP_WIDTH;
    enemyCollisionZ = ((int) enemy->z) + MAP_LENGTH;
    collisionMatrix[enemyCollisionX][enemyCollisionZ] = true;

}
void enemyWalk(Point3d* enemyPosition) {
    GLfloat deltaX = cameraPosition->x - enemyPosition->x;
    GLfloat deltaZ = cameraPosition->z - enemyPosition->z;
    GLfloat distance = sqrt( pow(deltaX, 2) + pow(deltaZ, 2) );
    if (distance < 100.0f) {
            enemy->x += (deltaX * ENEMY_SPEED);
            enemy->z += (deltaZ * ENEMY_SPEED);
        //printf("PERTO!\n");
    }
    //else printf("\n");
}
