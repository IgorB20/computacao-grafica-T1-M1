#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/freeglut.h>
#include <string>

using namespace std;

//globals

bool isFullScreenActive = false;

float xRotation;
float yRotation;
float zRotation;
int rotationUnit = 4;

float xTranslation = 0;
float yTranslation = 0;
float zTranslation = -25;
float translationUnit = 4;

float scale = 0.05f;
float scaleUnit = 0.05f;

bool isMouseRotationActive = false;
bool isMouseTranslationActive = false;
float mouseX, mouseY;


unsigned int exibitionList;
vector<vector<float>> vertices;
vector<vector<int>> faces;
vector<vector<float>> textures;
vector<vector<float>> normals;

void keyboard(unsigned char key, int x, int y);

void buildObj(){
    exibitionList = glGenLists(1);
    glNewList(exibitionList, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);

        for (size_t i = 0; i < faces.size(); ++i) {
            const auto& face = faces[i];
            
            if (face[2] != -1 && face[2] < (int)normals.size()) glNormal3fv(normals[face[2]].data());
            glVertex3fv(vertices[face[0]].data());
            
            if (face[5] != -1 && face[5] < (int)normals.size()) glNormal3fv(normals[face[5]].data());
            glVertex3fv(vertices[face[3]].data());
            
            if (face[8] != -1 && face[8] < (int)normals.size()) glNormal3fv(normals[face[8]].data());
            glVertex3fv(vertices[face[6]].data());
    
        }

        glEnd();
        glPopMatrix();
    }
    glEndList();
}

void loadObj(string fname)
{
    ifstream arquivo(fname);
    if (!arquivo.is_open()) {
        cout << "arquivo nao encontrado";
        exit(1);
    }
    else {
        string tipo;
        while (arquivo >> tipo)
        {

            if (tipo == "v")
            {
                vector<float> vertice;
                float x, y, z;
                arquivo >> x >> y >> z;
                vertice.push_back(x);
                vertice.push_back(y);
                vertice.push_back(z);
                vertices.push_back(vertice);
            }
            
            if(tipo == "vt")
            {
                vector<float> texture;
                float u, v;
                arquivo >> u >> v;
                texture.push_back(u);
                texture.push_back(v);
                textures.push_back(texture);
            }
            
            if(tipo == "vn")
            {
                vector<float> normal;
                float x, y, z;
                arquivo >> x >> y >> z;
                normal.push_back(x);
                normal.push_back(y);
                normal.push_back(z);
                normals.push_back(normal);
            }

            if (tipo == "f")
            {
                vector<string> facesString;
                string f1, f2, f3;
                arquivo >> f1 >> f2 >> f3;;

                facesString.push_back(f1);
                facesString.push_back(f2);
                facesString.push_back(f3);
                
                vector<int> indicesFace;
                
                for(int i = 0; i< facesString.size(); i++){
                    string segmento = facesString[i];
                    size_t barra1 = segmento.find('/');
                    size_t barra2 = string::npos;
                    if (barra1 != string::npos) {
                        barra2 = segmento.find('/', barra1 + 1);
                    }
                    
                    int v_idx = stoi(segmento.substr(0, barra1)) - 1;
                    int vt_idx = -1;
                    int vn_idx = -1;
                    
                    if (barra1 != string::npos) {
                        if (barra2 != string::npos) {
                            if (barra1 + 1 < barra2) {
                                vt_idx = stoi(segmento.substr(barra1 + 1, barra2 - (barra1 + 1))) - 1;
                            }
                            vn_idx = stoi(segmento.substr(barra2 + 1)) - 1;
                        } else {
                            if (segmento.at(barra1 + 1) == '/') {
                                vn_idx = stoi(segmento.substr(barra1 + 2)) - 1;
                            } else {
                                vt_idx = stoi(segmento.substr(barra1 + 1)) - 1;
                            }
                        }
                    }
                    
                    indicesFace.push_back(v_idx);
                    indicesFace.push_back(vt_idx);
                    indicesFace.push_back(vn_idx);

                }
                faces.push_back(indicesFace);
            }
        }
    }
}


void render()
{
    glPushMatrix();
    //glTranslatef(0, 0, -105);
    glColor3f(1.0, 0.23, 0.27);
    
    glTranslatef(xTranslation, yTranslation, zTranslation);
    
    glScalef(scale, scale, scale);
    
    glRotatef(xRotation, 1, 0, 0);
    glRotatef(yRotation, 0, 1, 0);
    glRotatef(zRotation, 0, 0, 1);
    
    glCallList(exibitionList);
    glPopMatrix();
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

void handleMouseWheel(int botao, int direcao, int x, int y) {
    if (direcao > 0) {
        scale += 0.05f;
    } else {
        if (scale > 0.05f) scale -= 0.05f;
    }
    glutPostRedisplay();
}

void handleMouseMovement(int x, int y) {
    if (isMouseRotationActive) {
        yRotation += (x - mouseX) * 0.5f;
        xRotation += (y - mouseY) * 0.5f;
        mouseX = x;
        mouseY = y;
    } else if (isMouseTranslationActive) {
        xTranslation += (x - mouseX) * 0.1f;
        yTranslation -= (y - mouseY) * 0.1f;
        mouseX = x;
        mouseY = y;
    }
    glutPostRedisplay();
}

void handleMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMouseRotationActive = true;
            mouseX = x;
            mouseY = y;
        } else {
            isMouseRotationActive = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMouseTranslationActive = true;
            mouseX = x;
            mouseY = y;
        } else {
            isMouseTranslationActive = false;
        }
    }
}

void rotatePositiveY(){
    yRotation = yRotation + rotationUnit;
    if (yRotation > 360) yRotation = yRotation - 360;
}

void rotateNegativeY(){
    yRotation = yRotation - rotationUnit;
    if (yRotation < 360) yRotation = yRotation + 360;
}

void rotatePositiveX(){
    xRotation = xRotation + rotationUnit;
    if (xRotation > 360) xRotation = xRotation - 360;
}

void rotateNegativeX(){
    xRotation = xRotation - rotationUnit;
    if (xRotation < 360) xRotation = xRotation + 360;
}

void rotatePositiveZ(){
    zRotation = zRotation + rotationUnit;
    if (zRotation > 360) zRotation = zRotation - 360;
}

void rotateNegativeZ(){
    zRotation = zRotation - rotationUnit;
    if (zRotation < 360) zRotation = zRotation + 360;
}

void scalePositive(){
    scale = scale + scaleUnit;
};

void scaleNegative(){
    if ((scale - scaleUnit) > scaleUnit) scale -= scaleUnit;
}




void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;
        
        case 'd':
            xTranslation += translationUnit;
            break;
            
        case 'a':
            xTranslation -= translationUnit;
            break;
                
        case 's':
            yTranslation -= translationUnit;
            break;
            
        case 'w':
            yTranslation += translationUnit;
            break;
            
        case 'q':
            zTranslation -= translationUnit;
            break;
                
        case 'e':
            zTranslation += translationUnit;
            break;
            
        case '+':
            scalePositive();
            break;
                
        case '-':
            scaleNegative();
            break;
            
        
        case 'u':
            rotateNegativeX();
            break;
            
        case 'j':
            rotatePositiveX();
            break;
        
        case 'h':
            rotateNegativeY();
            break;
            
        case 'k':
            rotatePositiveY();
            break;
            
            
        case 'y':
            rotatePositiveZ();
            break;
            
        case 'i':
            rotateNegativeZ();
            break;
            
    }
    
    cout << endl;
    cout << "xRotation: " << xRotation << endl;
    cout << "yRotation: " << yRotation << endl;
    cout << "zRotation: " << zRotation << endl;
    cout << endl;
    cout << "scale: " << scale << endl;
    cout << endl;
}


void display(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    render();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1080, 720);
    glutInitWindowPosition(20, 20);
    
    glutCreateWindow("Carregar OBJ");
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    
    glutKeyboardFunc(keyboard);
    
    glutMouseFunc(handleMouse);
    glutMotionFunc(handleMouseMovement);
    //glutMouseWheelFunc(handleMouseWheel);

    glutTimerFunc(10, timer, 0);
    
    loadObj("objetos/mba1.obj");
    buildObj();
    
    glutMainLoop();
    return 0;
}








