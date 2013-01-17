#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QHBoxLayout>

#include "model.h"
#include "controllers.h"

class View : public QWidget
{
    Q_OBJECT

public:
    explicit View(QWidget *parent = 0);
    ~View();
private:
     Model *model;
     QHBoxLayout *layout;
     CameraPositionController *cameraPositionController;
};

#endif
// VIEW_H
