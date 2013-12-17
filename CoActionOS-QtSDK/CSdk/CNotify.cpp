#include "CNotify.h"
#include "ui_CNotify.h"
#include "CFont.h"

QStatusBar * CNotify::statusBar = 0;
QProgressBar * CNotify::progressBar = 0;
QStringList CNotify::messages = QStringList();
QTimer * CNotify::msgTimer = 0;
CNotify * CNotify::notifyPtr = 0;

CNotify::CNotify(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CNotify)
{
  ui->setupUi(this);
  ui->okButton->setObjectName("greenButton");
  ui->okButton->setText(CFont::iconOk());
  ui->cancelButton->setObjectName("redButton");
  ui->cancelButton->setText(CFont::iconRemove());

  ui->infoLabel->setText(CFont::icomoon(CFont::iconInfoSign()) +
                         " Notice");
  ui->infoLabel->setObjectName("alertLabel");
  ui->warningLabel->setText(CFont::icomoon(CFont::iconWarningSign()) +
                            " Warning");
  ui->warningLabel->setObjectName("alertWarningLabel");
  ui->errorLabel->setText(CFont::icomoon(CFont::iconExclamationSign()) +
                          " Error");
  ui->errorLabel->setObjectName("alertErrorLabel");
  ui->questionLabel->setText(CFont::icomoon(CFont::iconQuestionSign()) +
                             " Question");
  ui->questionLabel->setObjectName("alertQuestionLabel");

  ui->infoLabel->setVisible(false);
  ui->warningLabel->setVisible(false);
  ui->errorLabel->setVisible(false);
  ui->questionLabel->setVisible(false);
}

CNotify::~CNotify()
{
  delete ui;
}

void CNotify::msgTimeout(){
  QStringList newList = QStringList();
  int i;
  for(i=1; i < messages.count(); i++){
      newList.append(messages.at(i));
    }

  if( messages.count() > 0 ){
      if( statusBar != 0 ){
          statusBar->showMessage("      " + newList.join(", "));
        }
      msgTimer->setInterval(1000);
      msgTimer->start();
    }

  messages = newList;
}

void CNotify::setUpdateObjects(QStatusBar * statusBar, QProgressBar * progressBar){
  CNotify::statusBar = statusBar;
  CNotify::progressBar = progressBar;
}

void CNotify::updateStatus(QString status){
  QStringList newList;
  int i;
  if( msgTimer == 0 ){
      notifyPtr = new CNotify();
      msgTimer = new QTimer();
      connect(msgTimer, SIGNAL(timeout()), notifyPtr, SLOT(msgTimeout()));
    }

  if( statusBar != 0 ){
      if ( status != "" ){
          messages.append(status);
          if( messages.count() > 4 ){
              for(i=1; i < messages.count(); i++){ newList.append(messages.at(i)); }
              messages = newList;
            }
          statusBar->showMessage("      " + messages.join(", "));
          msgTimer->setInterval(2000);
          msgTimer->start();
        }
      if( progressBar != 0) {
          progressBar->setVisible(false);
        }
    }
}

void CNotify::updateProgress(int progress, int max, bool showprogress){
  if( progressBar != 0 ){
      progressBar->setVisible(showprogress);
      progressBar->setMaximum(max);
      progressBar->setValue(progress);
      qApp->processEvents();
    }
}

void CNotify::execWarning(QString msg){
  setWindowTitle("Warning");
  ui->messageLabel->setText(msg);
  ui->cancelButton->hide();
  QDialog::exec();
}

void CNotify::setHeight(QString msg){
  QString style;
  int height;
  height = (msg.count('\n') + 1) * 30;
  style = "padding 0 0 0 0; max-height: " + QString::number(height) + "px; min-height: " + QString::number(height) + "px;";
  ui->messageLabel->setStyleSheet(style);
}

void CNotify::execInfo(QString msg){
  setWindowTitle("Info");
  setHeight(msg);
  ui->infoLabel->setVisible(true);
  ui->messageLabel->setText(msg);
  ui->cancelButton->hide();
  QDialog::exec();
}

void CNotify::execError(QString msg){
  setWindowTitle("Error");
  setHeight(msg);
  ui->errorLabel->setVisible(true);
  ui->messageLabel->setText(msg);
  ui->cancelButton->hide();
  updateProgress(0,0,false);
  QDialog::exec();
}

void CNotify::execNotConnected(){
  execError("Device is Not Connected");
}

int CNotify::execPrompt(QString msg){
  setWindowTitle("Question");
  setHeight(msg);
  ui->questionLabel->setVisible(true);
  ui->messageLabel->setText(msg);
  return QDialog::exec();
}

void CNotify::execLinkError(int number){
  switch(number){
    case 1:
      execError("Not super-user (Not owner)");
      break;
    case 2:
      execError("No such file or directory");
      break;
    case 3:
      execError("No such process");
      break;
    case 4:
      execError("Interrupted system call");
      break;
    case 5:
      execError("IO error");
      break;
    case 6:
      execError("No such device or address");
      break;
    case 7:
      execError("Argument list too long");
      break;
    case 8:
      execError("Bad executable format (ensure binary is valid and was built\nusing a compatible version)");
      break;
    case 9:
      execError("Bad file number");
      break;
    case 10:
      execError("No childer");
      break;
    case 11:
      execError("Resource temporarily unavailable");
      break;
    case 12:
      execError("No more memory");
      break;
    case 13:
      execError("Permission denied");
      break;
    case 14:
      execError("Bad address");
      break;
    case 15:
      execError("Block device required");
      break;
    case 16:
      execError("Mount device busy");
      break;
    case 17:
      execError("File exists");
      break;
    case 18:
      execError("Cross-device link");
      break;
    case 19:
      execError("No such device");
      break;
    case 20:
      execError("Not a directory");
      break;
    case 21:
      execError("Permission denied");
      break;
    case 22:
      execError("Invalid argument");
      break;
    case 23:
      execError("Too many open files on device");
      break;
    case 24:
      execError("Too many open files");
      break;
    case 25:
      execError("Not a character device");
      break;
    case 26:
      execError("Text file busy");
      break;
    case 27:
      execError("File too large");
      break;
    case 28:
      execError("No space left on device");
      break;
    case 29:
      execError("Illegal seek");
      break;
    case 30:
      execError("Read-only filesystem");
      break;
    case 31:
      execError("Too many links");
      break;
    case 32:
      execError("Broken pipe");
      break;
    case 57:
      execError("Bad font file format");
      break;
    case 61:
      execError("No data (for no delay IO)");
      break;
    case 62:
      execError("Timer expired");
      break;
    case 64:
      execError("Machine is not on the network");
      break;
    case 65:
      execError("Package not installed");
      break;
    case 66:
      execError("The object is remote");
      break;
    case 67:
      execError("The link has been severed");
      break;
    case 68:
      execError("Advertise error");
      break;
    case 69:
      execError("Srmount error");
      break;
    case 70:
      execError("Communication error on send");
      break;
    case 71:
      execError("Protocol error");
      break;
    case 74:
      execError("Multihop attempted");
      break;
    case 75:
      execError("Inode is remote (not really error)");
      break;
    case 88:
      execError("Function is not implemented");
      break;
    case 89:
      execError("No more files");
      break;
    case 90:
      execError("Directory not empty");
      break;
    case 91:
      execError("File or path name too long");
      break;
    case 92:
      execError("Too many symbolic links");
      break;
    case 109:
      execError("Protocol not available");
      break;
    case 134:
      execError("This operation is not supported");
      break;
    default:
      execError("Failed with error code " + QString::number(number));
    }

}

void CNotify::on_okButton_clicked(){ accept();  }
void CNotify::on_cancelButton_clicked(){ reject(); }
