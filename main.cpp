#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/freeglut.h>
#include <string>
#include <sstream>

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
float scaleUnit = 0.01f;

bool isMouseRotationActive = false;
bool isMouseTranslationActive = false;
float mouseX, mouseY;


unsigned int exibitionList;
vector<vector<float>> vertices;
vector<vector<int>> faces;
vector<vector<float>> textures;
vector<vector<float>> normals;

void keyboard(unsigned char key, int x, int y);

void buildObj() {
    exibitionList = glGenLists(1);
    glNewList(exibitionList, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);

        for (const auto& face : faces) {
            int nVerts = face.size() / 3;

            // fan triangulation
            for (int i = 1; i < nVerts - 1; i++) {
                int idxs[3][3] = {
                    {face[0],        face[1],        face[2]},        // v0
                    {face[i*3],      face[i*3+1],    face[i*3+2]},    // vi
                    {face[(i+1)*3],  face[(i+1)*3+1],face[(i+1)*3+2]} // vi+1
                };

                for (int k = 0; k < 3; k++) {
                    int v  = idxs[k][0];
                    int vt = idxs[k][1];
                    int vn = idxs[k][2];

                    if (vn >= 0 && vn < (int)normals.size())
                        glNormal3fv(normals[vn].data());
                    if (vt >= 0 && vt < (int)textures.size())
                        glTexCoord2fv(textures[vt].data());
                    if (v >= 0 && v < (int)vertices.size())
                        glVertex3fv(vertices[v].data());
                }
            }
        }

        glEnd();
        glPopMatrix();
    }
    glEndList();
}

void loadObj(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Erro: Arquivo '" << filename << "' não encontrado!" << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string type;
        ss >> type;

        if (type == "v") {
            vector<float> v(3);
            ss >> v[0] >> v[1] >> v[2];
            vertices.push_back(v);
        } else if (type == "vn") {
            vector<float> vn(3);
            ss >> vn[0] >> vn[1] >> vn[2];
            normals.push_back(vn);
        } else if (type == "vt") {
            vector<float> vt(2);
            ss >> vt[0] >> vt[1];
            textures.push_back(vt);
        } else if (type == "f") {
            vector<int> indices;
            string part;
            while (ss >> part) {
                size_t slash1 = part.find('/');
                size_t slash2 = string::npos;
                if (slash1 != string::npos) slash2 = part.find('/', slash1 + 1);

                int v_idx  = stoi(part.substr(0, slash1)) - 1;
                int vt_idx = -1;
                int vn_idx = -1;

                if (slash1 != string::npos) {
                    if (slash2 != string::npos) {
                        if (slash1 + 1 < slash2)
                            vt_idx = stoi(part.substr(slash1 + 1, slash2 - (slash1 + 1))) - 1;
                        vn_idx = stoi(part.substr(slash2 + 1)) - 1;
                    } else {
                        if (part.at(slash1 + 1) == '/')
                            vn_idx = stoi(part.substr(slash1 + 2)) - 1;
                        else
                            vt_idx = stoi(part.substr(slash1 + 1)) - 1;
                    }
                }

                indices.push_back(v_idx);
                indices.push_back(vt_idx);
                indices.push_back(vn_idx);
            }
            faces.push_back(indices);
        }
    }
    file.close();
}


void render()
{
    glPushMatrix();
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
    
    glutCreateWindow("Desenvolvimento M1 - 3D e Arquivos .obj");
    
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
    
    loadObj("objetos/porsche.obj");
    buildObj();
    
    glutMainLoop();
    return 0;
}







