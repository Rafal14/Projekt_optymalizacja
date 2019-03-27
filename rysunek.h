#ifndef RYSUNEK_H
#define RYSUNEK_H

#include <QDialog>

namespace Ui {
class rysunek;
}

class rysunek : public QDialog
{
    Q_OBJECT

public:
    explicit rysunek(QWidget *parent = 0);
    ~rysunek();

private:
    Ui::rysunek *ui;
};

#endif // RYSUNEK_H
