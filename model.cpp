#include "model.h"
#include "teapot.h"
#include "camera.h"

const float DEG2RAD = 3.141593f / 180;
const float FOV_Y = 60.0f;              // vertical FOV in degree
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 100.0f;
const float CAMERA_ANGLE_X = 45.0f;     // pitch in degree
const float CAMERA_ANGLE_Y = -45.0f;    // heading in degree
const float CAMERA_DISTANCE = 25.0f;    // camera distance

Model::Model(QWidget *parent):QGLWidget(parent)
{

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer.start(16);


    bgColor[0] = bgColor[1] = bgColor[2] = bgColor[3] = 0.f;
    cameraPosition = QVector3D(0, 0, 0);
    cameraAngle = QVector3D(0, 0, 0);
    modelPosition = QVector3D(0, 0, 0);
    modelAngle = QVector3D(0, 0, 0);

    matrixView.setToIdentity();
    matrixModel.setToIdentity();
    matrixModelView.setToIdentity();
    matrixProjection.setToIdentity();
}
Model::~Model()
{
}
void Model::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);

    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(bgColor[0],bgColor[1],bgColor[2],bgColor[3]);
    glClearStencil(0);
    glClearDepth(1.f);
    glDepthFunc(GL_LEQUAL);

    initializeLights();
    //    resetModelMatrix();
    //    resetViewMatrix();
}
void Model::resizeGL(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    windowSizeChanged = true;
}
void Model::paintGL()
{
    drawSub1();
    drawSub2();

    // post frame
    if(windowSizeChanged)
    {
        setViewport(0, 0, windowWidth, windowHeight);
        windowSizeChanged = false;
    }

    if(drawModeChanged)
    {
        if(drawMode == 0)           // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)      // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else if(drawMode == 2)      // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        drawModeChanged = false;
    }
}
void Model::keyPressEvent(QKeyEvent *e)
{
    qDebug("Key: %d",  e->key());
    if (e->key() == 81) // Q
        setCameraX(5);
    if (e->key() == 87) // W
        setCameraX(-5);
    if (e->key() == 90) // Z
        cameraDistance += 1.f;
    if (e->key() == 88) // X
        cameraDistance -= 1.f;
    if (e->key() == 16777234) // LEFT
        cameraAngleY -= 1.f;
    if (e->key() == 16777236) // RIGHT
        cameraAngleY += 1.f;
    if (e->key() == 16777235) // UP
        cameraAngleX -= 1.f;
    if (e->key() == 16777237) // DOWN
        cameraAngleX += 1.f;
}
void Model::initializeLights() {
    GLfloat ambient[] = {0.f, 0.f, 0.f, 1.f};
    GLfloat diffuse[] = {0.9f, 0.9f, 0.9f, 1.f};
    GLfloat specular[] = {1.f, 1.f ,1.f, 1.f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    GLfloat lightPosition[4] = {0.f,0.f,1.f,0.f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_LIGHT0);
}
void Model::setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
    QVector4D left, up, forward, position;
    QVector3D vector;

    //determine forward vector (direction reversed because of its camera)
    forward = QVector4D(posX - targetX, posY - targetY, posZ - targetZ, 0.f);
    forward.normalize();

    //assume up direction is straight up
    up = QVector4D(0.f,1.f,0.f,0);


    //compute left vector with corss product
    vector.crossProduct(up.toVector3D(), forward.toVector3D());
    left = vector.toVector4D();
    left.setW(1.0f);

    //recompute orthogonal up vector
    vector.crossProduct(forward.toVector3D(), left.toVector3D());
    up = vector.toVector4D();
    up.setW(0.0f);

    //camera position
    position = QVector4D(-posX, -posY, -posZ, 1.0f);

    //copy axis vectors to matrix
    matrixView.setToIdentity();
    matrixView.setColumn(0, left);
    matrixView.setColumn(1, up);
    matrixView.setColumn(2, forward);
    matrixView.setColumn(3, position);
}
void Model::setViewport(int x, int y, int w, int h)
{
    glViewport((GLsizei)x, (GLsizei)y, (GLsizei) w, (GLsizei) h);
    setFrustum(FOV_Y, (float)(w)/h, NEAR_PLANE, FAR_PLANE); // FOV, AspectRatio, NearClip, FarClip

    // copy projection matrix to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrixProjection.transposed().constData());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void Model::setViewportSub(int x, int y, int width, int height, float nearPlane, float farPlane)
{
    // set viewport
    glViewport(x, y, width, height);
    glScissor(x, y, width, height);

    // set perspective viewing frustum
    setFrustum(FOV_Y, (float)(width)/height, nearPlane, farPlane); // FOV, AspectRatio, NearClip, FarClip

    // copy projection matrix to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrixProjection.transposed().constData());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void Model::setDrawMode(int mode)
{
    if(drawMode != mode)
    {
        drawModeChanged = true;
        drawMode = mode;
    }
}
void Model::setFrustum(float fovY, float aspectRatio, float front, float back)
{
    float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
    float height = front * tangent;           // half height of near plane
    float width = height * aspectRatio;       // half width of near plane

    // params: left, right, bottom, top, near, far
    setFrustum(-width, width, -height, height, front, back);
}
void Model::setFrustum(float l, float r, float b, float t, float n, float f)
{
    matrixProjection.setToIdentity();
    matrixProjection.setRow(0, QVector4D(2 * n / (r - l), 0, (r + l) / (r - l) , 0));
    matrixProjection.setRow(1, QVector4D(0, 2 * n / (t - b), (t + b) / (t - b) , 0));
    matrixProjection.setRow(2, QVector4D(0, 0, -(f + n) / (f - n) ,-(2 * f * n) / (f - n)));
    matrixProjection.setRow(3, QVector4D(0, 0, -1 , 0));
}
void Model::setOrthoFrustum(float l, float r, float b, float t, float n, float f)
{
    matrixProjection.setToIdentity();
    matrixProjection.setRow(0, QVector4D(2 / (r - l), 0, 0, -(r + l) / (r - l)));
    matrixProjection.setRow(1, QVector4D(0, 2 / (t - b), 0, -(t + b) / (t - b)));
    matrixProjection.setRow(2, QVector4D(0, 0, -2 / (f - n), -(f + n) / (f - n)));
    matrixProjection.setRow(3, QVector4D(0,0,0,1));

    //    matrixProjection.identity();
    //    matrixProjection[0]  =  2 / (r - l);
    //    matrixProjection[3]  =  -(r + l) / (r - l);
    //    matrixProjection[5]  =  2 / (t - b);
    //    matrixProjection[7]  =  -(t + b) / (t - b);
    //    matrixProjection[10] = -2 / (f - n);
    //    matrixProjection[11] = -(f + n) / (f - n);
}
void Model::setViewMatrix(float x, float y, float z, float pitch, float heading, float roll)
{
    cameraPosition = QVector3D(x,y,z);
    cameraAngle = QVector3D(pitch, heading, roll);
    updateViewMatrix();
}
void Model::setModelMatrix(float x, float y, float z, float rx, float ry, float rz)
{
    modelPosition = QVector3D(x,y,z);
    modelAngle = QVector3D(rx,ry,rz);
    updateModelMatrix();
}
void Model::updateViewMatrix()
{
    //    matrixModel.identity();
    matrixView.setToIdentity();

    //    matrixView.translate(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);
    matrixView.translate(cameraPosition.operator *=(-1));

    QVector3D x = QVector3D(1.f, 0.f, 0.f);
    QVector3D y = QVector3D(0.f, 1.f, 0.f);
    QVector3D z = QVector3D(0.f, 0.f, 1.f);

    //    matrixView.rotateX(-cameraAngle[0]);    // pitch
    matrixView.rotate(-cameraAngle.x(), x);

    //    matrixView.rotateY(-cameraAngle[1]);    // heading
    matrixView.rotate(-cameraAngle.y(), y);

    //    matrixView.rotateZ(-cameraAngle[2]);    // roll
    matrixView.rotate(-cameraAngle.z(), z);

    matrixModelView = matrixView * matrixModel;
}
void Model::updateModelMatrix()
{
    //    matrixModel.identity();
    matrixModel.setToIdentity();

    QVector3D x = QVector3D(1.f, 0.f, 0.f);
    QVector3D y = QVector3D(0.f, 1.f, 0.f);
    QVector3D z = QVector3D(0.f, 0.f, 1.f);

    //    matrixModel.rotateZ(modelAngle[2]);
    matrixModel.rotate(modelAngle.z(), z);

    //    matrixModel.rotateY(modelAngle[1]);
    matrixModel.rotate(modelAngle.y(), y);

    //    matrixModel.rotateX(modelAngle[0]);
    matrixModel.rotate(modelAngle.x(), x);

    //    matrixModel.translate(modelPosition[0], modelPosition[1], modelPosition[2]);
    matrixModel.translate(modelPosition);

    matrixModelView = matrixView * matrixModel;
}
void Model::drawSub1()
{
    setViewportSub(0, windowHeight/2, windowWidth, windowHeight/2, 1, 10);

    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glPushMatrix();
    glLoadMatrixf(matrixView.transposed().constData());
    drawGrid(10, 1);
    glLoadMatrixf(matrixModelView.transposed().constData());
    drawAxis(4);
    drawTeapot();

    glPopMatrix();
}
void Model::drawSub2()
{
    // set bottom viewport
    setViewportSub(0, 0, windowWidth, windowHeight/2, NEAR_PLANE, FAR_PLANE);

    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);   // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glPushMatrix();

    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0); // pitch
    glRotatef(cameraAngleY, 0, 1, 0); // heading

    drawGrid(10, 1);

    glPushMatrix();
    glTranslatef(modelPosition.x(), modelPosition.y(), modelPosition.z());
    glRotatef(modelAngle.x(), 1, 0, 0);
    glRotatef(modelAngle.y(), 0, 1, 0);
    glRotatef(modelAngle.z(), 0, 0, 1);
    drawAxis(4);
    drawTeapot();
    glPopMatrix();

    // draw the camera
    glPushMatrix();
    glTranslatef(cameraPosition.x(), cameraPosition.y(), cameraPosition.z());
    glRotatef(cameraAngle.x(), 1, 0, 0);
    glRotatef(cameraAngle.y(), 0, 1, 0);
    glRotatef(cameraAngle.z(), 0, 0, 1);

    drawCamera();
    drawFrustum(FOV_Y, 1, 1, 10);
    glPopMatrix();

    glPopMatrix();
}
void Model::drawGrid(float size, float step)
{
    // disable lighting
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);

    glColor3f(0.3f, 0.3f, 0.3f);
    for(float i=step; i <= size; i+= step)
    {
        glVertex3f(-size, 0,  i);   // lines parallel to X-axis
        glVertex3f( size, 0,  i);
        glVertex3f(-size, 0, -i);   // lines parallel to X-axis
        glVertex3f( size, 0, -i);

        glVertex3f( i, 0, -size);   // lines parallel to Z-axis
        glVertex3f( i, 0,  size);
        glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
        glVertex3f(-i, 0,  size);
    }

    // x-axis
    glColor3f(0.5f, 0, 0);
    glVertex3f(-size, 0, 0);
    glVertex3f( size, 0, 0);

    // z-axis
    glColor3f(0,0,0.5f);
    glVertex3f(0, 0, -size);
    glVertex3f(0, 0,  size);

    glEnd();

    // enable lighting back
    glEnable(GL_LIGHTING);
}
void Model::drawAxis(float size)
{
    glDepthFunc(GL_ALWAYS);     // to avoid visual artifacts with grid lines
    glDisable(GL_LIGHTING);
    glPushMatrix();             //NOTE: There is a bug on Mac misbehaviours of
    //      the light position when you draw GL_LINES
    //      and GL_POINT. Remember the matrix.

    // draw axis
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(size, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, size, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, size);
    glEnd();
    glLineWidth(1);

    // draw arrows(actually big square dots)
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(1, 0, 0);
    glVertex3f(size, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, size, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, size);
    glEnd();
    glPointSize(1);

    // restore default settings
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
}
void Model::drawFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane)
{
    float tangent = tanf(fovY/2 * DEG2RAD);
    float nearHeight = nearPlane * tangent;
    float nearWidth = nearHeight * aspectRatio;
    float farHeight = farPlane * tangent;
    float farWidth = farHeight * aspectRatio;

    // compute 8 vertices of the frustum
    float vertices[8][3];
    // near top right
    vertices[0][0] = nearWidth;     vertices[0][1] = nearHeight;    vertices[0][2] = -nearPlane;
    // near top left
    vertices[1][0] = -nearWidth;    vertices[1][1] = nearHeight;    vertices[1][2] = -nearPlane;
    // near bottom left
    vertices[2][0] = -nearWidth;    vertices[2][1] = -nearHeight;   vertices[2][2] = -nearPlane;
    // near bottom right
    vertices[3][0] = nearWidth;     vertices[3][1] = -nearHeight;   vertices[3][2] = -nearPlane;
    // far top right
    vertices[4][0] = farWidth;      vertices[4][1] = farHeight;     vertices[4][2] = -farPlane;
    // far top left
    vertices[5][0] = -farWidth;     vertices[5][1] = farHeight;     vertices[5][2] = -farPlane;
    // far bottom left
    vertices[6][0] = -farWidth;     vertices[6][1] = -farHeight;    vertices[6][2] = -farPlane;
    // far bottom right
    vertices[7][0] = farWidth;      vertices[7][1] = -farHeight;    vertices[7][2] = -farPlane;

    float colorLine1[4] = { 0.7f, 0.7f, 0.7f, 0.7f };
    float colorLine2[4] = { 0.2f, 0.2f, 0.2f, 0.7f };
    float colorPlane[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw the edges around frustum
    glBegin(GL_LINES);
    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[4]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[5]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[6]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[7]);
    glEnd();

    glColor4fv(colorLine1);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);
    glEnd();

    glColor4fv(colorLine1);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glEnd();

    // draw near and far plane
    glColor4fv(colorPlane);
    glBegin(GL_QUADS);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);
    glEnd();

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

