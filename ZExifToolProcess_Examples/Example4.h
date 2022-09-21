#ifndef EXAMPLE4_H
#define EXAMPLE4_H

#include <QWidget>
#include "ZExifTool/ZExifToolProcess.h"

namespace Ui {
   class Example4;
}

class Example4 : public QWidget {
    Q_OBJECT

// METHODS
public:
    explicit Example4(QWidget *parent = nullptr);
    ~Example4();

// VARIABLES
private:
    Ui::Example4 *ui;
    ZExifToolProcess *etProcess= 0;
};

#endif // EXAMPLE4_H
