#include <iostream>
#include <cstdlib>
#include <random>

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include "camera.h"
#include "model.h"
#include "skybox.h"
#include "ObjModelLoader.h"

using namespace std;

/* Application settings */
string title                  = "Reckless Racing Game (prototype)";

int windowWidth               = 800,
    windowHeight              = 600;

bool isFullScreenEnable       = true;                      // Enable/Disable Fullscreen

float fieldOfVision           = 60.0f,
      cameraDistance          =  5.0f;

float backgroundColor[3]      = { 0, 0, 0 };                // Color: Black

int timeForUpdatingFrame      = 25;

const int KEY_ESCAPE          = 27;

bool isWireframeModeEnabled   = false;                      // Default: false,  Enable/Disable wireframe

/* Lighting Settings */
// Set material properties for lighting
bool isLightingEnabled        = true,
     isSpecularLightEnabled   = false;

float shininess               = 50.0f;                      // Shininess: 0 to 128;

GLfloat colorWhite[]          = { 1.00, 1.00, 1.00, 1.0 };  // Color: White
GLfloat colorDarkGray[]       = { 0.70, 0.70, 0.70, 1.0 };  // Color: Dark gray
GLfloat colorLightGray[]      = { 0.95, 0.95, 0.95, 1.0 };  // Color: Light gray

GLfloat lightPosition[]	      = { 0.5, 1.0, 1.0, 0.0 };

/* Camera Settings */
const float cameraRotationSpeed     = M_PI / 180 * 0.2;
//    float cameraTranslationSpeed  = 0.10f;
const float initialCameraHeight     = 2.0f;
bool keypress[256];

Camera *camera = new Camera(0, initialCameraHeight, 0);

/* Model Settings */
//const string modelPath      = "C:\\Users\\Zunayed Hassan\\Desktop\\reckless_racing_game\\bin\\Debug\\models\\";
const string modelPath      = "models\\";

// Skybox
const string skyboxPathName = modelPath + "skybox";
Skybox *skybox              = nullptr;

// Grass
const string grassPathName  = modelPath + "grass\\grass";
Model *grassModel           = nullptr;

// Cars
const string carFileName1   = modelPath + "car1\\Lincoln Navigator.obj";
ObjModelLoader *carModel1   = NULL;

// Asphalt
const int roadBlockSize     = 21;
const string roadTextureFileName = modelPath + "asphalt\\asphalt";
Model *roadModel            = nullptr;

// Stone House
const int houseDistance     = 15;

// Stone Walls
const string stoneWallPathName = modelPath + "stone_wall\\";

Model *stoneWallFrontModel  = nullptr;
Model *stoneWall2FrontModel = nullptr;
Model *stoneWallRearModel   = nullptr;
Model *stoneWall2RearModel  = nullptr;

Model *stoneWallExtra       = nullptr;
Model *stoneWall2Extra      = nullptr;

// Roof Tiles
const string roofTilesPathName = modelPath + "roof_tiles\\";
Model *roofTilesModel       = nullptr;
Model *roofTiles2Model      = nullptr;

// Door
const string doorPathName = modelPath + "door\\";
Model *doorModel            = nullptr;
Model *door2Model           = nullptr;

// Window
const string windowPathName = modelPath + "window\\";
Model *windowModel          = nullptr;

// Box
const float boxSize              = 3.0f;
const int totalNumberOfObstacles = 9;
int boxesPosition[totalNumberOfObstacles] = { };
const string boxPathName         = modelPath + "box\\box";
Model *boxModel                  = nullptr;

/* Game Control Settings */
const int startPosition     = -100;
const int endPosition       =  100;

// Car Position
float carPosX               = 0.0f,
      carPosY               = 0.0f,
      carPosZ               = 0.0f;

// Heads Up Display (HUD) related
const int font = (int) GLUT_BITMAP_HELVETICA_18;

const string hudTextScore   = "Score: ";
const string hudTextSpeed   = "Speed: ";

float scoreBoardColor[]     = { 0, 0, 1 };

float score                 = totalNumberOfObstacles;

float playerPositionX       = 0.0f,
      playerPositionY       = 0.0f,
      playerPositionZ       = 0.0f;

float playerPositionOffset  = 27.5f;

// Car speed related
float cameraTranslationSpeed          = 0.30f;
const float playerSpeedAcceleration   = 0.07f;
//const float playerSpeedDeacceleration = 0.00f;
const float maxSpeed                  = 1.0f;
const float minSpeed                  = 0.30f;

long firstTime              = 0l,
     lastTime               = 0l;

CommonTools *commonTools    = new CommonTools;

bool IsMoreThanOneSecondsGone(float second = 1.0f)
{
    if ((((firstTime - lastTime) / (second * 1000.0f)) >= 1) && (second >= 1.0f))
    {
        lastTime = firstTime;
        return true;
    }

    return false;
}

void SetScoreBoardColor(float *colorRGB)
{
    for (int i = 0; i < 3; i++)
    {
        scoreBoardColor[i] = colorRGB[i];
    }
}

void DrawGrid()
{
    glPushMatrix();
    glColor3f(1, 1, 1);

    for (int i= -50; i < 50; i++)
    {
        glBegin(GL_LINES);
        glVertex3f(i, 0, -50);
        glVertex3f(i, 0, 50);
        glEnd();
    }

    for (int i = -50; i < 50; i++)
    {
        glBegin(GL_LINES);
        glVertex3f(-50, 0, i);
        glVertex3f(50, 0, i);
        glEnd();
    }

    glPopMatrix();
}

void DrawHouse()
{
    stoneWallFrontModel->Draw();
    stoneWall2FrontModel->Draw();
    stoneWallRearModel->Draw();
    stoneWall2RearModel->Draw();

    stoneWallExtra->Draw();
    stoneWall2Extra->Draw();

    roofTilesModel->Draw(60, true, false, false, 0.0f, 9.0f, 0.0f);
    roofTiles2Model->Draw(-60, true, false, false, 0.0f, 9.0f, 11.0f);

    doorModel->Draw();
    doorModel->Draw(0, false, false, false, 0, 0, 0.2);
    door2Model->Draw();
    door2Model->Draw(0, false, false, false, 0, 0, -0.2);

    windowModel->Draw(0, false, false, false, -0.1, 0, 0);
    windowModel->Draw(0, false, false, false, 0.1, 0, 0);
    windowModel->Draw(0, false, false, false, 16.1, 0, 0);
    windowModel->Draw(0, false, false, false, 15.9, 0, 0);
}

void DrawHouses()
{
    for (int i = startPosition; i < endPosition; i += 20 + houseDistance)
    {
        glTranslatef(-roadBlockSize * 1.2, 0, (-i + 20));
        glRotatef(90, false, true, false);
        DrawHouse();
        glRotatef(-90, false, true, false);
        glTranslatef(roadBlockSize * 1.2, 0, -(-i + 20));

        glTranslatef(roadBlockSize * 1.2 - (roadBlockSize / 2), 0, (-i + 20));
        glRotatef(90, false, true, false);
        DrawHouse();
        glRotatef(-90, false, true, false);
        glTranslatef(-(roadBlockSize * 1.2 - (roadBlockSize / 2)), 0, -(-i + 20));
    }
}

void DrawRoad()
{
    glTranslatef(-(roadBlockSize / 2), 0, 0);

    for (int i = 0; i < (200 / roadBlockSize) + 1; i++)
    {
        roadModel->Draw();
        glTranslatef(0, 0, roadBlockSize);
    }
}

void DrawBoxes(int posXIndex = 0, int posZ = 0)
{
    int currentPosX = 0;

    if (boxesPosition[posXIndex] == 1)
    {
        currentPosX = -(roadBlockSize / 4);
    }
    else if (boxesPosition[posXIndex] == 2)
    {
        currentPosX = (roadBlockSize / 4);
    }
    else
    {
        currentPosX = 1;
    }

    glTranslatef(currentPosX, 0, 0);

    boxModel->Draw(0, false, false, false, 0, -2, posZ);
    boxModel->Draw(0, false, false, false, -boxSize, -2, posZ);
    boxModel->Draw(0, false, false, false, -boxSize + (boxSize / 2), -2 + boxSize, posZ);

    glTranslatef(-currentPosX, 0, 0);
}

void SetOrthographicProjection()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
       gluOrtho2D(0, windowWidth, 0, windowHeight);
    glScalef(1, -1, 1);
    glTranslatef(0, -windowHeight, 0);
    glMatrixMode(GL_MODELVIEW);
}

void ResetPerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void RenderBitmapString(float x, float y, void *font, const char *string)
{
    const char *c;
    glRasterPos2f(x, y);

    for (c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}

void DrawRectangle(int startPosX = 0, int startPosY = 0, int width = 1, int height = 1)
{
    glBegin(GL_QUADS);

    glVertex2f(startPosX, startPosY);
    glVertex2f(startPosX + width, startPosY);
    glVertex2f(startPosX + width, startPosY - height);
    glVertex2f(startPosX, startPosY - height);

    glEnd();
}

void DrawScene()
{
    firstTime = glutGet(GLUT_ELAPSED_TIME);

    ResetPerspectiveProjection();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);                             // Switch to the drawing perspective
    glLoadIdentity();                                       // Reset the drawing perspective

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    if (isWireframeModeEnabled)
    {
        // Turn on wireframe mode
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
    }
    else
    {
        // Turn off wireframe mode
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }

    camera->Refresh();

    glTranslatef(cameraDistance, 0.0f, -cameraDistance * 5);              // Move forward 5 units

    // Adding light
    if (isLightingEnabled)
    {
        // Set lighting intensity and color
        glLightfv(GL_LIGHT0, GL_AMBIENT,  colorDarkGray );
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  colorLightGray);

        if (isSpecularLightEnabled)
        {
            glLightfv(GL_LIGHT0, GL_SPECULAR, colorWhite);

            // Setting material properties
            glMaterialfv(GL_FRONT, GL_SPECULAR, colorWhite);
            glMaterialf(GL_FRONT, GL_SHININESS, shininess); // Shininess: 0 to 128
        }

        // Set the light position
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    }

    // Camera and Car position will be almost same
    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, false);

    glColor3f(0.3, 0.3, 0.3);

    camera->GetPosition(carPosX, carPosY, carPosZ);

    glTranslatef(carPosX - (roadBlockSize / 4), 0, carPosZ + 23);
    carModel1->Draw();

    if (!((carPosZ < startPosition - 15) || (carPosZ >= endPosition - 40)))
    {
        camera->Move(cameraTranslationSpeed);
    }

    glPopMatrix();

    for (int z = startPosition + ((-startPosition + endPosition - 20) / 2) / totalNumberOfObstacles + 20,
             boxIndex = 0;

             z < endPosition - ((-startPosition + endPosition - 20) / 2) / (totalNumberOfObstacles + 1),
             boxIndex < totalNumberOfObstacles;

             z += ((-startPosition + endPosition - 20) / 1) / totalNumberOfObstacles,
             boxIndex++)
    {
        DrawBoxes(boxIndex, z);
    }

    skybox->Draw();

    glTranslatef(0.0f, -1.8f, 0.0f);
    grassModel->Draw();

    DrawHouses();
    DrawRoad();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetOrthographicProjection();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();

    camera->GetPosition(playerPositionX, playerPositionY, playerPositionZ);

    // Detect collusion
    for (int z = startPosition + ((-startPosition + endPosition - 20) / 2) / totalNumberOfObstacles + 20,
             boxIndex = 0;

             z < endPosition - ((-startPosition + endPosition - 20) / 2) / (totalNumberOfObstacles + 1),
             boxIndex < totalNumberOfObstacles;

             z += ((-startPosition + endPosition - 20) / 1) / totalNumberOfObstacles,
             boxIndex++)
    {
        if (((playerPositionZ + playerPositionOffset) >= z) && ((playerPositionZ + playerPositionOffset) <= z + boxSize))
        {
            int boxPositionX = 0;

            if (boxesPosition[boxIndex] == 1)
            {
                boxPositionX = -(roadBlockSize / 4);
            }
            else if (boxesPosition[boxIndex] == 2)
            {
                boxPositionX = (roadBlockSize / 4);
            }
            else
            {
                boxPositionX = 1;
            }

            static float carWidth = (roadBlockSize / 4.0f);

            if ((playerPositionX >= boxPositionX) && (playerPositionX - carWidth <= boxPositionX + boxSize * 2))
            {
                float color[] = { 1, 0, 0 };

                SetScoreBoardColor(color);
                --score;
            }
            else
            {
                float color[] = { 0.33f, 0.48f, 0.18f };

                SetScoreBoardColor(color);

                ++score;
            }
        }
    }

    SetScoreBoardColor(scoreBoardColor);
    glColor3f(scoreBoardColor[0], scoreBoardColor[1], scoreBoardColor[2]);
    DrawRectangle(15, 40, 150, 30);
    glColor3d(1, 1, 1);
    RenderBitmapString(25, 30, (void *) font, (hudTextScore + commonTools->GetStringFromNumber(score)).data());

    glColor3d(1, 0, 1);
    DrawRectangle(windowWidth - 168, 40, 150, 30);
    glColor3d(1, 1, 1);
    RenderBitmapString(windowWidth - 128 - 10, 30, (void *) font, (hudTextSpeed+ commonTools->GetStringFromNumber(cameraTranslationSpeed) + " ps").data());
    glPopMatrix();

    glFlush();

    glutSwapBuffers();
}

void Update(int value)
{
    glutPostRedisplay();                                    // Tell GLUT that the display has changed

    if ((keypress['a'] || keypress['A']) && !(carPosX > (roadBlockSize / 2) + 2))
    {
        camera->Strafe(cameraTranslationSpeed);
    }
    else if (((keypress['d'] || keypress['D'])) && !(carPosX < -2))
    {
        camera->Strafe(-cameraTranslationSpeed);
    }

    camera->m_y = initialCameraHeight;

    // Tell GLUT to call update again in 25 milliseconds
    glutTimerFunc(timeForUpdatingFrame, Update, 0);
}

// Called when a key is pressed
void HandleKeypress(unsigned char key, int x, int y) {
    switch (key)
    {
        // Escape key
        case KEY_ESCAPE:
            exit(EXIT_SUCCESS);
            break;

        // Toggle Wireframe
        case 'w':
            if (IsMoreThanOneSecondsGone())
            {
                cameraTranslationSpeed += playerSpeedAcceleration;
            }

            if (cameraTranslationSpeed > maxSpeed)
            {
                cameraTranslationSpeed = maxSpeed;
            }

            break;

        default:
            keypress[key] = true;
            break;
    }
}

void KeyboardUp(unsigned char key, int x, int y)
{
//    if (IsMoreThanOneSecondsGone())
//    {
//        cameraTranslationSpeed -= playerSpeedDeacceleration;
//    }
//
//    if (cameraRotationSpeed < minSpeed)
//    {
//        cameraTranslationSpeed = minSpeed;
//    }

    keypress[key] = false;
}

void MouseMotion(int x, int y)
{
    // This variable is hack to stop glutWarpPointer from triggering an event callback to Mouse(...)
    // This avoids it being called recursively and hanging up the event loop
    static bool just_warped = false;

    if (just_warped) {
        just_warped = false;
        return;
    }

    int dx = x - windowWidth / 2;
    int dy = y - windowHeight / 2;

    if (dx)
    {
        camera->RotateYaw(cameraRotationSpeed * dx);
    }

    if (dy)
    {
        camera->RotatePitch(cameraRotationSpeed * dy);
    }

    glutWarpPointer(windowWidth / 2, windowHeight / 2);

    just_warped = true;
}

// Called when the window is resized
void HandleResize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfVision, (double) width / (double) height, 1.0, 200.0);
}

int main(int argc, char* argv[])
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);

    //Create the window
    glutCreateWindow(title.c_str());

    // Initialize
    glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
    glClearDepth(1.0f);                                 // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);                            // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);                             // Set the type of depth-test
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);                            // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
    glEnable(GL_TEXTURE_2D);

    if (isFullScreenEnable)
    {
        windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        glutFullScreen();
    }

    // Lighting set up
    if (isLightingEnabled)
    {
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }

    // Set handler functions
    glutDisplayFunc(DrawScene);
    glutReshapeFunc(HandleResize);

    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(windowWidth / 2, windowHeight / 2);

    glutKeyboardFunc(HandleKeypress);
    //glutMotionFunc(MouseMotion);
    //glutPassiveMotionFunc(MouseMotion);
    glutKeyboardUpFunc(KeyboardUp);

    // Camera
    camera->RotateYaw(1.57f);
    camera->RotatePitch(0.25f);
    camera->SetPosition((roadBlockSize / 4), 0, -112);      // (roadBlockSize / 4) is almost same as (Road Width / 2) - (Car Width / 2)

    skybox = new Skybox(skyboxPathName);
    grassModel = new Model(grassPathName, "Grass", true, 60);
    carModel1 = new ObjModelLoader(carFileName1);
    roadModel = new Model(roadTextureFileName, "Grass", true, 1);

    // Stone House
    stoneWallFrontModel = new Model(stoneWallPathName + "stone_wall.tga",  stoneWallPathName + "stone_wall_front.txt",   "Stone Wall Front",   true, 4);
    stoneWall2FrontModel = new Model(stoneWallPathName + "stone_wall.tga", stoneWallPathName + "stone_wall_front_2.txt", "Stone Wall Front 2", true, 4);
    stoneWallRearModel = new Model(stoneWallPathName + "stone_wall.tga",   stoneWallPathName + "stone_wall_rear.txt",    "Stone Wall Rear",    true, 3);
    stoneWall2RearModel = new Model(stoneWallPathName + "stone_wall.tga",  stoneWallPathName + "stone_wall_rear_2.txt",   "Stone Wall Rear 2", true, 3);

    stoneWallExtra = new Model(stoneWallPathName + "stone_wall.tga",  stoneWallPathName + "stone_wall_extra.txt",   "Stone Wall Extra", true, 2);
    stoneWall2Extra = new Model(stoneWallPathName + "stone_wall.tga",  stoneWallPathName + "stone_wall_extra_2.txt",   "Stone Wall Extra 2", true, 2);

    roofTilesModel = new Model(roofTilesPathName + "roof_tiles.tga",  roofTilesPathName + "roof_tiles.txt", "Roof Tiles", true, 2);
    roofTiles2Model = new Model(roofTilesPathName + "roof_tiles.tga",  roofTilesPathName + "roof_tiles_2.txt", "Roof Tiles 2", true, 2);

    doorModel = new Model(doorPathName + "door.tga",  doorPathName + "door.txt", "Door", true, 1);
    door2Model = new Model(doorPathName + "door.tga",  doorPathName + "door_2.txt", "Door 2", true, 1);

    windowModel = new Model(windowPathName + "window.tga",  windowPathName + "window.txt", "Window", true, 1);

    boxModel = new Model(boxPathName, "Box", true, 1);

    for (int i = 0; i < totalNumberOfObstacles; i++)
    {
        int posX = rand() % 3;
        boxesPosition[i] = posX;
    }

    glutTimerFunc(timeForUpdatingFrame, Update, 0);     // Add a timer

    glutMainLoop();
    return EXIT_SUCCESS;
}
