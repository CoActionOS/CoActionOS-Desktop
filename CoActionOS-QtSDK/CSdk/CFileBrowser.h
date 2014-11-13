#ifndef CFILEBROWSER_H
#define CFILEBROWSER_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "CLinkPtr.h"

namespace Ui {
class CFileBrowser;
}

class CFileBrowser : public QWidget, public CLinkPtr
{
    Q_OBJECT
    
public:
    explicit CFileBrowser(QWidget *parent = 0);
    ~CFileBrowser();

    void refresh(void);
    void loadDirectory(QTreeWidgetItem * treeItem);
    void setLink(CLink * d);

    static bool copyFileToDevice(CLink * d, const QString & filename, const QString & targetDir);
    static bool copyFilesToDevice(CLink * d, const QStringList & filenames, const QString & targetDir);
    static bool copyDirToDevice(CLink * d, QString srcDir, QString targetDir);

signals:
    void runApplication(QString path);

    
private slots:
    void saveFileToDevice(void);
    void deleteFile(void);
    void createDirectory(void);
    void loadFileFromDevice(void);
    void runApp(void);
    void saveFileToDeviceSelected(QStringList);
    void loadFileFromDeviceSelected(QString);
    void connected(bool value);
    void formatFilesystem(void);

    void on_topButton_clicked();
    void on_refreshButton_clicked();
    void on_deleteButton_clicked();
    void on_uploadButton_clicked();
    void on_downloadButton_clicked();
    void on_gotoLine_editingFinished();

    void on_tree_itemActivated(QTreeWidgetItem *item, int column);

    void on_tree_itemSelectionChanged();

    void on_formatButton_clicked();

    void on_runButton_clicked();

private:
    void resizeEvent(QResizeEvent * event);
    Ui::CFileBrowser *ui;
    QTreeWidgetItem * topItem;
    char decodeType(link_mode_t mode);
};

#endif // CFILEBROWSER_H
