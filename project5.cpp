#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define PI 3.14159265

GLuint textures[10];
char* textureFiles[10] = {
    "/Users/khudeyesehde/Desktop/compgra/2k_stars_milky_way.jpg", // [0] Background
    "/Users/khudeyesehde/Desktop/compgra/2k_sun.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_mercury.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_venus_surface.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_earth_daymap.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_mars.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_jupiter.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_saturn.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_uranus.jpg",
    "/Users/khudeyesehde/Desktop/compgra/2k_neptune.jpg"
};

typedef struct {
    float distance;
    float size;
    float orbitSpeed;
    float rotationSpeed;
    float orbitAngle;
    float rotationAngle;
} Planet;

Planet planets[8];
char* planetNames[8] = {
    "Mercury", "Venus", "Earth", "Mars",
    "Jupiter", "Saturn", "Uranus", "Neptune"
};

float cameraAngleX = 20.0f, cameraAngleY = -30.0f;
float cameraDistance = 20.0f;
float simulationSpeed = 1.0f;

int lastMouseX, lastMouseY;
bool mouseLeftDown = false;

GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) { printf("Failed to load %s\n", filename); exit(1); }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format;
    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4) format = GL_RGBA;
    else {
        printf("Unsupported format: %s\n", filename);
        exit(1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

void initPlanets() {
    planets[0] = (Planet){1.5, 0.1, 4.7, 6.0, 0, 0};
    planets[1] = (Planet){2.2, 0.2, 3.5, 5.0, 0, 0};
    planets[2] = (Planet){3.0, 0.25, 2.9, 4.0, 0, 0};
    planets[3] = (Planet){3.8, 0.15, 2.4, 3.5, 0, 0};
    planets[4] = (Planet){5.2, 0.5, 1.3, 2.5, 0, 0};
    planets[5] = (Planet){6.5, 0.4, 1.0, 2.0, 0, 0};
    planets[6] = (Planet){7.8, 0.35, 0.7, 1.5, 0, 0};
    planets[7] = (Planet){9.0, 0.33, 0.5, 1.0, 0, 0};
}

void drawSphere(float radius, GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, radius, 48, 48);
    gluDeleteQuadric(quad);
}

void drawBackground() {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glColor3f(1, 1, 1);
    glRotatef(90, 1, 0, 0);
    GLUquadric* sphere = gluNewQuadric();
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricOrientation(sphere, GLU_INSIDE);
    gluSphere(sphere, 50, 48, 48);
    gluDeleteQuadric(sphere);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawOrbits() {
    glColor3f(0.3, 0.3, 0.3);
    for (int i = 0; i < 8; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 100; j++) {
            float theta = 2.0f * PI * j / 100;
            float x = planets[i].distance * cos(theta);
            float z = planets[i].distance * sin(theta);
            glVertex3f(x, 0.0f, z);
        }
        glEnd();
    }
}

void drawLabel(float x, float y, float z, const char* text) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos3f(x, y, z);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *text++);
    }
    glEnable(GL_LIGHTING);
}

void drawSun() {
    GLfloat emission[] = {1.0, 1.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    drawSphere(0.7, textures[1]);
    GLfloat noEmission[] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
}

void drawPlanets() {
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        float angle = planets[i].orbitAngle * PI / 180.0f;
        float x = planets[i].distance * cos(angle);
        float z = planets[i].distance * sin(angle);
        glTranslatef(x, 0, z);
        glRotatef(planets[i].rotationAngle, 0, 1, 0);
        drawSphere(planets[i].size, textures[i + 2]);
        drawLabel(0, planets[i].size + 0.05f, 0, planetNames[i]);
        glPopMatrix();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);
    glRotatef(cameraAngleY, 0, 1, 0);

    drawBackground();
    drawOrbits();
    drawSun();
    drawPlanets();

    glutSwapBuffers();
}

void idle() {
    for (int i = 0; i < 8; i++) {
        planets[i].orbitAngle += planets[i].orbitSpeed * 0.1f * simulationSpeed;
        if (planets[i].orbitAngle > 360.0f) planets[i].orbitAngle -= 360.0f;

        planets[i].rotationAngle += planets[i].rotationSpeed * 0.1f * simulationSpeed;
        if (planets[i].rotationAngle > 360.0f) planets[i].rotationAngle -= 360.0f;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 1, 100);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouseLeftDown = (state == GLUT_DOWN);
        lastMouseX = x;
        lastMouseY = y;
    } else if (button == 3) {
        cameraDistance -= 1.0f;
    } else if (button == 4) {
        cameraDistance += 1.0f;
    }
    if (cameraDistance < 2.0f) cameraDistance = 2.0f;
    if (cameraDistance > 50.0f) cameraDistance = 50.0f;
    glutPostRedisplay();
}

void motion(int x, int y) {
    if (mouseLeftDown) {
        cameraAngleY += (x - lastMouseX) * 0.5f;
        cameraAngleX += (y - lastMouseY) * 0.5f;
        if (cameraAngleX < -90) cameraAngleX = -90;
        if (cameraAngleX > 90) cameraAngleX = 90;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '+' || key == '=') cameraDistance -= 0.5f;
    if (key == '-' || key == '_') cameraDistance += 0.5f;
    if (key == 'w') cameraAngleX -= 2;
    if (key == 's') cameraAngleX += 2;
    if (key == 'a') cameraAngleY -= 2;
    if (key == 'd') cameraAngleY += 2;
    if (key == '[') simulationSpeed -= 0.1f;
    if (key == ']') simulationSpeed += 0.1f;

    if (cameraDistance < 2.0f) cameraDistance = 2.0f;
    if (cameraDistance > 50.0f) cameraDistance = 50.0f;
    if (cameraAngleX < -90) cameraAngleX = -90;
    if (cameraAngleX > 90) cameraAngleX = 90;
    if (simulationSpeed < 0.1f) simulationSpeed = 0.1f;
    if (simulationSpeed > 5.0f) simulationSpeed = 5.0f;

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP) cameraAngleX -= 2;
    if (key == GLUT_KEY_DOWN) cameraAngleX += 2;
    if (key == GLUT_KEY_LEFT) cameraAngleY -= 2;
    if (key == GLUT_KEY_RIGHT) cameraAngleY += 2;

    if (cameraAngleX < -90) cameraAngleX = -90;
    if (cameraAngleX > 90) cameraAngleX = 90;
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lightPos[] = {0, 0, 0, 1};
    GLfloat diffuseLight[] = {1.0, 1.0, 0.8, 1.0};
    GLfloat ambientLight[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat specularLight[] = {1.0, 1.0, 1.0, 1.0};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    for (int i = 0; i < 10; i++)
        textures[i] = loadTexture(textureFiles[i]);

    initPlanets();
    glClearColor(0, 0, 0.05, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("Solar System Viewer");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}
