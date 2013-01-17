#ifndef MODEL_H
#define MODEL_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QTimer>

class Model : public QGLWidget
{
    Q_OBJECT
public:
    explicit Model(QWidget *parent = 0);
    ~Model();

protected:

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void keyPressEvent(QKeyEvent * e);

    void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
    void setDrawMode(int mode);
    void setWindowSize(int width, int height);
    void setViewMatrix(float x, float y, float z, float pitch, float heading, float roll);
    void setModelMatrix(float x, float y, float z, float rx, float ry, float rz);
    void setCameraX(float x)        { cameraPosition.setX(x); updateViewMatrix(); }
    void setCameraY(float y)        { cameraPosition.setY(y); updateViewMatrix(); }
    void setCameraZ(float z)        { cameraPosition.setZ(z); updateViewMatrix(); }
    void setCameraAngleX(float p)   { cameraAngle.setX(p); updateViewMatrix(); }
    void setCameraAngleY(float h)   { cameraAngle.setY(h); updateViewMatrix(); }
    void setCameraAngleZ(float r)   { cameraAngle.setZ(r); updateViewMatrix(); }
    float getCameraX()              { return cameraPosition.x(); }
    float getCameraY()              { return cameraPosition.y(); }
    float getCameraZ()              { return cameraPosition.z(); }
    float getCameraAngleX()         { return cameraAngle.x(); }
    float getCameraAngleY()         { return cameraAngle.y(); }
    float getCameraAngleZ()         { return cameraAngle.z(); }
    void setModelX(float x)         { modelPosition.setX(x); updateModelMatrix(); }
    void setModelY(float y)         { modelPosition.setY(y); updateModelMatrix(); }
    void setModelZ(float z)         { modelPosition.setZ(z); updateModelMatrix(); }
    void setModelAngleX(float angle)    { modelAngle.setX(angle); updateModelMatrix(); }
    void setModelAngleY(float angle)    { modelAngle.setY(angle); updateModelMatrix(); }
    void setModelAngleZ(float angle)    { modelAngle.setZ(angle); updateModelMatrix(); }
    float getModelX()               { return modelPosition.x(); }
    float getModelY()               { return modelPosition.y(); }
    float getModelZ()               { return modelPosition.z(); }
    float getModelAngleX()          { return modelAngle.x(); }
    float getModelAngleY()          { return modelAngle.y(); }
    float getModelAngleZ()          { return modelAngle.z(); }
    const float* getViewMatrixElements()        { return matrixView.constData(); }
    const float* getModelMatrixElements()       { return matrixModel.constData(); }
    const float* getModelViewMatrixElements()   { return matrixModelView.constData(); }
    const float* getProjectionMatrixElements()  { return matrixProjection.constData(); }
    void rotateCamera(int x, int y);
    void zoomCamera(int dist);
private:
    QTimer timer;
    void initializeLights();
    void setViewport(int x, int y, int width, int height);
    void setViewportSub(int left, int bottom, int width, int height, float nearPlane, float farPlane);
    void setFrustum(float l, float r, float b, float t, float n, float f);
    void setFrustum(float fovy, float ratio, float n, float f);
    void setOrthoFrustum(float l, float r, float b, float t, float n=-1, float f=1);

    void drawGrid(float size, float step);
    void drawAxis(float size);
    void drawSub1();
    void drawSub2();
    void drawFrustum(float fovy, float aspect, float near, float far);

    void updateModelMatrix();
    void updateViewMatrix();

//    void resetModelMatrix();
//    void resetViewMatrix();

    GLfloat bgColor[4];

    int windowWidth;
    int windowHeight;
    bool windowSizeChanged;
    bool drawModeChanged;
    int drawMode;

    float cameraAngleX;
    float cameraAngleY;
    float cameraDistance;

    QVector3D cameraPosition;
    QVector3D cameraAngle;

    QVector3D modelPosition;
    QVector3D modelAngle;

    QMatrix4x4 matrixView;
    QMatrix4x4 matrixModel;
    QMatrix4x4 matrixModelView;
    QMatrix4x4 matrixProjection;
};

#endif // MODEL_H
