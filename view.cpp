#include "view.h"
#include <QDebug>

View::View(QWidget *parent):QWidget(parent)
{
    layout = new QHBoxLayout;

//    labelLeftSection = new QLabel;
//    labelLeftSection->setText(tr("null"));

//    labelRightSection = new QLabel;
//    labelRightSection->setText(tr("null"));

    model = new Model;
    layout->addWidget(model);

    cameraPositionController = new CameraPositionController;    
    layout->addWidget(cameraPositionController);

    setLayout(layout);
}
View::~View()
{
}

