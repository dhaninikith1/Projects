// Include standard headers
# define GL_SILENCE_DEPRECATION
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

# define PI         3.141592653589793238462643383279502884L /* pi */
using namespace std;
GLfloat* listOfVertices;
GLint* listOfFaces;
vector<GLfloat> fileVertices;
vector<GLint> fileFaces;

int heightOfWindow = 800;
int widthOfWindow = 800;

string presentModel;
string cactus = "cactus";
string bunny = "bunny";
string tea = "tea";


bool enabledCulling = false;
bool frontFace = false;
bool cw = false;
bool ccw = false;

int drawMode = GL_TRIANGLES;

int presentState = 0;
double zoomFactor = 1.0;
float nearClipping = 0.1;
float farClipping = 20;

float cameraAxis[3][3] = { { 1,0,0 },{ 0,1,0 },{ 0,0,1 } };
float cameraPosition[3] = { 0,0,5 };
float cameraLookAtPoint[3] = { 0,0,0 };

float rotationMatrix[3][3] = { { 0,0,0 },{ 0,0,0 },{ 0,0,0 } };

float translateDistance = 0.2f;
float rotationAngleStep = 0.1f;
float viewdist = 5;

GLint noOfVertices=0;
GLint noOfFaces=0;

int win,win1, win2;

// for GLSL
//GLuint vsID, fsID, pID, VBO, VAO, EBO;
// for uniform input
//GLfloat projectionMat[16], modelViewMat[16];

GLfloat color[3] = { 1.0f,1.0f,1.0f };

//void calculateGluLookatMatrix();
//void glhPerspectivef2();

void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}
 

void extractFacesAndVerticesFromFile(int type) {

    ifstream myfile;
    string tempPresentModel;
    if (type == 1) {
    tempPresentModel = cactus;
        cout << " loading the cactus" << endl;
    }
    else if (type == 2) {
        tempPresentModel = bunny;
        cout << " loading the bunny" << endl;
    }
    
    /*GLfloat tempListOfVertices[300000] = { 0 };
    GLfloat tempListOfFaces[300000] = { 0 };*/
    vector<GLfloat> tempFileVertices;
    vector<GLint> tempFileFaces;
    myfile.open(tempPresentModel + ".obj", ios::in);
    bool abcd = myfile.is_open();

    int vertexCount = 0;
    int faceCount = 0;
    if (myfile.is_open() == true)
    {
        string lineAbc;
        while (getline(myfile, lineAbc)) {
            vector<string> x = split(lineAbc, ' ');

            if (x[0] == "v") {
                tempFileVertices.push_back(std::stof(x[1]));
                tempFileVertices.push_back(std::stof(x[2]));
                tempFileVertices.push_back(std::stof(x[3]));
                tempFileVertices.push_back(color[0]);
                tempFileVertices.push_back(color[1]);
                tempFileVertices.push_back(color[2]);
                vertexCount += 3;
            }
            else if (x[0] == "f") {
                tempFileFaces.push_back((std::stoi(x[1]))-1);
                tempFileFaces.push_back((std::stoi(x[2])) - 1);
                tempFileFaces.push_back((std::stoi(x[3])) - 1);
                faceCount += 3;
            }
        }
    }
    presentModel = tempPresentModel;
    fileFaces = tempFileFaces;
    fileVertices = tempFileVertices;
}

//code for fixed pipeline
void drawVextex(int i) {

    int vertexPosition = (i) * 6;
    GLfloat tempVertex[][3] = { {
        fileVertices[vertexPosition++],
        fileVertices[vertexPosition++],
        fileVertices[vertexPosition]} };
    glVertex3fv(tempVertex[0]);

}


void drawObject() {
    //int sizeOfFaces = fileFaces.size();
    for (int i = 0; i < fileFaces.size(); i++) {

            glBegin(drawMode);
            drawVextex(fileFaces[i++]);
            drawVextex(fileFaces[i++]);
            drawVextex(fileFaces[i]);
            glEnd();
        }
    }


void changeSize(int w, int h) {

    widthOfWindow = w;
    heightOfWindow = h;
    // Set the viewport to be the entire window
    glViewport(0, 0, widthOfWindow, heightOfWindow);

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);

    // Reset Matrix
    glLoadIdentity();

    gluPerspective(60.0f, (float)widthOfWindow / (float)heightOfWindow, nearClipping, farClipping);

    glMatrixMode(GL_MODELVIEW);
    
}


void renderScene(void) {

    glutSetWindow(win1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(60.0f, (float)widthOfWindow / (float)heightOfWindow, nearClipping, farClipping);
    GLfloat tempProjectionMat[16], tempModelViewMat[16];

    glGetFloatv(GL_PROJECTION_MATRIX, tempProjectionMat);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
    cameraLookAtPoint[0], cameraLookAtPoint[1], cameraLookAtPoint[2],
    cameraAxis[1][0], cameraAxis[1][1], cameraAxis[1][2]);
    glGetFloatv(GL_MODELVIEW_MATRIX, tempModelViewMat);
    drawObject();
    glutSwapBuffers();
}

void changeColor(float r, float g, float b) {

    try
    {
        for (int i = 2; i < fileVertices.size(); i = i + 3) {

            fileVertices[++i] = r;
            fileVertices[++i] = g;
            fileVertices[++i] = b;
        }
    }
    catch (int e)
    {
        cout << "An exception occurred. Exception Nr. " << e << '\n';
    }
    
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void colorTheObject(int type) {

    switch (type)
    {
    case 1:
        glColor3f(1.0f, 1.0f, 1.0f);
        changeColor(1.0f, 1.0f, 1.0f);
        break;

    case 2:
        glColor3f(1.0f, 0.0f, 0.0f);
        changeColor(1.0f, 0.0f, 0.0f);
        break;

    case 3:
        glColor3f(0.0f, 1.0f, 0.0f);
        changeColor(0.0f, 1.0f, 0.0f);
        break;

    case 4:
        glColor3f(0.0f, 0.0f, 1.0f);
        changeColor(0.0f, 0.0f, 1.0f);
        break;
    default:
        break;
    }

}

void cameraTranslationAlongCameraAxis(int axis, GLfloat dir)
{

    for (int i = 0; i<3; i++)
    {
        cameraPosition[i] += cameraAxis[axis][i] * dir * translateDistance;
        cameraLookAtPoint[i] += cameraAxis[axis][i] * dir * translateDistance;
    }
}

// www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix
void axisAngleToMatrix(GLfloat *axis, GLfloat angle)
{
    GLfloat c = cos(angle);
    GLfloat s = sin(angle);
    GLfloat t = 1 - c;

    // normalise axis vector
    GLfloat mag = sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
    axis[0] /= mag;
    axis[1] /= mag;
    axis[2] /= mag;

    GLfloat x = axis[0];
    GLfloat y = axis[1];
    GLfloat z = axis[2];

    rotationMatrix[0][0] = t * x * x + c;
    rotationMatrix[0][1] = t * x * y - z * s;
    rotationMatrix[0][2] = t * x * z + y * s;
    rotationMatrix[1][0] = t * x * y + z * s;
    rotationMatrix[1][1] = t * y * y + c;
    rotationMatrix[1][2] = t * y * z - x * s;
    rotationMatrix[2][0] = t * x * z - y * s;
    rotationMatrix[2][1] = t * y * z + x * s;
    rotationMatrix[2][2] = t * z * z + c;
}

void applyRotation()
{
    GLfloat temp[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };
    for (int i = 0; i<3; i++)
    {
        for (int j = 0; j<3; j++)
        {
            temp[i][j] = rotationMatrix[j][0] * cameraAxis[i][0] + rotationMatrix[j][1] * cameraAxis[i][1] + rotationMatrix[j][2] * cameraAxis[i][2];
        }
    }

    for (int i = 0; i<3; i++)
    {
        for (int j = 0; j<3; j++)
        {
            cameraAxis[i][j] = temp[i][j];
        }
    }

}

void rotateCamera(char axis, GLfloat dir)
{
    if (axis == 'x')
    {
        axisAngleToMatrix(cameraAxis[0], rotationAngleStep * dir);
    }
    else if (axis == 'y')
    {
        axisAngleToMatrix(cameraAxis[1], rotationAngleStep * dir);
    }
    else if (axis == 'z')
    {
        axisAngleToMatrix(cameraAxis[2], rotationAngleStep * dir);
    }
    applyRotation();

    float temp[3] = { 0,0,0 };
    temp[0] = rotationMatrix[0][0] * cameraLookAtPoint[0] + rotationMatrix[0][1] * cameraLookAtPoint[1] + rotationMatrix[0][2] * cameraLookAtPoint[2];
    temp[1] = rotationMatrix[1][0] * cameraLookAtPoint[0] + rotationMatrix[1][1] * cameraLookAtPoint[1] + rotationMatrix[1][2] * cameraLookAtPoint[2];
    temp[2] = rotationMatrix[2][0] * cameraLookAtPoint[0] + rotationMatrix[2][1] * cameraLookAtPoint[1] + rotationMatrix[2][2] * cameraLookAtPoint[2];

    cameraLookAtPoint[0] = cameraPosition[0] + cameraAxis[2][0] * viewdist*-1;
    cameraLookAtPoint[1] = cameraPosition[1] + cameraAxis[2][1] * viewdist*-1;
    cameraLookAtPoint[2] = cameraPosition[2] + cameraAxis[2][2] * viewdist*-1;
    cout << cameraLookAtPoint << endl;
}


static void Key(unsigned char key, int x, int y)
{
    switch (key) {
    case '1':
        presentState = 1;
        break;
    case '2':
        presentState = 2;
        break;
    case '3':
        presentState = 3;
        break;
    case '4':
        presentState = 4;
        break;
    case '5':
        presentState = 5;
        break;
    case '6':
        presentState = 6;
        break;
    case '7':
        presentState = 7;

        cameraAxis[0][0] = 1;
        cameraAxis[0][1] = 0;
        cameraAxis[0][2] = 0;

        cameraAxis[1][0] = 0;
        cameraAxis[1][1] = 1;
        cameraAxis[1][2] = 0;

        cameraAxis[2][0] = 0;
        cameraAxis[2][1] = 0;
        cameraAxis[2][2] = 1;

        cameraPosition[0] = 0;
        cameraPosition[1] = 0;
        cameraPosition[2] = 5;
        cameraLookAtPoint[0] = 0;
        cameraLookAtPoint[1] = 0;
        cameraLookAtPoint[2] = 0;

        rotationMatrix[3][3] = 0;

        nearClipping = 0.1;
        farClipping = 20;
        break;
    case '8':
        presentState = 8;
        break;
    case '9':
        presentState = 9;

        break;
    case 27:             /* Escape key. */
        exit(0);
    }
    //calculateGluLookatMatrix();
   // glhPerspectivef2();
    glutPostRedisplay();
}

static void Menu(int value) {
    /* Menu items have key values assigned to them.  Just pass
    this value to the key routine. */
    Key(value, 0, 0);
}

/* ARGSUSED1 */
static void SpecialKey(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        if (presentState == 1) {
            colorTheObject(1);
        }
        if (presentState == 2) {
            drawMode = GL_LINE_STRIP;
        }
        if (presentState == 3) {
            if (!(presentModel.compare(cactus) == 0)) {
                extractFacesAndVerticesFromFile(1);
            }
        }
        if (presentState == 5) {
            cameraTranslationAlongCameraAxis(0, +1);
            /*transInX += 1.f; */
        }
        if (presentState == 6) {
            rotateCamera('x', +1);
            //rotInX += 5;
        }
        if (presentState == 8) {
            nearClipping += 0.1;
            changeSize(widthOfWindow, heightOfWindow);
            cout << "nearClipping plane " << nearClipping << endl;
            }
        if (presentState == 9) {
            enabledCulling = true;
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        if (presentState == 1) {
            colorTheObject(2);
        }
        if (presentState == 2) {
            drawMode = GL_POINTS;
        }
        if (presentState == 3) {
            if (!(presentModel.compare(bunny) == 0)) {
                extractFacesAndVerticesFromFile(2);
            }
        }
        if (presentState == 5) {
            cameraTranslationAlongCameraAxis(0, -1);
            
        }
        if (presentState == 6) {
            rotateCamera('x', -1);
            
            //rotInX -= 5;
        }
        if (presentState == 8) {
            if (nearClipping > 0.2)
                nearClipping -= 0.1;
            cout << "nearClipping plane " << nearClipping << endl;
            changeSize(widthOfWindow, heightOfWindow);
            
        }
        if (presentState == 9) {

            enabledCulling = true;
            frontFace = true;
        }
        glutPostRedisplay();

        break;
    case GLUT_KEY_UP:
        if (presentState == 1) {
            colorTheObject(3);
        }
        if (presentState == 2) {
            drawMode = GL_TRIANGLES;
        }
        if (presentState == 3) {
            if (!(presentModel.compare(tea) == 0)) {
                extractFacesAndVerticesFromFile(3);
            }
        }
        if (presentState == 5) {
            cameraTranslationAlongCameraAxis(1, -1);
        }
        if (presentState == 6) {
            rotateCamera('y', -1);
            //rotInY += 5;
        }
        if (presentState == 8) {
            farClipping += 0.1;
            cout << "farClipping plane " << farClipping << endl;
            changeSize(widthOfWindow, heightOfWindow);
        }
        if (presentState == 9) {
            cw = true;
            ccw = false;
            glFrontFace(GL_CW);
        }
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        if (presentState == 1) {
            colorTheObject(4);
        }
        if (presentState == 2) {
            drawMode = GL_LINE_LOOP;
        }
        if (presentState == 3) {
            if (!(presentModel.compare(tea) == 0)) {
                extractFacesAndVerticesFromFile(4);
            }
        }
        if (presentState == 5) {
            
            cameraTranslationAlongCameraAxis(1, +1);
        }
        if (presentState == 6) {
            rotateCamera('y', +1);
        }
        if (presentState == 8) {
            farClipping -= 0.1;
            cout << "farClipping plane " << farClipping << endl;
            changeSize(widthOfWindow, heightOfWindow);
        }
        if (presentState == 9) {
            cw = false;
            ccw = true;
            glFrontFace(GL_CCW);
        }
        glutPostRedisplay();
        break;

    case GLUT_KEY_F1:
        if (presentState == 9) {
            cw = false;
            ccw = false;
            enabledCulling = false;
            frontFace = false;
        }
        if (presentState == 5) {
            /*transInY += 1.f;*/
            cameraTranslationAlongCameraAxis(2, -1);
        }
        if (presentState == 6) {
            rotateCamera('z', +1);
        }
        if (presentState == 9) {
            glDisable(GL_CULL_FACE);
            enabledCulling = false;
        }
        break;
    case GLUT_KEY_F2:
        if (presentState == 5) {
            /*transInY += 1.f;*/
            cameraTranslationAlongCameraAxis(2, +1);
        }
        if (presentState == 6) {
            rotateCamera('z', -1);
        }
        break;
    }
    //calculateGluLookatMatrix();
    //glhPerspectivef2();
    glutPostRedisplay();

    
}

/*void calculateGluLookatMatrix() {
    float matrix1[16];

    matrix1[0] = cameraAxis[0][0];
    matrix1[4] = cameraAxis[0][1];
    matrix1[8] = cameraAxis[0][2];

    //------------------
    matrix1[1] = cameraAxis[1][0];
    matrix1[5] = cameraAxis[1][1];
    matrix1[9] = cameraAxis[1][2];

    //------------------
    matrix1[2] = cameraAxis[2][0];
    matrix1[6] = cameraAxis[2][1];
    matrix1[10] = cameraAxis[2][2];

    //------------------
    matrix1[12] = -(cameraAxis[0][0] * cameraPosition[0] + cameraAxis[0][1] * cameraPosition[1] + cameraAxis[0][2] * cameraPosition[2]);
    matrix1[13] = -(cameraAxis[1][0] * cameraPosition[0] + cameraAxis[1][1] * cameraPosition[1] + cameraAxis[1][2] * cameraPosition[2]);
    matrix1[14] = -(cameraAxis[2][0] * cameraPosition[0] + cameraAxis[2][1] * cameraPosition[1] + cameraAxis[2][2] * cameraPosition[2]);
    //------------------

    matrix1[3] = matrix1[7] = matrix1[11] = 0.0;
    matrix1[15] = 1.0;
    memcpy(modelViewMat, matrix1, 16 * sizeof(float));
}
*/

/*void glhFrustumf2(float *matrix, float left, float right, float bottom, float top,
    float znear, float zfar)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0 * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix[0] = temp / temp2;
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = temp / temp3;
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = (right + left) / temp2;
    matrix[9] = (top + bottom) / temp3;
    matrix[10] = (-zfar - znear) / temp4;
    matrix[11] = -1.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = (-temp * zfar) / temp4;
    matrix[15] = 0.0;
}
 */

/*void glhPerspectivef2()
{
    float fovyInDegrees = 60.0f;
    float aspectRatio = (float)widthOfWindow / (float)heightOfWindow;
    float znear = nearClipping;
    float zfar = farClipping;
    float ymax, xmax;
    //float temp, temp2, temp3, temp4;
    ymax = znear * tanf(fovyInDegrees * PI / 360.0);
    xmax = ymax * aspectRatio;
    glhFrustumf2(projectionMat, -xmax, xmax, -ymax, ymax, znear, zfar);
}
 */
/*
void initBufferObject(void)
{

    GLfloat vertData[8 * 6];
    // position information
    vertData[0 * 6 + 0] = vertData[1 * 6 + 0] = vertData[2 * 6 + 0] = vertData[3 * 6 + 0] = -1.0f;
    vertData[4 * 6 + 0] = vertData[5 * 6 + 0] = vertData[6 * 6 + 0] = vertData[7 * 6 + 0] = 1.0f;
    vertData[0 * 6 + 1] = vertData[1 * 6 + 1] = vertData[4 * 6 + 1] = vertData[5 * 6 + 1] = -1.0f;
    vertData[2 * 6 + 1] = vertData[3 * 6 + 1] = vertData[6 * 6 + 1] = vertData[7 * 6 + 1] = 1.0f;
    vertData[0 * 6 + 2] = vertData[3 * 6 + 2] = vertData[4 * 6 + 2] = vertData[7 * 6 + 2] = 1.0f;
    vertData[1 * 6 + 2] = vertData[2 * 6 + 2] = vertData[5 * 6 + 2] = vertData[6 * 6 + 2] = -1.0f;

    // color
    vertData[0 * 6 + 3] = vertData[1 * 6 + 3] = vertData[2 * 6 + 3] = vertData[3 * 6 + 3]
        = vertData[4 * 6 + 3] = vertData[5 * 6 + 3] = vertData[6 * 6 + 3] = vertData[7 * 6 + 3] = 1.0f;

    vertData[0 * 6 + 4] = vertData[1 * 6 + 4] = vertData[2 * 6 + 4] = vertData[3 * 6 + 4]
        = vertData[4 * 6 + 4] = vertData[5 * 6 + 4] = vertData[6 * 6 + 4] = vertData[7 * 6 + 4] = 0.0f;

    vertData[0 * 6 + 5] = vertData[1 * 6 + 5] = vertData[2 * 6 + 5] = vertData[3 * 6 + 5]
        = vertData[4 * 6 + 5] = vertData[5 * 6 + 5] = vertData[6 * 6 + 5] = vertData[7 * 6 + 5] = 0.0f;

    GLuint indices[] =
    { 0, 1, 2, 3,
        3, 2, 6, 7,
        7, 6, 5, 4,
        4, 5, 1, 0,
        5, 6, 2, 1,
        7, 4, 0, 3 };

    std::vector<GLfloat> tempfileVertices(vertData, vertData + sizeof vertData / sizeof vertData[0]);
    std::vector<GLint> tempfileFaces(indices, indices + sizeof indices / sizeof indices[0]);
    // position information
    //int size = sizeof(vertData);
    //int size1 = sizeof(fileVertices);
    int size2 = fileVertices.size();
    int size3 = fileVertices.size() * sizeof(GLfloat);
    
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // setup VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, fileVertices.size() *sizeof(GLfloat), &fileVertices[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fileFaces.size() * sizeof(GLint), &fileFaces[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
     */

    // Use depth buffering for hidden surface elimination
    //glEnable(GL_DEPTH_TEST);
    //glhPerspectivef2();
   // calculateGluLookatMatrix();
//}

void createWindowForGraphicsPipeline() {
    win1 = glutCreateWindow("Assignment 1");
    glutCreateMenu(Menu);
    glutAddMenuEntry("Change Color", '1');
    glutAddMenuEntry("Rendering Types", '2');
    glutAddMenuEntry("Load different Model", '3');
    glutAddMenuEntry("translate", '5');
    glutAddMenuEntry("Rotation", '6');
    glutAddMenuEntry("Reset position", '7');
    glutAddMenuEntry("Change Clipping", '8');
    glutAddMenuEntry("Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    // register callbacks
    glutDisplayFunc(renderScene);
    glutSpecialFunc(SpecialKey);
}


int main(int argc, char **argv)
{
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(widthOfWindow, heightOfWindow);
    glutInitWindowPosition(0, 0);
    glutInitContextFlags(GLUT_COMPATIBILITY_PROFILE);
    extractFacesAndVerticesFromFile(1);
    createWindowForGraphicsPipeline();
    glutMainLoop();
    return 0;
}

