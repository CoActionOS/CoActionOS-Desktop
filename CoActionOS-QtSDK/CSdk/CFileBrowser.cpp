/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "CFileBrowser.h"
#include "ui_CFileBrowser.h"

#include <QFileDialog>
#include <QDir>
#include <QStringList>
#include <hwpl.h>
#include <sys/stat.h>
#include <applib/Appfs.hpp>

#include "CNotify.h"
#include "CFont.h"

static bool updateProgress(void * context, int prog, int max){
    CNotify::updateProgress(prog,max);
    qApp->processEvents();
    return false; //do not abort
}

CFileBrowser::CFileBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFileBrowser)
{
    ui->setupUi(this);

    ui->tree->setColumnHidden(1, true);
    ui->tree->setColumnHidden(2, true);

    topItem = NULL;

    ui->gotoLine->setToolTip("Top level browsing location");
    ui->pathLabel->setToolTip("Currently selected file/path");

    ui->refreshButton->setText(CFont::iconRefresh());
    ui->refreshButton->setObjectName("blueButton");
    ui->refreshButton->setToolTip("Refresh current view");

    ui->topButton->setText(CFont::iconHome());
    ui->topButton->setObjectName("blueButton");
    ui->topButton->setToolTip("Load root directory");

    ui->runButton->setText(CFont::iconPlay());
    ui->runButton->setObjectName("blueButton");
    ui->runButton->setToolTip("Execute");

    ui->downloadButton->setText(CFont::iconDownloadAlt());
    ui->downloadButton->setObjectName("blueButton");
    ui->downloadButton->setToolTip("Copy file to device");

    ui->uploadButton->setText(CFont::iconUploadAlt());
    ui->uploadButton->setObjectName("blueButton");
    ui->uploadButton->setToolTip("Get file from device");

    ui->deleteButton->setText(CFont::iconTrash());
    ui->deleteButton->setObjectName("blueButton");
    ui->deleteButton->setToolTip("Delete file");

    ui->formatButton->setText(CFont::iconHdd());
    ui->formatButton->setObjectName("blueButton");
    ui->formatButton->setToolTip("Format selected mount");

    ui->gotoLine->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->tree->setAttribute(Qt::WA_MacShowFocusRect, 0);

    ui->notConnectedLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Connected");
    ui->notConnectedLabel->setVisible(false);
    ui->notConnectedLabel->setEnabled(false);
    ui->notConnectedLabel->setObjectName("acceptLabel");
    ui->notConnectedWidget->setObjectName("bordered");

    this->connected(false);
}

CFileBrowser::~CFileBrowser()
{
    delete ui;
}

void CFileBrowser::setLink(CLink * d){
    CLinkPtr::setLink(d);
    connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)), Qt::DirectConnection);
}


void CFileBrowser::connected(bool value){

    if( link() != 0 ){
        if( link()->isBootloader() == true ){
            value = false;
        }
    }

    ui->refreshButton->setEnabled(value);
    ui->deleteButton->setEnabled(value);
    ui->uploadButton->setEnabled(value);
    ui->runButton->setEnabled(value);
    ui->downloadButton->setEnabled(value);
    ui->topButton->setEnabled(value);
    ui->notConnectedLabel->setVisible(!value);
    ui->notConnectedWidget->setVisible(!value);
    ui->formatButton->setEnabled(value);
    ui->tree->setVisible(value);
}

#define	LINK_S_IFMT		LINK__IFMT
#define	LINK_S_IFDIR	LINK__IFDIR
#define	LINK_S_IFCHR	LINK__IFCHR
#define	LINK_S_IFBLK	LINK__IFBLK
#define	LINK_S_IFREG	LINK__IFREG
#define	LINK_S_IFLNK	LINK__IFLNK
#define	LINK_S_IFSOCK	LINK__IFSOCK
#define	LINK_S_IFIFO	LINK__IFIFO

char CFileBrowser::decodeType(link_mode_t mode){
    int type;
    type = mode & LINK_S_IFMT;
    switch(type){
    case LINK_S_IFDIR:
        return 'd';
    case LINK_S_IFCHR:
        return 'c';
    case LINK_S_IFBLK:
        return 'b';
    case LINK_S_IFLNK:
        return 'l';
    case LINK_S_IFREG:
        return '-';
    default:
        return 'x';
    }
}


void CFileBrowser::refresh(void){
    //Update the tree
    CNotify notify;

    if ( link()->connected() == false ){
        //Error::showNotConnected();
        return;
    }

    ui->tree->clear();

    topItem = new QTreeWidgetItem();
    if ( topItem == NULL ){
        notify.execError("Top Item is NULL");
        return;
    }

    if ( ui->gotoLine->text() == "" ){
        //Goto to "/" as the default directory
        ui->gotoLine->setText("/");
    }

    topItem->setText(0, ui->gotoLine->text() );
    topItem->setText(1, ui->gotoLine->text() );
    topItem->setText(2, "false" );
    ui->tree->addTopLevelItem(topItem);
    ui->tree->setCurrentItem(topItem, 0);

    loadDirectory(topItem);
}

void CFileBrowser::saveFileToDevice(void){
    //Copy file to the device
    QFileDialog fileDialog;
    connect(&fileDialog,
            SIGNAL(filesSelected(QStringList)),
            this,
            SLOT(saveFileToDeviceSelected(QStringList)), Qt::QueuedConnection);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.exec();
    fileDialog.hide();
}

void CFileBrowser::saveFileToDeviceSelected(QStringList filenames){
    CNotify notify;

    if( copyFilesToDevice(link(), filenames, ui->tree->currentItem()->text(1)) == false ){
        notify.execLinkError(link_errno);
    } else {
        notify.updateStatus("File Saved");
        refresh();
    }
}

bool CFileBrowser::copyFilesToDevice(CLink * d,
                                    const QStringList & filenames,
                                    const QString & targetDir){
    int i;
    for(i=0; i < filenames.size(); i++){
        QString filename(filenames.at(i));
        if( copyFileToDevice(d, filename, targetDir) == false ){
            return false;
        }

    }
    return true;
}


bool CFileBrowser::copyFileToDevice(CLink * d, const QString & filename, const QString & targetDir){
    QList<QString> pathList;
    int err;
    QString target;
    char * buffer;
    int nbyte;
    pathList = filename.split('/');
    target = targetDir + "/" + pathList.back();
    qDebug("Target Dir: %s  Target: %s", targetDir.toStdString().c_str(), target.toStdString().c_str());
    if( (targetDir == "/app") ||
            (targetDir == "/app/flash") ||
            (target == "/app/.install") ){
        QFile f(filename);
        target = "/app/.install";
        f.open(QIODevice::ReadOnly);
        nbyte = f.size();
        buffer = (char*)malloc(nbyte);
        if( buffer == 0 ){
            return false;
        }
        f.read(buffer, nbyte);
        f.close();
        qDebug("Appfs create");
        err = Appfs::create(pathList.back().toStdString().c_str(),
                            buffer,
                            nbyte,
                            "/app",
                            d->handle());
        qDebug("Appfs returned %d", err);
        free(buffer);
    } else {
        qDebug("Copy file");
        err = d->cp(filename.toStdString(), target.toStdString(), 0777, true, updateProgress, 0);
    }
    if ( err < 0 ){
        qDebug("Err is %d %d", err, link_errno);
        //notify.execLinkError(link_errno);
        return false;
    }

    return true;
}

bool CFileBrowser::copyDirToDevice(CLink * d, QString srcDir, QString targetDir){
    QDir dir(srcDir);
    CNotify notify;
    QStringList files;
    bool ret;
    int i;
    files = dir.entryList(QDir::Files);
    for(i=0; i < files.size(); i++){
        ret = copyFileToDevice(d,
                             dir.path() + "/" + files.at(i),
                             targetDir
                             );
        if( ret < 0 ){
            return false;
        }
        notify.updateStatus("Copied " + files.at(i));
    }
    return true;
}



void CFileBrowser::loadFileFromDevice(){
    //Load a file from the device
    QFileDialog fileDialog;
    connect(&fileDialog,
            SIGNAL(fileSelected(QString)),
            this,
            SLOT(loadFileFromDeviceSelected(QString)), Qt::QueuedConnection);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    fileDialog.setOptions(QFileDialog::ShowDirsOnly);
    fileDialog.exec();
}

void CFileBrowser::loadFileFromDeviceSelected(QString filename){
    QString src, dest;
    QStringList path;
    int err;
    CNotify notify;

    src = ui->tree->currentItem()->text(1);
    path = src.split('/');

    if ( path.count() ){
        dest = filename + "/" + path[ path.count() -1 ];
        err = link()->cp(src.toStdString(), dest.toStdString(), 0, false, updateProgress);
        if ( err < 0 ){
            notify.execLinkError(link_errno);
        }
    } else {
        notify.execError("Invalid File");
    }

    CNotify::updateStatus("Uploaded to:  " + dest);

}

void CFileBrowser::formatFilesystem(void){
    QString filename;
    struct link_stat st;
    int err;
    int fmt;
    QTreeWidgetItem* item;
    CNotify notify;
    item = ui->tree->currentItem();
    filename = item->text(1);

    //check to see if this is a directory or a file
    err = link()->stat( item->text(1).toLocal8Bit().constData(),
                        &st);
    if ( err < 0 ){
        qDebug("Stat failed");
        notify.execLinkError(link_errno);
        return;
    }

    fmt = st.st_mode & LINK_S_IFMT;
    switch(fmt){
    case LINK_S_IFDIR:
        err = link()->format(filename.toLocal8Bit().constData());
        break;
    default:
        qDebug("Unknown file type");
        break;
    }
    if ( err ){
        notify.execLinkError(link_errno);
    }
    this->refresh();
    CNotify::updateStatus("Formatted " + filename);
}


void CFileBrowser::deleteFile(void){
    QString filename;
    struct link_stat st;
    int err;
    int fmt;
    QTreeWidgetItem* item;
    CNotify notify;
    item = ui->tree->currentItem();
    filename = item->text(1);

    //check to see if this is a directory or a file
    err = link()->stat( item->text(1).toLocal8Bit().constData(),
                        &st);
    if ( err < 0 ){
        qDebug("Stat failed");
        notify.execLinkError(link_errno);
        return;
    }

    fmt = st.st_mode & LINK_S_IFMT;
    switch(fmt){
    case LINK_S_IFDIR:
        err = link()->rmdir(filename.toStdString());
        break;
    case LINK_S_IFREG:
        qDebug("Remove file");
        err = link()->unlink(filename.toStdString());
        break;
    default:
        qDebug("Unknown file type");
        break;
    }
    if ( err ){
        qDebug("Remove file failed");
        notify.execLinkError(link_errno);
    }
    this->refresh();
    CNotify::updateStatus("Deleted " + filename);

}

void CFileBrowser::createDirectory(void){

}

void CFileBrowser::on_gotoLine_editingFinished(){

    if ( topItem ){
        if ( ui->gotoLine->text() ==
             topItem->text(0) ){
            return;
        }
    }

    this->refresh();

}

void CFileBrowser::on_tree_itemActivated(QTreeWidgetItem* item, int column){
    CNotify notify;
    qDebug("item activated %s %s", item->text(0).toLocal8Bit().constData(), item->text(1).toLocal8Bit().constData());
    if ( column != 0 ){
        notify.execError("Invalid Column");
        return;
    }
    loadDirectory(item);
    item->setExpanded(true);
}

void CFileBrowser::loadDirectory(QTreeWidgetItem * treeItem){
    int dirp;
    int err;
    QString dName;
    QTreeWidgetItem * child;
    QString currentDirectory;
    struct link_dirent entry;
    CNotify notify;

    //First read the current directory or root if no directory has been selected
    currentDirectory = treeItem->text(1);
    qDebug("loadDirectory:%s", currentDirectory.toLocal8Bit().constData());

    if ( ( treeItem->text(0) == "." ) ||
         ( treeItem->text(0) == ".." ) ){
        //These are aliases to the current and parent directory and should not expanded
        return;
    }

    //Check to see if the directory has already been loaded
    if ( treeItem->text(2) == "false" ){

        //mark the directory as having already been loaded
        treeItem->setText(2, "true");
        qDebug("Opendir %s", currentDirectory.toLocal8Bit().constData());
        dirp = link()->opendir(currentDirectory.toLocal8Bit().constData());

        qDebug("Open dir:%s at 0x%X", currentDirectory.toLocal8Bit().constData(), dirp );

        if( dirp > 0 ){
            while( link()->readdir_r(dirp, &entry, NULL) == 0 ){

                dName = QString(entry.d_name);

                if ( dName != "" ){
                    if ( (dName != ".") &&
                         (dName != "..") ){

                        child = new QTreeWidgetItem();

                        child->setText(0, dName);
                        if ( currentDirectory == "/" ){
                            child->setText(1, currentDirectory + dName);
                        } else {
                            child->setText(1, currentDirectory + "/" + dName);
                        }

                        child->setText(2, "false"); //Mark the new directory as having not been loaded
                        if ( ui->showHiddenFilesCheckBox->checkState() == Qt::Checked ){
                            treeItem->addChild(child);
                        } else if ( !dName.startsWith(".") ){
                            treeItem->addChild(child);
                        }
                    }
                }
            }
            err = link()->closedir(dirp);
            if ( err < 0 ){
                notify.execLinkError(link_errno);
            }
        }

    } else {
        qDebug("Item %s already loaded", treeItem->text(2).toLocal8Bit().constData());
    }
}

void CFileBrowser::runApp(void){
    struct link_stat st;
    int err;
    CNotify notify;
    QTreeWidgetItem* item;
    item = ui->tree->currentItem();
    if( item == 0 ){
        notify.execError("Invalid Selection");
        return;
    }

    err = link()->stat( item->text(1).toLocal8Bit().constData(),
                        &st);

    if ( err < 0 ){
        notify.execLinkError(link_errno);
        return;
    }

    if ( (st.st_mode & LINK_S_IFMT) != LINK_S_IFREG ){
        notify.execError("File is not executable");
        return;
    }

    if( st.st_mode & (LINK_S_IXGRP|LINK_S_IXOTH|LINK_S_IXUSR) ){
        //Execute a program on the target device
        emit runApplication( ui->tree->currentItem()->text(1) );
    } else {
        notify.execError("File is not executable");
    }
}


void CFileBrowser::on_refreshButton_clicked()
{
    this->refresh();
}

void CFileBrowser::on_topButton_clicked()
{
    ui->gotoLine->setText("/");
    refresh();
}

void CFileBrowser::on_tree_itemSelectionChanged()
{
    QTreeWidgetItem * item;
    int column;
    struct link_stat st;
    int err;
    QString fileStats;

    item = ui->tree->currentItem();
    column = 0;

    err = link()->stat( item->text(1).toLocal8Bit().constData(),
                        &st);

    fileStats.clear();
    if ( err >= 0 ){
        fileStats = QString(decodeType(st.st_mode));
        if ( st.st_mode & LINK_S_IROTH ){ fileStats += 'r'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IWOTH ){ fileStats += 'w'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IXOTH ){ fileStats += 'x'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IRGRP ){ fileStats += 'r'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IWGRP ){ fileStats += 'w'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IXGRP ){ fileStats += 'x'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IRUSR ){ fileStats += 'r'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IWUSR ){ fileStats += 'w'; } else { fileStats += '-'; }
        if ( st.st_mode & LINK_S_IXUSR ){ fileStats += 'x'; } else { fileStats += '-'; }
        if ( (st.st_mode & LINK_S_IFMT) == LINK_S_IFREG){
            fileStats += "\t" + QString::number(st.st_size, 10);
        }
    } else {
        fileStats = "Unable to load";
    }

    ui->pathLabel->setText( fileStats + "\t" + item->text(1) );
    ui->tree->setCurrentItem(item, column);

    if ( (st.st_mode & LINK_S_IFMT) == LINK_S_IFDIR){
        on_tree_itemActivated(item, column);
    }
}

void CFileBrowser::on_downloadButton_clicked(){
    CNotify notify;
    if( topItem != 0 ){
        this->saveFileToDevice();
    } else {
        notify.execInfo("No Target Folder Selected");
    }
}
void CFileBrowser::on_uploadButton_clicked(){
    CNotify notify;
    if( topItem != 0 ){
        this->loadFileFromDevice();
    } else {
        notify.execInfo("No Source File Selected");
    }
}
void CFileBrowser::on_deleteButton_clicked(){
    CNotify notify;
    if( topItem != 0 ){
        this->deleteFile();
    } else {
        notify.execInfo("No File Selected");
    }
}
void CFileBrowser::on_formatButton_clicked()
{
    QString filename;
    QTreeWidgetItem* item;

    CNotify notify;
    if( topItem != 0 ){
        item = ui->tree->currentItem();
        filename = item->text(1);
        qDebug("format %s", filename.toLocal8Bit().constData());
        link()->format(filename.toStdString());
        this->refresh();
    } else {
        notify.execInfo("No Mount Selected to Format");
    }
}

void CFileBrowser::on_runButton_clicked(){ runApp(); }


void CFileBrowser::resizeEvent(QResizeEvent * event){
    QSize s = size();
    if( s.width() < 450 ){
        ui->showHiddenFilesCheckBox->hide();
    } else {
        ui->showHiddenFilesCheckBox->show();
    }
    event = 0;
}
