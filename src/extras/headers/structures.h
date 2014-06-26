
typedef struct Point3d {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Point3d;

typedef struct Window {
    GLint id;
    GLsizei width;
    GLsizei height;
} Window;

typedef struct Mouse {
    GLfloat x;
    GLfloat y;
} Mouse;

typedef enum { NOTHING, SOLID_BLOCK, THROWABLE_BLOCK, ENEMY, PENGUIN } OBJ_ENUM;
typedef enum { TOP, BOTTOM, LEFT, RIGHT, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, NOCOLLISION} COLLISION_ENUM;
