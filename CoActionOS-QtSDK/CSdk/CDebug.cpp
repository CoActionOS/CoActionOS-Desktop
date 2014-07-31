#include "CDebug.h"
#include "ui_CDebug.h"

#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QScrollBar>

#include "CFont.h"
#include "CNotify.h"
#include "CSettings.h"

static CDebug * message_obj_ptr = 0;

static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

void linkDebugCallback(link_debug_context_t * context){
    QtMsgType type;
    QMessageLogContext messageContext(context->file, context->line, context->function, "TBD");
    QString msg(context->msg);

    switch(context->type){
    case LINK_DEBUG_MESSAGE:
        type = QtDebugMsg;
        break;
    case LINK_DEBUG_WARNING:
        type = QtWarningMsg;
        break;
    case LINK_DEBUG_CRITICAL:
        type = QtCriticalMsg;
        break;
    case LINK_DEBUG_FATAL:
        type = QtFatalMsg;
        break;
    default:
        type = QtDebugMsg;
        break;
    }

    messageOutput(type, messageContext, msg);
}


void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if( message_obj_ptr != 0 ){
        message_obj_ptr->appendDebugMessage(type, context, msg);
    }
}

CDebug::CDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDebug)
{
    ui->setupUi(this);
    ui->startTraceButton->setObjectName("greenButton");
    ui->startTraceButton->setText(CFont::iconPlay());
    ui->startTraceButton->setToolTip("Create trace analyzer");
    ui->startTraceButton->setEnabled(false);
    ui->stopTraceButton->setObjectName("redButton");
    ui->stopTraceButton->setText(CFont::iconStop());
    ui->stopTraceButton->setToolTip("Shutdown trace analyzer");
    ui->stopTraceButton->setEnabled(false);
    ui->showTraceButton->setObjectName("defaultButton");
    ui->showTraceButton->setText(CFont::iconBug());
    ui->showTraceButton->setToolTip("Show/hide trace");
    ui->showAssemblyButton->setObjectName("defaultButton");
    ui->showAssemblyButton->setText(CFont::iconCode());
    ui->showAssemblyButton->setToolTip("Show/hide code");
    ui->showLogButton->setObjectName("defaultButton");
    ui->showLogButton->setText(CFont::iconAlignLeft());
    ui->showLogButton->setToolTip("Show/hide log");
    ui->showSettingsButton->setObjectName("defaultButton");
    ui->showSettingsButton->setText(CFont::iconPreferences());
    ui->showSettingsButton->setToolTip("Show/hide preferences");
    ui->refreshPidButton->setObjectName("blueButton");
    ui->refreshPidButton->setText(CFont::iconRefresh());
    ui->refreshPidButton->setToolTip("Refresh PID list");
    ui->clearButton->setObjectName("defaultButton");
    ui->clearButton->setText(CFont::iconRemove());
    ui->clearButton->setToolTip("Clear trace and log");
    ui->errorCheckbox->setChecked();
    ui->logTextEdit->setWordWrapMode(QTextOption::NoWrap);
    ui->messageCheckbox->setText(CFont::iconCommentAlt());
    ui->messageCheckbox->setToolTip("Enable/disable messages");
    ui->warningCheckbox->setText(CFont::iconWarningSign());
    ui->warningCheckbox->setToolTip("Enable/disable warnings");
    ui->errorCheckbox->setText(CFont::iconRemoveSign());
    ui->errorCheckbox->setToolTip("Enable/disable errors");

    ui->traceListWidget->setToolTip("Trace");
    ui->logTextEdit->setToolTip("Log");
    ui->assemblyListWidget->setToolTip("Assembly code");

    ui->traceListWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->assemblyListWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);

    ui->traceLabel->setText(CFont::iconBug() + " Trace");
    ui->assemblyLabel->setText(CFont::iconCode() + " Code");
    ui->logLabel->setText(CFont::iconAlignLeft() + " Log");


    ui->refreshPidButton->setEnabled(false);

    if( ui->showAssemblyButton->isVisible() ){
        toggleSettings();
    }

    assemblyVisible = false;
    logVisible = false;
    traceVisible = true;

    trace_id = 0;
    connect(&traceTimer, SIGNAL(timeout()), this, SLOT(traceTimerExpired()));
}

CDebug::~CDebug()
{
    savePreferences();
    delete ui;
}

void CDebug::savePreferences(){
    CSettings settings(CSettings::userScope());

    settings.setKey("KEY_DEBUGSHOWASSEMBLY", assemblyVisible );
    settings.setKey("KEY_DEBUGSHOWTRACE", traceVisible );
    settings.setKey("KEY_DEBUGSHOWLOG", logVisible );
    settings.setKey("KEY_MESSAGECHECKBOX", ui->messageCheckbox->isChecked());
    settings.setKey("KEY_WARNINGCHECKBOX", ui->warningCheckbox->isChecked());
    settings.setKey("KEY_ERRORCHECKBOX", ui->errorCheckbox->isChecked());
}

void CDebug::resizeEvent(QResizeEvent * event){
    //hide some settings buttons if visible
    int w = width();
    if( w < 900 ){
        ui->showAssemblyButton->hide();
        ui->showTraceButton->hide();
        ui->showLogButton->hide();
        ui->showSettingsButton->hide();
        //CNotify::updateStatus("Hide Show Buttons");
    } else {
        ui->showAssemblyButton->show();
        ui->showTraceButton->show();
        ui->showLogButton->show();
        ui->showSettingsButton->show();
    }

    if( w < 700 ){
        ui->messageCheckbox->setText(CFont::iconCommentAlt());
        ui->warningCheckbox->setText(CFont::iconWarningSign());
        ui->errorCheckbox->setText(CFont::iconRemoveSign());
    } else {
        ui->messageCheckbox->setText("Messages");
        ui->warningCheckbox->setText("Warnings");
        ui->errorCheckbox->setText("Errors");
    }


    if( w < 550 ){
        //hide some buttons
        ui->pidComboBox->hide();
        ui->refreshPidButton->hide();
    } else {
        ui->pidComboBox->show();
        ui->refreshPidButton->show();
    }

    event = 0;
}

void CDebug::enableMessageHandler(){
    message_obj_ptr = this;
    qInstallMessageHandler(messageOutput);
    CLink::setDebugCallback(linkDebugCallback);
}

void CDebug::setLink(CLink * d){
    CLinkPtr::setLink(d);
    connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}

void CDebug::on_showTraceButton_clicked()
{
    ui->traceListWidget->setVisible( !ui->traceListWidget->isVisible() );


    if( ui->traceListWidget->isVisible() == false ){
        ui->assemblyListWidget->hide();
        ui->assemblyLabel->hide();
        ui->showAssemblyButton->setEnabled(false);
        ui->showLogButton->setEnabled(false);
        ui->logTextEdit->show();
    } else {
        ui->showAssemblyButton->show();
        ui->showAssemblyButton->setEnabled(true);
        ui->showLogButton->setEnabled(true);
    }

    this->traceVisible = ui->traceListWidget->isVisible();
    this->assemblyVisible = ui->assemblyListWidget->isVisible();

    ui->traceLabel->setVisible( traceVisible );

}

void CDebug::loadPreferences(){
    CSettings settings(CSettings::userScope());
    appPath = QString(settings.getStringKey(""));

    traceVisible = settings.getBoolKey("KEY_DEBUGSHOWTRACE");
    assemblyVisible = settings.getBoolKey("KEY_DEBUGSHOWASSEMBLY");
    logVisible = settings.getBoolKey("KEY_DEBUGSHOWLOG");

    ui->assemblyListWidget->setVisible(assemblyVisible);
    ui->traceListWidget->setVisible(traceVisible);
    ui->logTextEdit->setVisible(logVisible);
    ui->messageCheckbox->setChecked( settings.getBoolKey("KEY_MESSAGECHECKBOX") );
    ui->warningCheckbox->setChecked( settings.getBoolKey("KEY_WARNINGCHECKBOX") );
    ui->errorCheckbox->setChecked( settings.getBoolKey("KEY_ERRORCHECKBOX") );

    if( (ui->logTextEdit->isVisible() == false) && (ui->traceListWidget->isVisible() == false )){
        ui->traceListWidget->setVisible(true);
    }


    if( traceVisible ){
        ui->showLogButton->setEnabled(true);
        ui->showAssemblyButton->setEnabled(true);
    } else {
        ui->showLogButton->setEnabled(false);
        ui->showAssemblyButton->setEnabled(false);
    }

    if( logVisible ){
        ui->showTraceButton->setEnabled(true);
    } else {
        ui->showTraceButton->setEnabled(false);
    }

    ui->logLabel->setVisible( logVisible );
    ui->assemblyLabel->setVisible( assemblyVisible );
    ui->traceLabel->setVisible( traceVisible );

}




void CDebug::on_showAssemblyButton_clicked()
{
    ui->assemblyListWidget->setVisible( !ui->assemblyListWidget->isVisible() );
    assemblyVisible = ui->assemblyListWidget->isVisible();
    ui->assemblyLabel->setVisible(assemblyVisible);
}

void CDebug::on_showLogButton_clicked()
{
    ui->logTextEdit->setVisible( !ui->logTextEdit->isVisible() );

    logVisible = ui->logTextEdit->isVisible();

    if( ui->logTextEdit->isVisible() == false ){
        //must show trace is log is hidden
        ui->showTraceButton->setEnabled(false);
    } else {
        ui->showTraceButton->setEnabled(true);
    }

    ui->logLabel->setVisible(logVisible);
}

void CDebug::appendLog(QString text, const QColor & textColor){
    ui->logTextEdit->appendText(text, textColor);
}

void CDebug::connected(bool arg){
    CNotify notify;
    ui->startTraceButton->setEnabled(arg);
    ui->stopTraceButton->setEnabled(false);
    ui->pidComboBox->setEnabled(arg);
    ui->refreshPidButton->setEnabled(arg);
    if( !arg ){
        traceTimer.stop();
        if( trace_id != 0 ){
            //shutdown the currently active trace
            if( link()->trace_shutdown(trace_id) < 0 ){
                notify.execError("Failed to shutdown trace");
            }
            trace_id = 0;
        }
    }
}

void CDebug::appendDebugMessage(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msg){
    QColor color;
    QString output;
    QFileInfo fileInfo(context.file);

    if( ui->logTextEdit->isVisible() ){
        switch (type) {
        case QtDebugMsg:
            if( ui->messageCheckbox->isChecked() == false ){
                return;
            }
            color.setNamedColor("#5cb85c");
            break;
        case QtWarningMsg:
            if( ui->warningCheckbox->isChecked() == false ){
                return;
            }
            color.setNamedColor("#31708f");
            break;
        case QtCriticalMsg:
            if( ui->errorCheckbox->isChecked() == false ){
                return;
            }
            color.setNamedColor("#a94442");
            break;
        case QtFatalMsg:
            color.setNamedColor("#a94442");
            //abort();
            break;
        default:
            return;
        }
        output = QString(context.function) + ":" + msg + "\n";
        ui->logTextEdit->appendText(output, color);
    }
}


void CDebug::toggleSettings(){
    ui->pidComboBox->setVisible( !ui->showAssemblyButton->isVisible() );
    ui->refreshPidButton->setVisible( !ui->showAssemblyButton->isVisible() );
    ui->messageCheckbox->setVisible( !ui->showAssemblyButton->isVisible() );
    ui->warningCheckbox->setVisible( !ui->showAssemblyButton->isVisible() );
    ui->errorCheckbox->setVisible( !ui->showAssemblyButton->isVisible() );
    ui->showLogButton->setVisible( !ui->showAssemblyButton->isVisible() );
    ui->showTraceButton->setVisible( !ui->showAssemblyButton->isVisible() );
    ui->showAssemblyButton->setVisible( !ui->showAssemblyButton->isVisible() );
}

void CDebug::on_clearButton_clicked()
{
    ui->logTextEdit->clear();
    //ui->traceTextEdit->clear();
    ui->traceListWidget->clear();
    ui->assemblyListWidget->clear();
}

void CDebug::on_showSettingsButton_clicked()
{
    //toggleSettings();
}

void CDebug::updateStatus(const QString &msg){
    ui->logTextEdit->appendText(QDateTime::currentDateTime().toString("hh:mm:ss.zzz--") + msg + "\n", QColor("#333333"));
}


void CDebug::currentAppUpdated(QString workspace, QString project, QString conf){
    //load the s file to the app text area
    QString path(workspace + "/" + project + "/" + conf + "/" + project + ".s");
    appPath = path;
    CNotify::updateStatus("App set to " + path);

}

void CDebug::currentKernelUpdated(QString workspace, QString project, QString conf){
    QString path(workspace + "/" + project + "/" + conf + "/" + project + ".s");
    kernelPath = path;
    CNotify::updateStatus("Kernel set to " + path);
}


void CDebug::on_refreshPidButton_clicked()
{
    //put a list of the running processes in the PID combo box
    int fd;
    CNotify notify;
    int err;
    sys_taskattr_t task;
    QString desc;
    int i;

    if( link()->isConnected() == false ){
        notify.execNotConnected();
        return;
    }

    fd = link()->open("/dev/sys", LINK_O_RDWR);
    if( fd < 0 ){
        notify.execError("Failed to read running proceses");
        return;
    }

    ui->pidComboBox->clear();
    i = 0;
    do {
        task.tid = i++;
        err = link()->ioctl(fd, I_SYS_GETTASK, &task);
        if( err == 1 ){
            //add task to combo box
            if( (task.is_enabled != 0) && (task.is_thread == 0) ){
                desc = QString::number(task.pid) + ":" + task.name;
                ui->pidComboBox->addItem(desc);
            }
        }
        qApp->processEvents();
    } while( err >= 0 );

    link()->close(fd);
}

void CDebug::on_startTraceButton_clicked()
{
    CNotify notify;
    int pid;
    QString pidDesc;
    QStringList list;

    //create a trace for the PID in the combo box
    if( link()->isConnected() == false ){
        notify.execNotConnected();
        return;
    }

    if( trace_id != 0 ){
        notify.execError("Trace has not been shutdown");
        return;
    }

    pidDesc = ui->pidComboBox->currentText();
    list = pidDesc.split(":");
    pid = list.at(0).toInt();

    if( link()->trace_create(pid, &trace_id) < 0 ){
        notify.execError("Failed to create trace");
        return;
    }

    CNotify::updateStatus(QString("Opened trace id:%1").arg(trace_id, 0, 16));
    traceTimer.setInterval(50);
    traceTimer.start();

    ui->startTraceButton->setEnabled(false);
    ui->stopTraceButton->setEnabled(true);
    ui->pidComboBox->setEnabled(false);
    ui->refreshPidButton->setEnabled(false);

}

void CDebug::on_stopTraceButton_clicked()
{
    CNotify notify;
    traceTimer.stop();
    if( trace_id != 0 ){
        //shutdown the currently active trace
        if( link()->trace_shutdown(trace_id) < 0 ){
            notify.execError("Failed to shutdown trace");
        }
    }
    CNotify::updateStatus(QString("Closed trace id:%1").arg(trace_id, 0, 16));
    trace_id = 0;
    ui->startTraceButton->setEnabled(true);
    ui->stopTraceButton->setEnabled(false);
    ui->pidComboBox->setEnabled(true);
    ui->refreshPidButton->setEnabled(true);
}

void CDebug::traceTimerExpired(void){
    char buffer[1024];
    char * str;
    int ret;
    link_posix_trace_event_info_t * event_info;
    int i;
    QString entry;
    QColor fg, bg;

    //read the trace events (if available)
    if( link()->connected() == false ){
        traceTimer.stop();
        return;
    }

    i = 0;
    do {
        memset(buffer, 0, 1024);
        ret = link()->trace_tryget_events(trace_id, buffer, 1024);
        if( ret == 0 ){
            CNotify::updateStatus("Got zero");
        }
        if( ret > 0 ){
            //format the trace events and add to the trace textEdit area
            event_info = (link_posix_trace_event_info_t *)buffer;
            str = buffer + sizeof(link_posix_trace_event_info_t);
            entry = QString("%1").arg(event_info->posix_timestamp_tv_sec, 0, 10) + "." +
                    QString("%1").arg(event_info->posix_timestamp_tv_nsec/1000, 6, 10, QChar('0')) + ":" +
                    QString("%1").arg(event_info->posix_prog_address, 0, 16) + ":" +
                    QString("%1.%2").arg(event_info->posix_pid).arg(event_info->posix_thread_id) + ":" +
                    QString(link_posix_trace_getname(event_info->posix_event_id)) + ":";



            switch(event_info->posix_event_id){
            case LINK_POSIX_TRACE_MESSAGE:
                fg = Qt::black;
                bg = Qt::white;
                entry += str;
                break;
            case LINK_POSIX_TRACE_WARNING:
                fg = QColor("#8a6d3b");
                bg = QColor("#fcf8e3");
                entry += str;
                break;
            case LINK_POSIX_TRACE_CRITICAL:
                fg = QColor("#a94442");
                bg = QColor("#f2dede");
                entry += str;
                break;
            case LINK_POSIX_TRACE_FATAL:
                fg = QColor("#ffffff");
                bg = QColor("#333333");
                entry += str;
                break;
            default:
                //print hex data
                break;
            }
            //ui->traceTextEdit->appendText(entry + "\n", Qt::black);
            ui->traceListWidget->appendItem(entry, fg, bg);

        } else if( ret < 0 ){
            if( link_errno != 11 ) {  //this is EAGAIN on the device
                traceTimer.stop();
            }
        }
        i++;
    } while( (ret > 0) && (i < 16) );
}

void CDebug::on_traceListWidget_itemClicked(QListWidgetItem *item)
{
    bool ok;
    QFile assembly;
    uint32_t addr;
    QString hexValue;
    int lineNumber;
    int count;
    int startLine;
    int endLine;
    int activeRow;
    QString line;
    QStringList list = item->text().split(":");
    if( list.size() < 4 ){
        return;
    }

    //if the assembly is not visible -- don't load
    if( ui->assemblyListWidget->isVisible() == false ){
        return;
    }

    CNotify::updateStatus("Load " + list.at(1));

    hexValue = list.at(1);
    addr = hexValue.toUInt(&ok, 16);

    if( ok == true ){
        if( (addr & 0xDE000000) == 0xDE000000 ){
            assembly.setFileName(appPath);
        } else {
            assembly.setFileName(kernelPath);
        }

        CNotify::updateStatus("Load " + assembly.fileName());

        if( assembly.open(QFile::ReadOnly | QFile::Text) == false ){
            CNotify::updateStatus(QString("Failed to read " + assembly.fileName()));
            return;
        }


        lineNumber = 0;
        while ( assembly.atEnd() == false ) {
            line = assembly.readLine();
            lineNumber++;
            if( line.startsWith(hexValue + ":") ){
                //load the previous and next 100 lines
                break;
            }

        }


        startLine = lineNumber - 50;
        endLine = lineNumber + 50;
        if( lineNumber < 0 ){
            lineNumber = 0;
        }
        count = 0;
        assembly.reset();
        ui->assemblyListWidget->clear();
        while(count < lineNumber + 200 ){
            line = assembly.readLine();
            count++;
            if( (count < endLine) && (count > startLine) ){
                line.remove(QRegExp("[\\n\\r]"));
                ui->assemblyListWidget->addItem(QString::number(count) + ":" + line);
                if( count == lineNumber ){
                    activeRow = ui->assemblyListWidget->count() - 1;
                }
            }
        }

        ui->assemblyListWidget->item(activeRow)->setBackground(
                    QBrush(QColor("#d9edf7"))
                    );
        ui->assemblyListWidget->item(activeRow)->setForeground(
                    QBrush(QColor("#31708f"))
                    );

        assembly.close();

        if( ui->assemblyListWidget->count() > 0 ){
            //now find and highlight the desired line
            ui->assemblyListWidget->verticalScrollBar()->setValue(
                        activeRow * ui->assemblyListWidget->verticalScrollBar()->maximum() / ui->assemblyListWidget->count()
                        );
        }


    }
}
