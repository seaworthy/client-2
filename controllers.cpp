#include "controllers.h"
#include <QDebug>

CameraPositionController::CameraPositionController(QWidget *parent)
{
    layout = new QVBoxLayout;
    signalMapper = new QSignalMapper;
    for (int i = 0; i < 3; i++) {
        QSpinBox *spinBox = new QSpinBox;
        spinBoxes.append(spinBox);
        signalMapper->setMapping(spinBox, i);
        connect(spinBox, SIGNAL(valueChanged(int)),signalMapper,SLOT(map()));
        layout->addWidget(spinBox);
        qDebug() << spinBox;
    }

    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(notify(int)));
    setLayout(layout);

    qDebug("%d QSpinBox(s) created!", spinBoxes.count());
}
CameraPositionController::~CameraPositionController()
{
}
void CameraPositionController::notify(int id)
{
    qDebug()<<spinBoxes.at(id);
    qDebug()<<spinBoxes.at(id)->value();

}
