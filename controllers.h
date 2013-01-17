#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSignalMapper>
#include <QSlider>

class CameraPositionController : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPositionController(QWidget *parent = 0);
    ~CameraPositionController();

private:
    QSignalMapper *signalMapper;
    QVBoxLayout *layout;
    QList<QSpinBox *> spinBoxes;

private slots:
    void notify(int id);
};

#endif // CONTROLLERS_H
