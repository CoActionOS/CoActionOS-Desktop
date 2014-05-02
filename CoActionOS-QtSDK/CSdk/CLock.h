#ifndef CLOCK_H
#define CLOCK_H

#include "CCheckbox.h"

class CLock : public CCheckbox
{
    Q_OBJECT
public:
    CLock(QWidget * parent = 0);

private:
    void refresh();

};

#endif // CLOCK_H
