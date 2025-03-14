//MAX BEAM


//https://stackoverflow.com/questions/41269788/how-i-can-add-auto-texture-coordinates-in-opengl for binding texture to bezier surface

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#define PI 3.14159
#define MAX_NUM_PARTICLES 1000
#define INITIAL_NUM_PARTICLES 100
#define INITIAL_POINT_SIZE 2.0
#define INITIAL_SPEED 10.0



#define INITIAL_RECT_SIZE 0.03
#define INITIAL_SPEEDW 0.5




using namespace std;

// Begin globals.

float doorPos = 0.0f;
int num_particles;
float point_size;
float rect_size;
float speed;
float speedW;
static float meX = 0, meY = 2, meZ = 0;
static float angle = 0;
static float stepsize = 1.0;
static float turnsize = 10.0;
static int localViewer = 1;
static float m = 0.2;
static float spotAngle = 50.0; 
static float spotExponent = 2.0;
float sunBrightness = 1.0;
bool doorOpen = false;
bool lampOn = false;
bool isWinter = false;
bool isFoggy = false;
bool isRaining = false;
bool lightOn = false;
static int fogMode = GL_LINEAR; 
static float fogDensity = 0.01; 
static float fogStart = 0.0;
static float fogEnd = 100.0;
static unsigned int texture[7]; 
static float theta = 180.0;
double inGameTime = 12.0;
float quadraticAttenuation = 0.001;
bool pastDoor = false;

int mouseX = 0;
bool isDragging = false;
float objectX = 0.0f;

static float controlPoints[6][4][3] =
{
    {{-3.0, 0.0, 5.0}, {-0.25, 0.0, 5.0}, {0.25, 0.0, 5.0}, {3.0, 0.0, 5.0}},
    {{-3.0, 0.0, 3.0}, {-0.25, 12.6, 3.0}, {0.25, 0.0, 3.0}, {3.0, 0.0, 3.0}},
    {{-3.0, 0.0, 1.0}, {-0.25, 0.0, 1.0}, {0.25, 0.0, 1.0}, {3.0, 0.0, 1.0}},
    {{-3.0, 0.0, -1.0}, {-0.25, 0.0, -1.0}, {0.25, -5.1, -1.0}, {3.0, 0.0, -1.0}},
    {{-3.0, 0.0, -3.0}, {-0.25, -0.3, -3.0}, {0.25, 10.4, -3.0}, {3.0, 0.0, -3.0}},
    {{-3.0, 0.0, -5.0}, {-0.25, 0.0, -5.0}, {0.25, 0.0, -5.0}, {3.0, 0.0, -5.0}},
};

static int rowCount = 0, columnCount = 0; 

int LAMP = 1;
int SHADE = 2;
int RAINB = 3;
int WINTERB = 4;
int FOGB = 5;
int DOOR = 6;
int LIGHT = 7;

int itemID = 0;
int height, width;
bool selecting = false;
int xClick, yClick; 


struct BitMapFile
{
    int sizeX;
    int sizeY;
    unsigned char* data;
};

typedef struct particle {
    float position[3];
    float velocity[3];
} particle;

typedef struct particleW {
    float position[3];
    float velocity[3];
} particleW;

particleW particlesW[MAX_NUM_PARTICLES];
particle particles[MAX_NUM_PARTICLES];


void animateDoor(int value) {
    if (doorOpen && doorPos < 3.0f) {
        doorPos += 0.1f; 
        glutPostRedisplay(); 
        glutTimerFunc(20, animateDoor, 0); 
    }
    else if (!doorOpen && doorPos > 0.0f) {
        doorPos -= 0.1f; 
        glutPostRedisplay(); 
        glutTimerFunc(20, animateDoor, 0); 
    }
}

void onMouseMove(int x, int y) {
    mouseX = x;
    //cout << mouseX << " \n";
    glutPostRedisplay(); 
}


void mouseControl(int button, int state, int x, int y)
{
    
    
    if (state == GLUT_DOWN && button == GLUT_LEFT)
    {
        selecting = true;
        xClick = x;
        yClick = 500 - y;
       
        
        //if (!isDragging) mouseX = x;
        //cout << "X: " << x << " and Y: " << (800 - y);
        glutPostRedisplay();
    }
    //if (!isDragging) mouseX = x;
    isDragging = (state == GLUT_DOWN); 
    //cout << isDragging << "\n";
    
}




void getID(int x, int y)
{
    unsigned char pixel[3];
    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);


    if ((int)pixel[0] == 10 && (int)pixel[1] == 10 && (int)pixel[2] == 10)
    {
        itemID = LAMP; 
    }
    else if ((int)pixel[0] == 20 && (int)pixel[1] == 20 && (int)pixel[2] == 20)
    {
        itemID = SHADE;  
    }
    else if ((int)pixel[0] == 30 && (int)pixel[1] == 30 && (int)pixel[2] == 30)
    {
        itemID = RAINB;  
    }
    else if ((int)pixel[0] == 40 && (int)pixel[1] == 40 && (int)pixel[2] == 40)
    {
        itemID = WINTERB;
    }
    else if ((int)pixel[0] == 50 && (int)pixel[1] == 50 && (int)pixel[2] == 50)
    {
        itemID = FOGB;
    }
    else if ((int)pixel[0] == 60 && (int)pixel[1] == 60 && (int)pixel[2] == 60)
    {
        itemID = DOOR; 
    }
    else if ((int)pixel[0] == 70 && (int)pixel[1] == 70 && (int)pixel[2] == 70)
    {
        itemID = LIGHT;
    }
    else itemID = 0;   

    int RAINB = 3;
    int WINTERB = 4;
    int FOGB = 5;

    selecting = false;
    glutPostRedisplay();
}

void checkCollision() {
  
    if (!doorOpen) {
        if (meZ < -8.5) {
            meZ = -8.5;
        }
        if (meX < -22.0) {
            meX = -22.0;
        }
        if (meX > 22.0) {
            meX = 22.0;
        }
        if (meZ > 0.5) {
            meZ = 0.5;
        }
    }
    else {
        if (meZ < -27.0f) {
            meZ = -27.0f;
        }
        if (meX < -8.0) {
            meX = -8.0;
        }
        if (meX > 8.0) {
            meX = 8.0;
        }
    }
    
  
    
    
   
}


BitMapFile* getBMPData(string filename)
{
    BitMapFile* bmp = new BitMapFile;
    unsigned int size, offset, headerSize;

    ifstream infile(filename.c_str(), ios::binary);

    infile.seekg(10);
    infile.read((char*)&offset, 4);

    infile.read((char*)&headerSize, 4);

    infile.seekg(18);
    infile.read((char*)&bmp->sizeX, 4);
    infile.read((char*)&bmp->sizeY, 4);

    size = bmp->sizeX * bmp->sizeY * 24;
    bmp->data = new unsigned char[size];

    // Read bitmap data.
    infile.seekg(offset);
    infile.read((char*)bmp->data, size);

    // Reverse color from bgr to rgb.
    int temp;
    for (int i = 0; i < size; i += 3)
    {
        temp = bmp->data[i];
        bmp->data[i] = bmp->data[i + 2];
        bmp->data[i + 2] = temp;
    }

    return bmp;
}

// Load external textures.
void loadExternalTextures()
{
    // Local storage for bmp image data.
    BitMapFile* image[7];

    // Load the textures.
    image[0] = getBMPData("mrbeamTEXTURES/grass.bmp");
    image[1] = getBMPData("mrbeamTEXTURES/sky.bmp");
    image[2] = getBMPData("mrbeamTEXTURES/nightsky.bmp");
    image[3] = getBMPData("mrbeamTEXTURES/snow.bmp");
    image[4] = getBMPData("mrbeamTEXTURES/hallwayFloor.bmp");
    image[5] = getBMPData("mrbeamTEXTURES/wood.bmp");
    image[6] = getBMPData("mrbeamTEXTURES/logo.bmp");

    // Bind grass image to texture index[0]. 
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);

    // Bind sky image to texture index[1]
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);

    // Bind night sky image to texture index[2]
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3]->sizeX, image[3]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[3]->data);

    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[4]->sizeX, image[4]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[4]->data);

    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[5]->sizeX, image[5]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[5]->data);

    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[6]->sizeX, image[6]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[6]->data);
}

void updateTime(int value) {

    inGameTime += 0.01; 
    if (inGameTime >= 24.0) {
        inGameTime -= 24.0; 
    }

    theta = 180.0 - (inGameTime / 24.0) * 180.0;

    if (inGameTime < 12.0) {
        sunBrightness = inGameTime / 12.0; // Brighter as it approaches 12
    }
    else {
        sunBrightness = 1.0 - ((inGameTime - 12.0) / 12.0); // Darker as it approaches 24
    }

    //cout << theta << "  ";
    glutPostRedisplay();
    glutTimerFunc(10, updateTime, 0);
}

// Routine to draw a stroke character string.
void writeStrokeString(void* font, const char* string)
{
    const char* c;

    for (c = string; *c != '\0'; c++) glutStrokeCharacter(font, *c);
}

void displayClock() {
    int hours = static_cast<int>(inGameTime);
    int minutes = static_cast<int>((inGameTime - hours) * 60);

 
    char timeString[10]; 
    timeString[0] = '0' + (hours / 10); 
    timeString[1] = '0' + (hours % 10); 
    timeString[2] = ' ';
    timeString[3] = ':'; 
    timeString[4] = '0' + (minutes / 10); 
    timeString[5] = '0' + (minutes % 10); 
    timeString[6] = '\0'; 

    //cout << timeString;

    //glDisable(GL_LIGHTING);
   // glScalef(0.0005f, 0.0005f, 0.0005f);

    writeStrokeString(GLUT_STROKE_ROMAN, timeString);
   // glEnable(GL_LIGHTING);
}



// Initialization routine.
void setup(void)
{
    num_particles = INITIAL_NUM_PARTICLES;
    point_size = INITIAL_POINT_SIZE;
    speed = INITIAL_SPEED;

    for (int i = 0; i < num_particles; i++) {
        particles[i].position[0] = 2.0 * ((float)rand() / RAND_MAX) - 1.0;
        particles[i].position[1] = 2.0 * ((float)rand() / RAND_MAX) - 1.0;
        particles[i].position[2] = 2.0 * ((float)rand() / RAND_MAX) - 1.0;
        particles[i].velocity[0] = 0.0;
        particles[i].velocity[1] = -speed * ((float)rand() / RAND_MAX + 0.5);
        particles[i].velocity[2] = 0.0;
    }

    glPointSize(point_size);

    //Winter particles
    num_particles = INITIAL_NUM_PARTICLES;
    rect_size = INITIAL_RECT_SIZE;
    speedW = INITIAL_SPEEDW;

    for (int i = 0; i < num_particles; i++) {
        particlesW[i].position[0] = 2.0 * ((float)rand() / RAND_MAX) - 1.0;
        particlesW[i].position[1] = 2.0 * ((float)rand() / RAND_MAX) - 1.0;
        particlesW[i].position[2] = 2.0 * ((float)rand() / RAND_MAX) - 1.0;
        particlesW[i].velocity[0] = 0.0;
        particlesW[i].velocity[1] = -speedW * ((float)rand() / RAND_MAX + 0.1); // Slower falling speed
        particlesW[i].velocity[2] = 0.0;
    }

 
    glClearColor(1.0, 1.0, 1.0, 0.0);


    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glGenTextures(6, texture);
   loadExternalTextures();

   glEnable(GL_TEXTURE_2D);   

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

   glEnable(GL_BLEND); 
}

void myIdle() {
    float dt;
    static int last_time = 0;
    int present_time;

    present_time = glutGet(GLUT_ELAPSED_TIME);
    dt = 0.001 * (present_time - last_time);

    if (isWinter) {
        for (int i = 0; i < num_particles; i++) {
            particlesW[i].position[0] += dt * particlesW[i].velocity[0];
            particlesW[i].position[1] += dt * particlesW[i].velocity[1];
            particlesW[i].position[2] += dt * particlesW[i].velocity[2];

     
            if (particlesW[i].position[1] < -1.0)
                particlesW[i].position[1] = 1.0;

            particlesW[i].position[0] += 0.01 * ((float)rand() / RAND_MAX - 0.5);
        }
    }
    else {
        for (int i = 0; i < num_particles; i++) {
            particles[i].position[0] += dt * particles[i].velocity[0];
            particles[i].position[1] += dt * particles[i].velocity[1];
            particles[i].position[2] += dt * particles[i].velocity[2];

            if (particles[i].position[1] < -1.0)
                particles[i].position[1] = 1.0;

       
            particles[i].position[0] += 0.01 * ((float)rand() / RAND_MAX - 0.5);
        }
    }

    last_time = present_time;
    glutPostRedisplay();
}

void drawTree() {

    float trunkAmbient[] = { 0.54, 0.27, 0.07, 1.0 };
    float trunkDiffuse[] = { 0.54, 0.27, 0.07, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, trunkAmbient);

  
    glPushMatrix();
    glScalef(0.2f, 1.0f, 0.2f); 
    glutSolidCube(1.0);
    glPopMatrix();

    
    float leavesAmbient[] = { 0.0, 0.5, 0.0, 1.0 };
    float leavesDiffuse[] = { 0.0, 0.5, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, leavesAmbient);

    
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f); 
    glutSolidSphere(0.5, 20, 20); 
    glPopMatrix();
}

void drawTreeW() {
    
    float trunkAmbient[] = { 0.54f, 0.27f, 0.07f, 1.0f };
    float trunkDiffuse[] = { 0.54f, 0.27f, 0.07f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, trunkAmbient);

    
    glPushMatrix();
    glScalef(0.2f, 1.0f, 0.2f); 
    glutSolidCube(1.0);
    glPopMatrix();

   
    float leavesAmbient[] = { 0.0f, 0.5f, 0.0f, 1.0f };
    float leavesDiffuse[] = { 0.0f, 0.5f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, leavesAmbient);
    glPushMatrix();
    glTranslatef(0.0f, -0.6f, 0.0f);
  
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f);
    glRotatef(90.0, -1.0, 0.0, 0.0);
    glutSolidCone(0.4, 0.5, 20, 20);
    glPopMatrix();

   
    glPushMatrix();
    glTranslatef(0.0f, 0.9f, 0.0f);
    glRotatef(90.0, -1.0, 0.0, 0.0);
    glutSolidCone(0.3, 0.5, 20, 20);
    glPopMatrix();

  
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.0f);
    glRotatef(90.0, -1.0, 0.0, 0.0);
    glutSolidCone(0.2, 0.5, 20, 20);
    glPopMatrix();

    glPopMatrix();

}

void drawTable(void) {
    float tableAmbAndDif[] = { 0.5, 0.3, 0.0, 1.0 };
    float tableSpec[] = { 1.0, 0.7, 0,7, 0. };
    float tableShine[] = { 70.0 };

   
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tableAmbAndDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, tableSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, tableShine);

    glColor3f(0.4, 0.2, 0.0); 
    glPushMatrix();
    glTranslatef(-0.6, 0.0, -0.4); 
    glScalef(0.1, 1.0, 0.1); 
    glutSolidCube(1.0); 
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.6, 0.0, -0.4);
    glScalef(0.1, 1.0, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.6, 0.0, 0.4);
    glScalef(0.1, 1.0, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.6, 0.0, 0.4);
    glScalef(0.1, 1.0, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();





    glColor3f(0.5, 0.3, 0.0); 
    glPushMatrix();
    glTranslatef(0.0, 0.5, 0.0); 
    glScalef(1.5, 0.1, 1.0); 
    glutSolidCube(1.0); 
    glPopMatrix();


    //Top for lighting
    

   
}


void drawShapes(void) 
{
    
    float alpha; 
    //cout << meX << ", " << meY << ", " << meZ << "\n";
 
    float lightPos[] = { cos((PI / 180.0) * theta), sin((PI / 180.0) * theta), 0.0, 0.0 };
    float fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    float globAmb[] = { m, m, m, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); 
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //FOG
    if (itemID == FOGB) {
        isFoggy = !isFoggy;
        itemID = 0;
    }
    if (isFoggy) glEnable(GL_FOG);
    else glDisable(GL_FOG);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, fogMode);
    glFogf(GL_FOG_START, fogStart);
    glFogf(GL_FOG_END, fogEnd);
    glFogf(GL_FOG_DENSITY, fogDensity);

    gluLookAt(meX, meY, meZ,
        meX - sin(angle * PI / 180), meY, meZ - cos(angle * PI / 180),
        0, 1, 0);
    float lightAmb[] = { 0.2, 0.2, 0.2, 1.0 };
    float lightDifAndSpec0[] = { 1.0, 1.0, 1.0, 1.0 };
    float spotDirection[] = { 0.0, -1.0, 0.0 };   //Z y -1
    float lightPos0[] = { 0.0, 4.9, -9.45, 1 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);

    float sunAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float sunDifAndSpec[] = { sunBrightness, sunBrightness, sunBrightness, 1.0 };
    float sunPos[] = { 0.0, 1.0, 0.0, 0.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, sunAmb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDifAndSpec);
    glLightfv(GL_LIGHT1, GL_SPECULAR, sunDifAndSpec);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);


    float bulbAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float bulbDifAndSpec[] = { 0.5, 0.5, 0.5, 1.0 };
    float bulbPos[] = { 0.0, 8.0, -20.0, 1.0 };

    glLightfv(GL_LIGHT2, GL_AMBIENT, bulbAmb);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, bulbDifAndSpec);
    glLightfv(GL_LIGHT2, GL_SPECULAR, bulbDifAndSpec);
    glLightfv(GL_LIGHT2, GL_POSITION, bulbPos);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);

    float lampAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float lampDifAndSpec0[] = { 1.0, 0.0, 0.0, 1.0 };
    float lampDirection[] = { 0.0, -1.0, 0.5 };   //Z y -1
    float lampPos[] = { 8.0, 2.9, -15.35, 1.0};
    float lampAngle = 50.0;
    float lampExponent = 2.0;

    glLightfv(GL_LIGHT3, GL_AMBIENT, lampAmb);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lampDifAndSpec0);
    glLightfv(GL_LIGHT3, GL_SPECULAR, lampDifAndSpec0);
    glLightfv(GL_LIGHT3, GL_POSITION, lampPos);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, lampAngle);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, lampDirection);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, lampExponent);



    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); 
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    if (lightOn) {
        glEnable(GL_LIGHT2);
    }
    else {
        glDisable(GL_LIGHT2);
    }
    
    if (lampOn) {
        glEnable(GL_LIGHT3);
    }
    else {
        glDisable(GL_LIGHT3);
    }
  

    
    float groundAmbAndDif[] = { 0.0, 1.0, 0.0, 1.0 };
    float groundSpec[] = { 1.0, 1.0, 1,0, 1.0 };
    float groundShine[] = { 1000.0 };

    float groundAmbAndDifW[] = { 1.0, 1.0, 1.0, 1.0 };
    float groundSpecW[] = { 1.0, 1.0, 1,0, 1.0 };
    float groundShineW[] = { 1000.0 };
 

 
    

    //GROUND
   // glDisable(GL_LIGHTING);
    //glScalef(1.0f, 1.0f, 1.0f);
   // glColor3f(0.0, 0.0, 0.0);
   // writeStrokeString(GLUT_STROKE_ROMAN, "timeString");
   // glEnable(GL_LIGHTING);
    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisable(GL_TEXTURE_2D);
        glColor3ub(55, 55, 55);
        //cout << "HI";
    }
    else {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpec);
        glMaterialfv(GL_FRONT, GL_SHININESS, groundShine);
        glEnable(GL_TEXTURE_2D);
        if (isWinter) {
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDifW);
            glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpecW);
            glMaterialfv(GL_FRONT, GL_SHININESS, groundShineW);
        }
    }


    //glTranslatef(0.0, 0.0, -70.0);
    //glScaled(1.5, 1.5, 1.5);
    //glBlendFunc(GL_ZERO, GL_ZERO); // Specify blending parameters to overwrite background.
    if (isWinter) {
        glBindTexture(GL_TEXTURE_2D, texture[3]);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, texture[0]);
    }
    
    //glBegin(GL_POLYGON);
    //glNormal3f(0.0, 1.0, 0.0);
    //glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 0.0, 0.0);
    //glTexCoord2f(8.0, 0.0); glVertex3f(100.0, 0.0, 0.0);
   // glTexCoord2f(8.0, 8.0); glVertex3f(100.0, 0.0, -300.0);
   // glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, 0.0, -300.0);
   // glEnd();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.0, 0.0);
    glTexCoord2f(4.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(4.0, 4.0); glVertex3f(0.0, 0.0, -100.0);
    glTexCoord2f(0.0, 4.0); glVertex3f(-200.0, 0.0, -100.0);
    glEnd();



    glPushMatrix();
    glTranslatef(200.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.0, 0.0);
    glTexCoord2f(4.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(4.0, 4.0); glVertex3f(0.0, 0.0, -100.0);
    glTexCoord2f(0.0, 4.0); glVertex3f(-200.0, 0.0, -100.0);
    glEnd();

    glTranslatef(0.0, 0.0, -100.0);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.0, 0.0);
    glTexCoord2f(4.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(4.0, 4.0); glVertex3f(0.0, 0.0, -100.0);
    glTexCoord2f(0.0, 4.0); glVertex3f(-200.0, 0.0, -100.0);
    glEnd();

    glTranslatef(-200.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.0, 0.0);
    glTexCoord2f(4.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(4.0, 4.0); glVertex3f(0.0, 0.0, -100.0);
    glTexCoord2f(0.0, 4.0); glVertex3f(-200.0, 0.0, -100.0);
    glEnd();

    
    glPopMatrix();





    glDisable(GL_LIGHTING); 


    //SKY

    glBlendFunc(GL_ONE, GL_ZERO); 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, texture[2]);    glDisable(GL_DEPTH_TEST);

    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.0, -150.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(200.0, 0.0, -150.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(200.0, 240.0, -150.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 240.0, -150.0);
    glEnd();

    // The blending parameter varies with the angle of the sun.
    if (theta <= 90.0) alpha = theta / 90.0;
    else alpha = (180.0 - theta) / 90.0;
    glColor4f(1.0, 1.0, 1.0, alpha);


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Specify blending parameters to mix skies.
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 0.0, -150.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(200.0, 0.0, -150.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(200.0, 240.0, -150.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 240.0, -150.0);
    glEnd();
    glEnable(GL_DEPTH_TEST);


    glEnable(GL_LIGHTING);
    //glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[4]);

    glDisable(GL_LIGHT1);

    //Hallway Ground
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    float groundHAmbAndDif[] = { 1.0, 0.0, 0.0, 1.0 };
    float groundHSpec[] = { 1.0, 1.0, 1,0, 1.0 };
    float groundHShine[] = { 50.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDifW);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundAmbAndDifW);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundAmbAndDifW);

    double q = 25.0;
    //PT1
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(0.0, 0.05, -10.0);
    glScalef(24.0, 1.0, 12.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    //glNormal3f(0.0, 0.0, 1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glTexCoord2f(2 * (c / (q / 2)), 2 * (r / q));  glVertex3f(c / q, r / q, 0.0);
            glTexCoord2f(2 * (c / (q / 2)), 2 * ((r + 1) / q));  glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //PT 2
    glPushMatrix();
    glColor3f(0.8, 0.8, 0.8);
    glTranslatef(-24.0, 0.05, -10.0);
    glScalef(24.0, 1.0, 12.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    //glNormal3f(0.0, 0.0, 1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glTexCoord2f(2 * (c / (q/2)), 2 * (r / q));  glVertex3f(c / q, r / q, 0.0);
            glTexCoord2f(2 * (c / (q/2)), 2 * ((r + 1) / q));  glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    glEnable(GL_LIGHT1);



    //Bezier
    //glDisable(GL_LIGHTING);
    if (isWinter) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDifW);
        glBindTexture(GL_TEXTURE_2D, texture[3]);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDif);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
    }
    glPushMatrix();
    glFrontFace(GL_CW);
    glScalef(15.0, 15.0, 15.0);
    glTranslatef(0.0, 0.0, -10);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 6, controlPoints[0][0]);
    glEnable(GL_MAP2_VERTEX_3);
    
    
    //glColor3f(0.0, 0.0, 0.0);
    //****REFERENCE used source for GENT_T and GEN_S
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_S);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    
    glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
    glEvalMesh2(GL_FILL, 0, 20, 0, 20);
    glPopMatrix();
    glFrontFace(GL_CCW);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_S);

    glDisable(GL_LIGHT1);

    //glEnable(GL_LIGHTING);

    
    //glEnable(GL_LIGHTING);

    //PT2

    //glDisable(GL_LIGHTING);
    
    //glEnable(GL_LIGHTING);

    
    float wallAmbAndDif[] = { 0.956f, 0.871f, 0.702f, 1.0f };
    float wallSpec[] = { 1.0, 1.0, 1,0, 1.0 };
    float wallShine[] = { 50.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wallAmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wallSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, wallShine);
    //Hallway
    //FRONT WALL
    //P1
    if (meZ < -10.0) {
        glDisable(GL_LIGHT0);
    
    }
    glNormal3f(0.0, 0.0, -1.0);
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(1.5, 0.0, -10.0);
    glScalef(22.5, 6.5, 1.0);
    
    //glNormal3f(0.0, 0.0, 1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();
    //P2
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(-24.0, 0.0, -10.0);
    glScalef(22.5, 6.5, 1.0);
    //glRotatef(90.0, 1.0, 0.0, 0.0);
    //glNormal3f(0.0, 0.0, 1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //P3 (ABOVE DOOR)
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(-1.5, 4.0, -10.0);
    glScalef(3.0, 2.5, 1.0);
    //glRotatef(90.0, 1.0, 0.0, 0.0);
    //glNormal3f(0.0, 0.0, 1.0);
    q = 10;
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();
    //glEnable(GL_LIGHTING);

    //P4 DOOR
    
    float doorAmbAndDif[] = { 0.6, 0.6, 0.7, 1.0 };
    float doorSpec[] = { 1.0, 1.0, 1,0, 1.0 };
    float doorShine[] = { 50.0 };

    

    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glColor3ub(60, 60, 60);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, doorAmbAndDif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, doorSpec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, doorShine);
    }

    if (itemID == DOOR) {
        doorOpen = !doorOpen;
        glutTimerFunc(20, animateDoor, 0);
        itemID = 0;
    }

    glPushMatrix();
    //glColor3f(0.0, 0.0, 0.0);
    glTranslatef(-1.5, doorPos, -10.0);
    glScalef(3.0, 4.0-doorPos, 1.0);
    //glRotatef(90.0, 1.0, 0.0, 0.0);
    //glNormal3f(0.0, 0.0, 1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

   


    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glColor3ub(34, 76, 21);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wallAmbAndDif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wallSpec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, wallShine);
    }

    if (meZ < -10.0) {
        glEnable(GL_LIGHT0);

    }

    
    q = 25;
   
    //CEILING
    glPushMatrix();
    glColor3f(0.0, 1.0, 0.0);
    glTranslatef(0.0, 6.0, -3.9);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(12.0, 3.0, 0.07);
    glutSolidCube(4.0);
    glPopMatrix();

    //BACK WALL
    glFrontFace(GL_CW);
    glPushMatrix();
    glNormal3f(0.0, 0.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, 2.0);
    glScalef(12.0, 3.0, 0.07);
    glutSolidCube(4.0);
    glPopMatrix();


    //LEFT WALL
    glPushMatrix();
    //glNormal3f(0.0, 0.0, -1.0);
    glColor3f(0.0, 0.0, 1.0);
    glTranslatef(-24.0, 0.0, -4.0);
    glScalef(0.07, 3.0, 3.0);
    glutSolidCube(4.0);
    glPopMatrix();

    //RIGHT WALL
    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);
    glTranslatef(24.0, 0.0, -4.0);
    glScalef(0.07, 3.0, 3.0);
    glutSolidCube(4.0);
    glPopMatrix();

    glFrontFace(GL_CCW);

    //HALL LIGHT1
    glDisable(GL_LIGHTING);

    //FIXTURE
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0, 5, -9.45);
    glScalef(3.5, 0.2, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();
    //glEnable(GL_LIGHTING);

   
    //BULB
    if (itemID == LIGHT) {
        lightOn = !lightOn;
        itemID = 0;
    }

    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
    glScalef(3.1, 0.15, 0.7);
    glutSolidCube(1.0);
    glPopMatrix();
    glEnable(GL_LIGHTING);
  //  glEnable(GL_LIGHTING);

    //
    //ROOM LEFT WALL
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glNormal3f(0.0, 0.0, -1.0);
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(-10.0, 0.0, -10.0);
    glScalef(1.0, 10.0, 20.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();


    //ROOM RIGHT WALL
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(10.0, 0.0, -10.0);
    glScalef(1.0, 10.0, 20.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    //glNormal3f(0.0, 0.0, 1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //SU POSTER

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDifW);
    glNormal3f(0.0, 0.0, 1.0);
   
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    //Hallway Ground
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(9.95, 3.0, -19.0);
    glScalef(1.0, 5.0, 10.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);

    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 1.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //glEnable(GL_LIGHTING);
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wallAmbAndDif);

    //ROOM BACK WALL
    //Pt1 (Bottom)
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(-10.0, 0.0, -30.0);
    glScalef(20.0, 1.0, 0.0);
    //glRotatef(90.0, 0.0, 1.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //Side left
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(-10.0, 0.0, -30.0);
    glScalef(7.0, 7.0, 0.0);
    //glRotatef(90.0, 0.0, 1.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //Side right
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(3.0, 0.0, -30.0);
    glScalef(7.0, 7.0, 0.0);
    //glRotatef(90.0, 0.0, 1.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //Pt2 (Top)
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(-10.0, 5.0, -30.0);
    glScalef(20.0, 5, 0.0);
    //glRotatef(90.0, 0.0, 1.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();


    

    //glEnable(GL_LIGHTING);

    //ROOM CEILING
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wallAmbAndDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, wallSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, wallShine);
    glColor3f(0.0, 0.0, 1.0);
    glTranslatef(-10.0, 10.0, -30.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(20.0, 20.0, 0.0);
    
    glNormal3f(0.0, 1.0, 0.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //ROOM FLOOR
    float rFloorAmbAndDif[] = { 0.6, 0.3, 0.1, 1.0 };
  //  float groundHSpec[] = { 1.0, 1.0, 1,0, 1.0 };
  //  float groundHShine[] = { 50.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rFloorAmbAndDif);
  //  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundAmbAndDifW);
  //  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundAmbAndDifW);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);
    glTranslatef(-10.0, 0.01, -30.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(20.0, 20.0, 0.0);

    glNormal3f(0.0, 1.0, 0.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glTexCoord2f(2 * (c / q), 2 * (r / q));  glVertex3f(c / q, r / q, 0.0);
            glTexCoord2f(2 * (c / q), 2 * ((r + 1) / q));  glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHT0);
    /////

    //FRONT WALL FILL
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wallAmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wallSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, wallShine);
    glPushMatrix();
    glColor3f(0.0, 1.0, 0.0);
    glTranslatef(-10.0, 6.15, -10.0);
    glScalef(21.0, 5.0, 1.0);
    //glRotatef(90.0, 0.0, 1.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    for (int r = 0; r < q; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= q; c++)
        {
            glVertex3f(c / q, r / q, 0.0);
            glVertex3f(c / q, (r + 1) / q, 0.0);
        }
        glEnd();
    }
    glPopMatrix();

    //LIGHT
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0.0, 8.0, -20.0);
    if (lightOn) {
        glColor3f(1.0, 1.0, 1.0);
    }
    else {
        glColor3f(0.3, 0.3, 0.3);
    }
    //glColor3f(1.0, 1.0, 1.0);
    glutSolidSphere(1.0, 15.0, 15.0);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    
    
    //TABLE
    glPushMatrix();
    glTranslatef(8.0, 0.0, -14.0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    glScalef(4.5, 2.5, 3.5);
    drawTable();
    glPopMatrix();

    //table top (for lighting)
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(6.3, 1.378, -17.3);
    glRotated(90.0, 1.0, 0.0, 0.0);
    glScalef(3.5, 6.7, 1.0);
    float n = 25;
    //glScalef(24.0, 1.0, 12.0);
    //glRotatef(90.0, 1.0, 0.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    for (int r = 0; r < n; r++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int c = 0; c <= n; c++)
        {
            glVertex3f(c / n, r / n, 0.0);
            glVertex3f(c / n, (r + 1) / n, 0.0);
        }
        glEnd();
    }
    glPopMatrix();
    //glEnable(GL_LIGHTING);

    //Lamp
    if (itemID == LAMP) {
        lampOn = !lampOn;
        itemID = 0;
    }
    

    float lampAmbAndDif[] = { 0.0, 0.0, 0.0, 1.0 };
    float lampSpec[] = { 1.0, 1.0, 1,0, 1.0 };
    float lampShine[] = { 50.0 };

   
    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glColor3ub(10, 10, 10);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lampAmbAndDif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lampSpec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, lampShine);
    }
    glPushMatrix();
    glTranslatef(8.0, 1.65, -16.0);
    glScalef(0.5, 0.2, 0.5);
    glRotated(90.0, 1.0, 0.0, 0.0);
    //glColor3f(0.5, 0.5, 0.5); 
    GLUquadricObj* cylinder = gluNewQuadric();
    gluCylinder(cylinder, 1.0, 1.0, 2.0, 20, 20); 

    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.0, 2.5, -16.0);
    glScalef(0.15, 2.0, 0.15);
    //glRotated(90.0, 1.0, 0.0, 0.0);
    glutSolidCube(1);
    //glColor3f(0.5, 0.5, 0.5);
    
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.0, 3.25, -15.77);
    glRotated(130.0, 1.0, 0.0, 0.0);
    glScalef(0.15, 0.65, 0.15);
    
    glutSolidCube(1);


    glPopMatrix();

    //Light cone
    glPushMatrix();
    glTranslatef(8.0, 2.8, -15.3);
    glRotated(240.0, 1.0, 0.0, 0.0);
    glScalef(0.4, 0.4, 0.55);

    glutSolidCone(0.7, 1, 10, 10);

    glPopMatrix();

    

    //Lamp bulb
    glDisable(GL_LIGHTING);
    glPushMatrix();
    if (!lampOn) {

        glColor3f(0.4, 0.4, 0.4);
    }
    else {
        glColor3f(1.0, 0.0, 0.0);
    }
    
    glTranslatef(lampPos[0], lampPos[1], lampPos[2]);
    //glRotated(240.0, 1.0, 0.0, 0.0);
    glScalef(0.13, 0.13, 0.13);

    glutSolidSphere(1.0, 5, 5);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    //ALARM STAND
    glPushMatrix();
    glColor3ub(56, 65, 77);
    glTranslatef(-7.0, 0.0, -15.0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    glScalef(2.0, 2.0, 2.0);
    drawTable();
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lampAmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lampSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, lampShine);

    //ALARM
    glPushMatrix();
    //glColor3f(0.0, 0.0, 0.0);
    glTranslatef(-7.0, 1.5, -15.0);
    glScalef(1.0, 0.5, 1.0);
    glutSolidCube(1);
    glPopMatrix();

    //ALARM SCREEN
    float alarmAmbAndDif[] = { 0.75, 0.0, 0.0, 1.0 };
    float alarmSpec[] = { 1.0, 1.0, 1,0, 1.0 };
    float alarmShine[] = { 50.0 };
    float alarmEmission[] = { 0.75, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_EMISSION, alarmEmission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lampAmbAndDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lampSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, lampShine);
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(-6.5, 1.5, -15.0);
    glScalef(0.1, 0.4, 0.8);
    glutSolidCube(1);
    glPopMatrix();
    //glEnable(GL_LIGHTING);

    float blackColor[] = { 0.0, 0.0, 0.0, 1.0 }; 
    glMaterialfv(GL_FRONT, GL_EMISSION, blackColor);

    //ALARM TEXT
    //updateTime();
    //displayClock();
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(-6.42, 1.5, -14.75);
    glRotatef(90, 0.0, 1.0, 0.0);
    glScalef(0.0012f, 0.0012f, 0.0012f);
    
    //glRotatef(180.0, 0.0, 1.0, 0.0);
    //glColor3f(0.0f, 0.0f, 0.0f); 

    displayClock();
    //updateTime();
    //writeStrokeString(GLUT_STROKE_ROMAN, "randomName.c_str()");
    glPopMatrix();
    glEnable(GL_LIGHTING);



    


    //BED
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDifW);
    glPushMatrix();
    glTranslatef(8.0, 0.0, -26.0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    glScalef(3.5, 2.0, 3.5);
    drawTable();
    glPopMatrix();

    //glDisable(GL_LIGHTING);
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundAmbAndDifW);
    glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, groundShine);
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(8.0, 1.3, -26.0);
    glScalef(3.0, 0.4, 4.7);
    glutSolidCube(1);
    glPopMatrix();
    //glEnable(GL_LIGHTING);


    //TREES
    if (!isWinter) {
        glPushMatrix();
        glTranslatef(-7.0, 2.5, -45.0);
        glScaled(5.0, 5.0, 5.0);
        drawTree();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(4.0, 2.5, -60.0);
        glScaled(3.0, 3.0, 3.0);
        drawTree();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(10.0, 15.0, -120.0);
        glScaled(3.0, 3.0, 3.0);
        drawTree();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-15.0, 8.0, -115.0);
        glScaled(3.0, 3.0, 3.0);
        drawTree();
        glPopMatrix();
    }
    else {
        glPushMatrix();
        glTranslatef(-7.0, 2.5, -45.0);
        glScaled(5.0, 5.0, 5.0);
        drawTreeW();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(4.0, 2.5, -60.0);
        glScaled(3.0, 3.0, 3.0);
        drawTreeW();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(10.0, 15.0, -120.0);
        glScaled(3.0, 3.0, 3.0);
        drawTreeW();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-15.0, 8.0, -115.0);
        glScaled(3.0, 3.0, 3.0);
        drawTreeW();
        glPopMatrix();
    }

    //RAIN
    if (isRaining && !isWinter) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0.0, 3.0, -34.0);
        glScalef(6.0, 13.0, 1.0);
        glColor3f(0.1, 0.1, 1.0); 
        for (int i = 0; i < num_particles; i++) {
            float x = particles[i].position[0];
            float y = particles[i].position[1];
            float z = particles[i].position[2];
            float size = 0.02; 
            float half_width = size / 2;
            float height = size * 2;

  
            glBegin(GL_POLYGON);
            glVertex3f(x - half_width, y - height / 2, z);
            glVertex3f(x + half_width, y - height / 2, z);
            glVertex3f(x + half_width, y + height / 2, z);
            glVertex3f(x - half_width, y + height / 2, z);
            glEnd();
        }
        glPopMatrix();
        glColor3f(0.7, 0.7, 0.7);
        glEnable(GL_LIGHTING);
    }
    if (isRaining && isWinter) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0.0, 3.0, -34.0);
        glScalef(6.0, 6.0, 1.0);
        glColor3f(1.0, 1.0, 1.0);
        for (int i = 0; i < num_particles; i++) {
            glPushMatrix();
            glTranslatef(particlesW[i].position[0], particlesW[i].position[1], 0.0);
            glBegin(GL_POLYGON);
            glVertex2f(-rect_size / 2, -rect_size / 2);
            glVertex2f(rect_size / 2, -rect_size / 2);
            glVertex2f(rect_size / 2, rect_size / 2);
            glVertex2f(-rect_size / 2, rect_size / 2);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
        glEnable(GL_LIGHTING);
    
    }


    
    float buttonAmbAndDifR[] = { 1.0, 0.0, 0.0, 1.0 };

    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glColor3ub(30, 30, 30);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, buttonAmbAndDifR);

    }

    if (itemID == RAINB) {
        isRaining = !isRaining;
        itemID = 0;
    }

    glPushMatrix();
    glTranslatef(-9.8, 2.0, -20.0);
    glScalef(0.2, 1.3, 0.7);
    glutSolidCube(1.0);
    glPopMatrix();

    float buttonAmbAndDifG[] = { 0.0, 1.0, 0.0, 1.0 };
    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glColor3ub(40, 40, 40);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, buttonAmbAndDifG);

    }

    if (itemID == WINTERB) {
        isWinter = !isWinter;
        itemID = 0;
    }

    

    glPushMatrix();
    glTranslatef(-9.8, 2.0, -23.0);
    glScalef(0.2, 1.3, 0.7);
    glutSolidCube(1.0);
    glPopMatrix();

    float buttonAmbAndDifL[] = { 1.0, 0.0, 1.0, 1.0 };
    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glColor3ub(50, 50, 50);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, buttonAmbAndDifL);

    }

    

    

    glPushMatrix();
    glTranslatef(-9.8, 2.0, -26.0);
    glScalef(0.2, 1.3, 0.7);
    glutSolidCube(1.0);
    glPopMatrix();


    float buttonAmbAndDifLight[] = { 1.0, 1.0, 0.0, 1.0 };
    if (selecting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glColor3ub(70, 70, 70);
    }
    else {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, buttonAmbAndDifLight);

    }

    
    glPushMatrix();
    glTranslatef(-9.8, 2.0, -17.0);
    glScalef(0.2, 1.3, 0.7);
    glutSolidCube(1.0);
    glPopMatrix();

    
    //WINDOW
   // glDisable(GL_LIGHTING);
    float windowAmbAndDif[] = { 0.956f, 0.871f, 0.702f, 0.05f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, windowAmbAndDif);
    glFrontFace(GL_CW);
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(0.0, 3.0, -30.0); 
    glScalef(6.0, 4.0, 0.0);
    glBegin(GL_POLYGON);

    glVertex3f(0.5, 0.5, 0.0); 
    glVertex3f(0.5, -0.5, 0.0); 
    glVertex3f(-0.5, -0.5, 0.0); 
    glVertex3f(-0.5, 0.5, 0.0);
    glEnd();
    
    glPopMatrix();


    //BLINDS
    //cout << isDragging << "\n";
    //cout << itemID;
    if (isDragging) {
        objectX = (float)mouseX / glutGet(GLUT_WINDOW_WIDTH) * 10.0f - 5.0f;
        if (objectX > 5.0f) objectX = 5.0f;
        if (objectX < 0.0f) objectX = 0.0f;

    }

    float blindAmbAndDif[] = { 0.5, 0.5, 0.5, 1.0 };
    glPushMatrix();
    glNormal3f(0.0, 0.0, 1.0);
    glColor3f(1.0, 1.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blindAmbAndDif);
    glTranslatef(objectX + 1.5, 3.0, -29.95); 
    glScalef(3.0, 4.0, 0.0); 
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 0.5, 0.0); 
    glVertex3f(0.5, -0.5, 0.0); 
    glVertex3f(-0.5, -0.5, 0.0); 
    glVertex3f(-0.5, 0.5, 0.0); 
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blindAmbAndDif);
    glTranslatef(-objectX - 1.5, 3.0, -29.95); 
    glScalef(3.0, 4.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 0.5, 0.0); 
    glVertex3f(0.5, -0.5, 0.0); 
    glVertex3f(-0.5, -0.5, 0.0);
    glVertex3f(-0.5, 0.5, 0.0); 
    glEnd();
    glFrontFace(GL_CCW);
    glPopMatrix();


    




}

// Drawing routine.
void drawScene(void)
{
    if (selecting) {
        drawShapes();
        getID(xClick, yClick);
        glutSwapBuffers();
    }
    else {
        drawShapes();
        glutSwapBuffers();
    }
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100, 1, 1, 200);
    //gluPerspective(60.0, (float)w / (float)h, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.

void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'c':
        lightOn = !lightOn;
        glutPostRedisplay();
        break;
    case 'd':
        doorOpen = !doorOpen;
        glutTimerFunc(20, animateDoor, 0);
        glutPostRedisplay();
        break;
    case 'l':
        lampOn = !lampOn;
        glutPostRedisplay();
        break;
    case 'r':
        isRaining = !isRaining;
        glutPostRedisplay();
        break;
    case 'w':
        isWinter = !isWinter;
        glutPostRedisplay();
        break;
    case 'f':
        isFoggy = !isFoggy;
        glutPostRedisplay();
        break;
   
    default:
        break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP: //forward

        meZ = meZ - stepsize * cos(angle * PI / 180);
        meX = meX - stepsize * sin(angle * PI / 180);
        checkCollision();
        break;
    case GLUT_KEY_DOWN: //back
        meZ += cos(angle * PI / 180) * stepsize;
        meX += sin(angle * PI / 180) * stepsize;
        checkCollision();
        break;
    case GLUT_KEY_RIGHT: //turn right
        angle -= turnsize;
        break;
    case GLUT_KEY_LEFT: //turn left
        angle = angle + turnsize;

        break;
    }//end switch
    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Press a and d to turn left and right respectively." << endl
        << "Press w and s to move forwards and backwards respectively." << endl
        << "Press c to turn on/off the ceiling light." << endl
        << "Press d to open and close door." << endl
        << "Press l to tunr on and off the lamp on the desk." << endl
        << "Press w to toggle winter on and off." << endl
        << "Press r to toggle rain/snow on and off." << endl
        << "Press f to toggle fog on and off." << endl;
}

// Main routine.
int main(int argc, char** argv)
{
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("CollegeDorm3D.cpp");
    setup();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);
    glutMouseFunc(mouseControl);
    glutTimerFunc(16, updateTime, 0);
    glutIdleFunc(myIdle);
    glutMotionFunc(onMouseMove);
    //glutIdleFunc(myIdleW);
    glutMainLoop();

    return 0;
}
