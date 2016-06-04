#include "About.h"
#include "ui_About.h"
#include "caoslink.h"

#include <QDateTime>
#include <CSdk/CFont.h>
#include <CSdk/CNotify.h>

About::About(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::About)
{
    qDebug("About Init");
    ui->setupUi(this);
    ui->versionValueLabel->setAlignment(Qt::AlignLeft);
    ui->versionValueLabel->setText(COACTIONOS_LINK_VERSION);
    ui->coactionosNameIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosNameIconLabel->setEnabled(false);
    ui->coactionosNameIconLabel->setObjectName("acceptLabel");
    ui->coactionosVersionIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosVersionIconLabel->setEnabled(false);
    ui->coactionosVersionIconLabel->setObjectName("acceptLabel");
    ui->coactionosClockIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosClockIconLabel->setEnabled(false);
    ui->coactionosClockIconLabel->setObjectName("acceptLabel");
    ui->coactionosSignatureIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosSignatureIconLabel->setEnabled(false);
    ui->coactionosSignatureIconLabel->setObjectName("acceptLabel");
    ui->coactionosTimeButton->setText(CFont::iconTime());
    ui->coactionosTimeButton->setToolTip("Set time on device to computer time");
    ui->coactionosTimeButton->setObjectName("blueButton");
    ui->coactionosTimeButton->setEnabled(false);
    ui->coactionosTimeOfDayLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosTimeOfDayLabel->setObjectName("acceptLabel");
    ui->coactionosTimeOfDayLabel->setEnabled(false);

    ui->aboutLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " About");
    ui->kernelLabel->setText(CFont::icomoon(CFont::iconKernel()) + " Kernel");
    ui->creditsLabel->setText(CFont::icomoon(CFont::iconReorder()) + " Credits");

    connect(&timeTimer, SIGNAL(timeout()), this, SLOT(updateTimeOfDay()));
    qDebug("About Init Complete");
}

About::~About()
{
    delete ui;
}

void About::setLink(CLink *d){
    CLinkPtr::setLink(d);
    connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}


void About::connected(bool value){
    if( link() != 0 ){
        if( link()->is_bootloader() == true ){
            value = false;
        }
    }
    ui->coactionosNameIconLabel->setAlignment(Qt::AlignCenter);
    ui->coactionosNameIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosVersionIconLabel->setAlignment(Qt::AlignCenter);
    ui->coactionosVersionIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosClockIconLabel->setAlignment(Qt::AlignCenter);
    ui->coactionosClockIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosSignatureIconLabel->setAlignment(Qt::AlignCenter);
    ui->coactionosSignatureIconLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    ui->coactionosNameIconLabel->setEnabled(false);
    ui->coactionosClockIconLabel->setEnabled(false);
    ui->coactionosSignatureIconLabel->setEnabled(false);
    ui->coactionosVersionIconLabel->setEnabled(false);



    ui->coactionosTimeButton->setEnabled(value);
    ui->coactionosTimeOfDayLabel->setEnabled(value);
    if( value == false ){
        timeTimer.stop();
        ui->coactionosTimeOfDayLabel->setAlignment(Qt::AlignCenter);
        ui->coactionosTimeOfDayLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Available");
    } else {
        //start the timer
        if( !link()->is_bootloader() ){
            refreshKernel();
            updateTimeOfDay();
            timeTimer.setInterval(1000);
            timeTimer.start();
        }
    }
}

void About::updateTimeOfDay(void){
    struct tm gt;
    QString theTime;
    if( this->isVisible() ){
        if( link()->get_time(&gt) == 0 ){
            //check to see if the time is valid
            if( (gt.tm_year > 20) && (gt.tm_year < 2000) &&
                    (gt.tm_mday > 0) && (gt.tm_mday < 32) &&
                    (gt.tm_sec > -1) && (gt.tm_sec < 62) &&
                    (gt.tm_min > -1) && (gt.tm_min < 62) &&
                    (gt.tm_mon > -1) && (gt.tm_mon < 12) &&
                    (gt.tm_hour > -1) && (gt.tm_hour < 24)
                    ){
                theTime = asctime(&gt);
                ui->coactionosTimeOfDayLabel->setAlignment(Qt::AlignLeft);
                ui->coactionosTimeOfDayLabel->setEnabled(true);
                ui->coactionosTimeOfDayLabel->setText(theTime.remove("\n") );
            } else {
                ui->coactionosTimeOfDayLabel->setEnabled(false);
                ui->coactionosTimeOfDayLabel->setText(CFont::fontAwesome(CFont::iconInfoSign()) + " Time not Set");
            }


        } else {
            ui->coactionosTimeOfDayLabel->setAlignment(Qt::AlignLeft);
            ui->coactionosTimeOfDayLabel->setEnabled(false);
        }
    }
}

void About::refreshKernel()
{
    //load the version, signature, and CPU speed
    int fd;
    sys_attr_t attr;

    fd = link()->open("/dev/sys", LINK_O_RDWR);
    if( fd < 0 ){
        CNotify::updateStatus("Failed to load kernel info");
        return;
    }

    if( link()->ioctl(fd, I_SYS_GETATTR, &attr) < 0 ){
        CNotify::updateStatus("Failed to load kernel info");
        return;
    }


    if( link()->close(fd) < 0 ){
        CNotify::updateStatus("Failed to load kernel info");
    }

    ui->coactionosClockIconLabel->setAlignment(Qt::AlignLeft);
    ui->coactionosClockIconLabel->setText(QString::number(attr.cpu_freq) );
    ui->coactionosSignatureIconLabel->setAlignment(Qt::AlignLeft);
    ui->coactionosSignatureIconLabel->setText("0x" + QString::number(attr.signature, 16) );
    ui->coactionosVersionIconLabel->setAlignment(Qt::AlignLeft);
    ui->coactionosVersionIconLabel->setText(QString(attr.sys_version) + " (" + QString(attr.version) + ")");
    ui->coactionosNameIconLabel->setAlignment(Qt::AlignLeft);
    ui->coactionosNameIconLabel->setText(attr.name);
    ui->coactionosNameIconLabel->setEnabled(true);
    ui->coactionosClockIconLabel->setEnabled(true);
    ui->coactionosSignatureIconLabel->setEnabled(true);
    ui->coactionosVersionIconLabel->setEnabled(true);
}

void About::on_coactionosTimeButton_clicked()
{
    time_t t;
    struct tm * gt;
    time(&t);
    gt = localtime(&t);

    if( link()->set_time(gt) < 0 ){
        //error
        return;
    }
}
