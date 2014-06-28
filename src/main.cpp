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
#include "extras/headers/structures.h"

#include "../files needed/include/al/al.h"
#include "../files needed/include/al/alc.h"
#include "../files needed/include/al/alut.h"


#define DEFAULT_CAMERA_HEIGHT 1.5f

#define DEFAULT_WIDTH 900
#define DEFAULT_HEIGHT 600

#define HORIZON 100

#define MAP_WIDTH 20
#define MAP_HEIGHT 20
#define MAP_LENGTH 20

#define FLOOR_WIDTH (MAP_WIDTH + 1)
#define FLOOR_LENGTH (MAP_LENGTH + 1)

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
void renderScene(bool updateCam);
void renderMiniMap();
void renderFloor();
void renderFromSceneMatrix();
void renderFromCollisionMatrix();
void renderSkyBox();
void updateCamera();
void updateLight();
void setTextureToOpengl();
void renderPenguin();
void testDraw();

//MOUSE FUNCTIONS
void onMouseButton(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onMousePassiveMove(int x, int y);

//PENGUIN FUNCTIONS
void updatePenguinState();
void throwBlock();

//COLLISION FUNCTIONS
bool collidesAt(Point3d* coordinate);

//ENEMIES FUNCTIONS
void updateEnemies();
void enemyWalk(Point3d* enemyPosition);

//SOUND FUNCTIONS
void soundInit();

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

//WINDOW GLOBAL
Window* mainWindow = (Window*) (malloc(sizeof(Window)));

//CAMERA GLOBAL
bool isThirdPerson = false;

//MOUSE COORDINATES GLOBAL
Mouse* mouse = (Mouse*) (malloc(sizeof(Mouse)));

//POINT3D COORDINATES GLOBALS
Point3d* penguinPosition = (Point3d*) (malloc(sizeof(Point3d)));
Point3d* centerPosition = (Point3d*) (malloc(sizeof(Point3d)));
Point3d* upVector = (Point3d*) (malloc(sizeof(Point3d)));
Point3d* enemy = (Point3d*) (malloc(sizeof(Point3d)));

//OBJECTS GLOBAL
OBJ_ENUM *sceneMatrix;
int sceneHeight, sceneWidth;
C3DObject enemyObject, solidBlock, throwableBlock, penguin;
bool isBlockMoving = false;

//COLLISION GLOBALS
OBJ_ENUM collisionMatrix[MAP_WIDTH*2][MAP_LENGTH*2] = {NOTHING};

//MOVEMENT GLOBALS
bool walkingForward = false;
bool walkingBackward = false;
bool walkingLeft = false;
bool walkingRight = false;
bool isRunning = false;
bool isJumping = false;
const float WALK_SPEED = 0.2f; //range must be > 0
const float RUNNING_SPEED = 0.3f; //range must be > 0
const float ENEMY_SPEED = 0.02; // range must be > 0
const float JUMP_SPEED = 0.04f; //range must be > 0
GLfloat jump_period = 0.0f;

//SOUND STUFF

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
    soundInit();

}

void modelInit() {
	printf("Loading models.. \n");
	enemyObject.Init();
	enemyObject.Load("../res/models/ball.obj");
	solidBlock.Init();
	solidBlock.Load("../res/models/flowers.obj");
	throwableBlock.Init();
	throwableBlock.Load("../res/models/dolphins.obj");
	penguin.Init();
	penguin.Load("../res/models/penguin.obj");
	//modelAL = CModelAl();
	//modelAL.Init();
	printf("Models ok. \n \n \n");
}

void cameraInit() {
    penguinPosition->x = 0.0f;
    penguinPosition->y = DEFAULT_CAMERA_HEIGHT;
    penguinPosition->z = 0.0f;
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

	GLfloat light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light0_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat light0_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light0_position[] = {0.0f, 0.0f, 0.0f, 1.0f };

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0.0f, 15.0f, 0.0f);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glPopMatrix();
}

/**
Initialize the texture using the library bitmap
*/
void textureInit(void)
{
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
            rgbaptr[3] = 255;
    }
    BITMAPINFO *sceneInfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO));
    GLubyte *sceneBmp = LoadDIBitmap("../res/textures/scene.bmp", &sceneInfo);
    if (sceneBmp == (GLubyte*) NULL) {
        printf("Could not load scene.bmp, please make sure it exists.");
        return;
    }
    printf("Recovering scene information from scene.bmp. The mapping is:\n\
           Red pixels are Communists!\n\
           Green pixels are Throwable Blocks\n\
           Blue pixels are Solid Blocks\n\
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
                sceneMatrix[aux] = ENEMY;
            else if (cor == 0x0000FF)
                sceneMatrix[aux] = SOLID_BLOCK;
            else if (cor == 0x00FF00)
                sceneMatrix[aux] = THROWABLE_BLOCK;
                else if (cor == 0xFFFFFF)
                sceneMatrix[aux] = PENGUIN;
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

void fogInit() {
    glEnable(GL_FOG);
    glFogf(GL_FOG_DENSITY, 0.005f);
}

void enemyInit() {
    enemy->x = 10.0f;
    enemy->y = 0.0f;
    enemy->z = 10.0f;
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

        case 'q':
            throwBlock();
            break;

        case RUNNING_KEY:
            isRunning = true;
            break;

        case SPACE_KEY:
            isJumping = true;
            break;

        case 'c':
            if(isThirdPerson) isThirdPerson = false;
            else isThirdPerson = true;
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

         case 'c':
            ;//do nothing
            break;

        case ESC:
            exit(0);
            break;
    }
}

//MOVEMENT FUNCTIONS
void walkForward() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat deltaX = centerPosition->x - penguinPosition->x;
    GLfloat deltaZ = centerPosition->z - penguinPosition->z;

    GLfloat oldPositionX = penguinPosition->x;
    GLfloat oldPositionZ = penguinPosition->z;

    penguinPosition->x = penguinPosition->x + (deltaX * movementSpeed);
    penguinPosition->z = penguinPosition->z + (deltaZ * movementSpeed);


    if(penguinPosition->x < -MAP_WIDTH || penguinPosition->x > MAP_WIDTH || collidesAt(penguinPosition)) {
        penguinPosition->x = oldPositionX;
    }
    if(penguinPosition->z < -MAP_LENGTH || penguinPosition->z > MAP_LENGTH || collidesAt(penguinPosition)) {
        penguinPosition->z = oldPositionZ;
    }
}

void walkBackward() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat deltaX = centerPosition->x - penguinPosition->x;
    GLfloat deltaZ = centerPosition->z - penguinPosition->z;

    GLfloat oldPositionX = penguinPosition->x;
    GLfloat oldPositionZ = penguinPosition->z;

    penguinPosition->x = penguinPosition->x - (deltaX * movementSpeed);
    penguinPosition->z = penguinPosition->z - (deltaZ * movementSpeed);

    if(penguinPosition->x < -MAP_WIDTH || penguinPosition->x > MAP_WIDTH || collidesAt(penguinPosition)) {
        penguinPosition->x = oldPositionX;
    }
    if(penguinPosition->z < -MAP_LENGTH || penguinPosition->z > MAP_LENGTH || collidesAt(penguinPosition)) {
        penguinPosition->z = oldPositionZ;
    }
}

void walkLeft() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat oldPositionX = penguinPosition->x;
    GLfloat oldPositionZ = penguinPosition->z;

    GLfloat deltaX = centerPosition->x - penguinPosition->x;
    GLfloat deltaZ = centerPosition->z - penguinPosition->z;

    penguinPosition->x = penguinPosition->x + (deltaZ * movementSpeed);
    penguinPosition->z = penguinPosition->z - (deltaX * movementSpeed);

    if(penguinPosition->x < -MAP_WIDTH || penguinPosition->x > MAP_WIDTH || collidesAt(penguinPosition)) {
        penguinPosition->x = oldPositionX;
    }
    if(penguinPosition->z < -MAP_LENGTH || penguinPosition->z > MAP_LENGTH || collidesAt(penguinPosition)) {
        penguinPosition->z = oldPositionZ;
    }
}

void walkRight() {
    GLfloat movementSpeed = WALK_SPEED;
    if(isRunning) movementSpeed = RUNNING_SPEED;

    GLfloat oldPositionX = penguinPosition->x;
    GLfloat oldPositionZ = penguinPosition->z;

    GLfloat deltaX = centerPosition->x - penguinPosition->x;
    GLfloat deltaZ = centerPosition->z - penguinPosition->z;

    penguinPosition->x = penguinPosition->x - (deltaZ * movementSpeed);
    penguinPosition->z = penguinPosition->z + (deltaX * movementSpeed);

    if(penguinPosition->x < -MAP_WIDTH || penguinPosition->x > MAP_WIDTH || collidesAt(penguinPosition)) {
        penguinPosition->x = oldPositionX;
    }
    if(penguinPosition->z < -MAP_LENGTH || penguinPosition->z > MAP_LENGTH || collidesAt(penguinPosition)) {
        penguinPosition->z = oldPositionZ;
    }
}

void jump() {
    if(jump_period < 1.0f) {
        penguinPosition->y = DEFAULT_CAMERA_HEIGHT + (sin(jump_period * M_PI));
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
	renderScene(false);

    glClear(GL_DEPTH_BUFFER_BIT);
	renderScene(true);

	glFlush();
	glutPostRedisplay();
    Sleep(30);
}

void renderScene(bool isMiniMap) {
    if(isMiniMap) glViewport(mainWindow->width/50, mainWindow->height/50, mainWindow->width/4, mainWindow->height/4);
    else glViewport(0, 0, mainWindow->width, mainWindow->height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    updateLight();

    if(isMiniMap)
    gluLookAt(0.0f, 50.0f, 0.0f,
              0.0f, 0.1f, 0.1f,
              0.0f, 1.0f, 0.0f);
    else updateCamera();

    renderPenguin();

    setTextureToOpengl();
    renderFloor();
    //renderSkyBox();
    renderFromSceneMatrix();
    //testDraw();
    updateEnemies();
    updatePenguinState();
}


void renderFloor(){
    glBindTexture(type, texture);

    // draws 1x1 quads from upper-left to bottom-right sides of the map
    for(GLint x = -FLOOR_WIDTH + 1; x <= FLOOR_WIDTH; x++) {
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
            /*
            glBegin(GL_LINE_STRIP);
                glVertex3f(x, 0 , z - 1);
                glVertex3f(x - 1, 0 , z - 1);
                glVertex3f(x - 1, 0 , z);
                glVertex3f(x, 0 , z);
            glEnd();
            */
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
void renderFromSceneMatrix() {

    glEnable(GL_COLOR_MATERIAL);

    for (int i = 0; i < sceneHeight; ++i) {
        for (int j = 0; j < sceneWidth; ++j) {
            GLfloat x = i - (sceneHeight/2) + 0.5f;
            GLfloat y = 0.0f;
            GLfloat z = j - (sceneWidth/2) + 0.5f;

            GLint xAtMatrix = MAP_WIDTH - (int) x;
            GLint zAtMatrix = MAP_WIDTH - (int) z;

            glPushMatrix();
            glTranslatef(x, y ,z);

            switch (sceneMatrix[i*sceneWidth + j])
            {
            case ENEMY:
                enemyObject.Draw(SMOOTH_MATERIAL_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, NULL);
                glEnable(GL_COLOR_MATERIAL);
                break;
            case SOLID_BLOCK:
                glDisable(GL_CULL_FACE);
                glScalef(1.0f, 4.0f, 1.0f);
                glColor4f(0.6f, 0.6f, 1.0f, 1.0f);
                glutSolidCube(1.0f);
                glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
                glutWireCube(1.0f);
                collisionMatrix[xAtMatrix][zAtMatrix] = SOLID_BLOCK;
                glEnable(GL_CULL_FACE);
                break;
            case THROWABLE_BLOCK:
                glDisable(GL_CULL_FACE);
                glScalef(1.0f, 2.0f, 1.0f);
                glColor4f(0.6f, 1.0f, 0.6f, 1.0f);
                glutSolidCube(1.0f);
                glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
                glutWireCube(1.0f);
                collisionMatrix[xAtMatrix][zAtMatrix] = THROWABLE_BLOCK;
                glEnable(GL_CULL_FACE);
                break;
            case PENGUIN:
                glTranslatef(0.0f, 1.0f, 0.0f);
                penguin.Draw(SMOOTH_MATERIAL_TEXTURE);
                glBindTexture(GL_TEXTURE_2D, NULL);
                glEnable(GL_COLOR_MATERIAL);
                break;
            case NOTHING:
            default:
                break;
            }
            glPopMatrix();
        }
    }
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
}

void renderSkyBox() {
    glBindTexture(type, texture);
    //glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.8f, 0.8f, 1.0f);

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
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(-MAP_WIDTH, MAP_HEIGHT, MAP_LENGTH);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(MAP_WIDTH, MAP_HEIGHT, MAP_LENGTH);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(MAP_WIDTH, MAP_HEIGHT, -MAP_LENGTH);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(-MAP_WIDTH, MAP_HEIGHT, -MAP_LENGTH);
        glEnd();
    glPopMatrix();

}

void updateCamera() {

    centerPosition->y = penguinPosition->y + mouse->y;
    centerPosition->x = penguinPosition->x - sin(mouse->x * M_PI * 1.2f);
    centerPosition->z = penguinPosition->z + cos(mouse->x * M_PI * 1.2f);

    //printf("x: %f \t z: %f \n",centerPosition->x, centerPosition->z);
    if(isThirdPerson) {
        gluLookAt(penguinPosition->x + (sin(mouse->x * M_PI * 1.2f)) * 4,
                                        penguinPosition->y + 1.5f,
                                        penguinPosition->z - (cos(mouse->x * M_PI * 1.2f)) * 4,
              centerPosition->x, centerPosition->y, centerPosition->z,
              upVector->x, upVector->y, upVector->z);
    }
    else {
        gluLookAt(penguinPosition->x, penguinPosition->y, penguinPosition->z,
              centerPosition->x, centerPosition->y, centerPosition->z,
              upVector->x, upVector->y, upVector->z);
    }
}

void updateLight() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0.0f, 25.0f, 0.0f);
	GLfloat light0_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glPopMatrix();
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

void renderPenguin() {
    GLfloat angle = -(mouse->x);
   // GLfloat angle = -( (asin (sin(mouse->x * M_PI * 1.2f))) * 180/M_PI );

   /* GLfloat xTranslate = (penguinPosition->x * cos(angle)) - (penguinPosition->z * sin(angle));
    GLfloat zTranslate = (penguinPosition->z * cos(angle)) - (penguinPosition->z * sin(angle));
    */
    //printf("xt: %f \t zt: %f\n",xTranslate,zTranslate);
    printf("%.2f\n", mouse->x);
    glPushMatrix();
        glTranslatef(0.0f, 1.0f, 0.0f);
        glTranslatef(penguinPosition->x, penguinPosition->y - 1.5f, penguinPosition->z);
        glRotatef(angle * 210, 0.0f, 1.0f, 0.0f);
        penguin.Draw(SMOOTH_MATERIAL_TEXTURE);
    glPopMatrix();

}

void testDraw() {
    GLint drawType = SMOOTH_MATERIAL_TEXTURE;
    glPushMatrix();
        glTranslatef(10.0f, 0 ,0.0f);
        solidBlock.Draw(drawType);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-10.0f, 0,00.0f);
        solidBlock.Draw(drawType);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0,10.0f);
        solidBlock.Draw(drawType);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0,-10.0f);
        solidBlock.Draw(drawType);
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
    collidesAt(penguinPosition);
}

void throwBlock() {
    GLfloat deltaX = centerPosition->x - penguinPosition->x;
    GLfloat deltaZ = centerPosition->z - penguinPosition->z;

    GLint x = MAP_WIDTH - (int) penguinPosition->x;
    GLint z = MAP_LENGTH - (int) penguinPosition->z;

    printf("Collision\t object: %i\n", collisionMatrix[x][z-1]);

    if(isBlockMoving) {

    } else

    //penguin is looking to positive Z: front
    if(deltaZ >= 0.5 && collisionMatrix[x][z-1] == THROWABLE_BLOCK) {
    }
    else
    //penguin is looking to negative Z: behind
    if(deltaZ <= -0.5 && collisionMatrix[x][z+1]) {
    }
    else
    //penguin is looking to positive X: left
    if(deltaX >= 0.5 && collisionMatrix[x-1][z]) {
    }
    else
    ////penguin is looking to negative X: right
    if(deltaX <= -0.5 && collisionMatrix[x-1][z]) ;
}


bool collidesAt(Point3d* coordinate) {
    GLint x = MAP_WIDTH - (int) coordinate->x;
    GLint z = MAP_LENGTH - (int) coordinate->z;

    OBJ_ENUM isColliding = collisionMatrix[x][z];

    if(isColliding == SOLID_BLOCK || isColliding == THROWABLE_BLOCK){
            return true;
    }
    else return false;

}

void updateEnemies() {
    glPushMatrix();
        glTranslatef(enemy->x, enemy->y, enemy->z);
        enemyObject.Draw(SMOOTH_MATERIAL_TEXTURE);
    glPopMatrix();

    // clears the old position
    //GLint enemyCollisionX = ((int) enemy->x) + MAP_WIDTH;
    //GLint enemyCollisionZ = ((int) enemy->z) + MAP_LENGTH;
    //collisionMatrix[enemyCollisionX][enemyCollisionZ] = NOTHING;

    enemyWalk(enemy);

    // sets true to the new enemy position in the collision matrix
    //enemyCollisionX = ((int) enemy->x) + MAP_WIDTH;
    //enemyCollisionZ = ((int) enemy->z) + MAP_LENGTH;
    //collisionMatrix[enemyCollisionX][enemyCollisionZ] = ENEMY;

}
void enemyWalk(Point3d* enemyPosition) {
    GLfloat deltaX = penguinPosition->x - enemyPosition->x;
    GLfloat deltaZ = penguinPosition->z - enemyPosition->z;
    GLfloat distance = sqrt( pow(deltaX, 2) + pow(deltaZ, 2) );

    Point3d* OldEnemyPosition = enemy;

    if (distance < 100.0f) {
            enemy->x += (deltaX * ENEMY_SPEED);
            if(collidesAt(enemy)) {
                enemy->x -= (deltaX * ENEMY_SPEED);
            }

            enemy->z += (deltaZ * ENEMY_SPEED);
            if(collidesAt(enemy)) {
                enemy->z -= (deltaZ * ENEMY_SPEED);
            }
    }
    //else printf("\n");
}

/**
Initialize openal and check for errors
*/
void soundInit() {
}
