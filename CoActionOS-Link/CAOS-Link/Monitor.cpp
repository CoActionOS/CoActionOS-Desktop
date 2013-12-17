#include "Monitor.h"
#include "ui_Monitor.h"

#include <QList>
#include <QTableWidgetItem>

#include <dev/sys.h>

#include "caoslink.h"
#include <CSdk/CFont.h>
#include <CSdk/CNotify.h>


#define UPDATE_IN_MS (500)

enum {
  NAME_COL,
  PROCESS_ID_COL,
  THREAD_ID_COL,
  PRIO_COL,
  CPU_COL,
  MEM_COL,
  TASK_TIME_COL,
  DELTA_TASK_TIME_COL,
  STACK_HEAP_COL
};

Monitor::Monitor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Monitor)
{
  qDebug("Monitor Init");
  ui->setupUi(this);
  //Initialize the table
  ui->table->setColumnHidden(TASK_TIME_COL, true);
  ui->table->setColumnHidden(DELTA_TASK_TIME_COL, true);
  ui->table->setColumnWidth(STACK_HEAP_COL, 170);
  ui->table->setColumnWidth(PROCESS_ID_COL, 150);
  ui->table->setColumnWidth(THREAD_ID_COL, 145);
  ui->table->setColumnWidth(PRIO_COL, 145);
  ui->table->setColumnWidth(CPU_COL, 145);
  connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(updateAll()));
  ui->killButton->setToolTip("Kill Selected User Process");
  sortColumn = PROCESS_ID_COL;


  sortAscendingAction = new QAction( "Sort Ascending", ui->table );
  sortAscendingAction->setCheckable(true);
  sortAscendingAction->setChecked(true);
  sortOrder = Qt::AscendingOrder;

  sortDescendingAction = new QAction( "Sort Descending", ui->table );
  sortDescendingAction->setCheckable(true);
  sortDescendingAction->setChecked(false);

  QAction * separator = new QAction( "", ui->table );
  separator->setSeparator(true);

  sortByProcessIdAction = new QAction( "Sort By Process ID", ui->table );
  sortByProcessIdAction->setCheckable(true);
  sortByProcessIdAction->setChecked(true);
  sortColumn = PROCESS_ID_COL;

  sortByNameAction = new QAction( "Sort By Name", ui->table );
  sortByNameAction->setCheckable(true);


  sortByCpuUsageAction = new QAction( "Sort By CPU Usage", ui->table );
  sortByCpuUsageAction->setCheckable(true);


  sortByMemoryUsageAction = new QAction( "Sort By Allocated Memory", ui->table );
  sortByMemoryUsageAction->setCheckable(true);

  connect( sortAscendingAction, SIGNAL( changed() ), this, SLOT( sortAscending() ) );
  connect( sortDescendingAction, SIGNAL( changed() ), this, SLOT( sortDescending() ) );
  connect( sortByProcessIdAction, SIGNAL( changed() ), this, SLOT( sortByProcessId() ) );
  connect( sortByNameAction, SIGNAL( changed() ), this, SLOT( sortByName() ) );
  connect( sortByCpuUsageAction, SIGNAL( changed() ), this, SLOT( sortByCpuUsage() ) );
  connect( sortByMemoryUsageAction, SIGNAL( changed() ), this, SLOT( sortByMemoryUsage() ) );

  /*
  ui->table->setContextMenuPolicy( Qt::ActionsContextMenu );
  ui->table->addAction ( sortAscendingAction );
  ui->table->addAction ( sortDescendingAction );
  ui->table->addAction ( separator );
  ui->table->addAction ( sortByProcessIdAction );
  ui->table->addAction ( sortByNameAction );
  ui->table->addAction ( sortByCpuUsageAction );
  ui->table->addAction ( sortByMemoryUsageAction );
  */

  ui->notConnectedWidget->setObjectName("bordered");
  ui->killButton->setObjectName("blueButton");
  ui->killButton->setText(CFont::iconRemove());
  ui->killButton->setToolTip("Kill process");
  ui->continueButton->setObjectName("blueButton");
  ui->continueButton->setText(CFont::iconPlay());
  ui->continueButton->setToolTip("Continue process");
  ui->pauseButton->setObjectName("blueButton");
  ui->pauseButton->setText(CFont::iconPause());
  ui->pauseButton->setToolTip("Pause process");
  ui->termButton->setObjectName("blueButton");
  ui->termButton->setText(CFont::iconStop());
  ui->termButton->setToolTip("Terminate process");
  ui->notConnectedLabel->setText(CFont::icomoon(CFont::iconInfoSign()) + " Not Connected");
  ui->notConnectedLabel->setObjectName("acceptLabel");
  ui->notConnectedLabel->setEnabled(false);
  ui->notConnectedWidget->setVisible(false);

  qDebug("Monitor Init Complete");

}

Monitor::~Monitor()
{
  delete ui;
}

void Monitor::setLink(CLink * d){
  CLinkPtr::setLink(d);
  connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)), Qt::DirectConnection);
  this->connected(false);
}


void Monitor::connected(bool value){
  ui->killButton->setEnabled(value);
  ui->continueButton->setEnabled(value);
  ui->pauseButton->setEnabled(value);
  ui->termButton->setEnabled(value);
  ui->notConnectedWidget->setVisible(!value);
  ui->table->setVisible(value);


  if( value == true ){
      if( fd < 0 ){
          fd = link()->open("/dev/sys", LINK_O_RDWR);
          if( fd < 0 ){
              fd = -1;
            }
        }
    } else {
      if( fd != -1 ){
          if( link()->isConnected() == true ){
              link()->close(fd);
            }
          fd = -1;
        }

      stopTimer();
    }
}

void Monitor::updateItem(int row, int column, QString value){
  QTableWidgetItem * item;
  item = ui->table->item(row, column);
  if ( item == NULL ){
      item = new QTableWidgetItem();
    }
  item->setText( value );
  item->setTextAlignment(Qt::AlignCenter);

  if ( ui->table->item(row, column) == NULL ){
      ui->table->setItem(row, column, item);
    }
}


void Monitor::updateRow(int tid, void * data){
  int i;
  int row;
  QTableWidgetItem * item;
  int64_t oldTimer;
  link_task_data_t * task = (link_task_data_t*)data;
  int stackUsage;
  int heapUsage;

  row = -1;
  //find the tid if it exists in the table
  for(i=0; i < ui->table->rowCount(); i++){
      item = ui->table->item(i, THREAD_ID_COL);
      if ( item != NULL ){
          if ( tid == item->text().toInt() ){
              row = i;
              break;
            }
        } else {
          row = i;
          break;
        }
    }

  if ( task->is_enabled ){
      if ( row == -1 ){
          row = ui->table->rowCount();
          ui->table->insertRow(ui->table->rowCount());
        }
    } else {
      if ( row != -1 ){
          ui->table->removeRow(row);
        }
      return;
    }

  updateItem(row, NAME_COL, task->name);

  if ( task->is_thread ){
      updateItem(row, PROCESS_ID_COL, QString::number(task->pid, 10) + " (thread)");
    } else {
      updateItem(row, PROCESS_ID_COL, QString::number(task->pid, 10));
    }
  if ( task->tid == 0 ){
      updateItem(row, THREAD_ID_COL, QString::number(task->tid, 10) + " (idle)");
      updateItem(row, PRIO_COL, "n/a");
    } else {
      updateItem(row, THREAD_ID_COL, QString::number(task->tid, 10));
      updateItem(row, PRIO_COL,
                 QString::number(task->prio_ceiling, 10) + " (" +
                 QString::number(task->prio, 10) + ")");
    }



  stackUsage = (task->mem_loc + task->mem_size) - task->stack_ptr;
  if ( task->malloc_loc != 0 ){
      heapUsage = task->malloc_loc - task->mem_loc;
    } else {
      heapUsage = 0;
    }
  updateItem(row, MEM_COL,  QString::number( 100.0 * (stackUsage + heapUsage) / ( task->mem_size)));

  updateItem(row, STACK_HEAP_COL, QString::number(task->mem_size) + " ("
             + QString::number(stackUsage, 10) + "/"
             + QString::number(heapUsage, 10) + ")");

  item = ui->table->item(row, TASK_TIME_COL);
  if ( item != NULL ){
      oldTimer = item->text().toLongLong();
    } else {
      item = NULL;
      oldTimer = 0;
    }

  updateItem(row, TASK_TIME_COL, QString::number(task->timer, 10));
  updateItem(row, DELTA_TASK_TIME_COL, QString::number(task->timer - oldTimer, 10));
}

bool Monitor::isRunningUser(void){
  sys_taskattr_t task;
  int id;
  int err;
  CNotify notify;


  if( fd < 0 ){
      if ( link()->isConnected() == true ){
          fd = link()->open("/dev/sys", LINK_O_RDWR);
        }
    }
  id = 0;
  do {
      if ( link()->isConnected() == false ){
          return -1;
        }

      task.is_enabled = 0;
      task.tid = id;
      err = link()->ioctl(fd, I_SYS_GETTASK, &task);
      if ( err < -2 ){
          notify.execError(errorMessage() );
          return -1;
        }

      if( task.is_enabled != 0 ){
          if( QString(task.name) != "sys" ){
              return true;
            }
        }

      id++;
    } while( err != -1 );

  return false;
}

void Monitor::updateAll(){
  int id;
  int err;
  int row;
  int cpuRow;
  uint64_t totalTime;
  uint64_t taskTime;
  sys_taskattr_t task;
  CNotify notify;
  id = 0;

  if( link()->isConnected() == false ){
      return;
    }

  if( fd < 0 ){
      return;
    }

  do {
      if ( link()->isConnected() == false ){
          stopTimer();
          for(row = 0; row < ui->table->rowCount(); row++){
              ui->table->removeRow(row);
            }
          return;
        }

      task.is_enabled = 0;
      task.tid = id;
      err = link()->ioctl(fd, I_SYS_GETTASK, &task);
      if ( err < -2 ){
          stopTimer();
          for(row = 0; row < ui->table->rowCount(); row++){
              ui->table->removeRow(row);
            }
          notify.execError( errorMessage() );
          return;
        }

      //update the entries in the table
      updateRow(id, &task);
      id++;
    } while( err != -1 );


  cpuRow = ui->table->rowCount();
  totalTime = 0;
  for(row = 0; row < cpuRow; row++){
      totalTime += ui->table->item(row, DELTA_TASK_TIME_COL)->text().toLongLong();
    }


  for(row = 0; row < cpuRow; row++){
      taskTime = ui->table->item(row, DELTA_TASK_TIME_COL)->text().toLongLong();
      updateItem(row, CPU_COL, QString::number( taskTime * 100.0 / totalTime, 'f', 2 ));
    }

  ui->table->sortItems(sortColumn, sortOrder);


}

void Monitor::startTimer(void){
  refreshTimer.start(UPDATE_IN_MS);
}

void Monitor::stopTimer(void){
  refreshTimer.stop();

}

void Monitor::uncheckAll(void){
  sortByProcessIdAction->setChecked(false);
  sortByNameAction->setChecked(false);
  sortByCpuUsageAction->setChecked(false);
  sortByMemoryUsageAction->setChecked(false);
}


void Monitor::sortByProcessId(void){
  sortColumn = PROCESS_ID_COL;
  uncheckAll();
  sortByProcessIdAction->setChecked(true);
}

void Monitor::sortByName(void){
  sortColumn = NAME_COL;
  uncheckAll();
  sortByNameAction->setChecked(true);
}

void Monitor::sortByCpuUsage(void){
  sortColumn = CPU_COL;
  uncheckAll();
  sortByCpuUsageAction->setChecked(true);
}

void Monitor::sortByMemoryUsage(void){
  sortColumn = STACK_HEAP_COL;
  uncheckAll();
  if( sortByMemoryUsageAction != 0 ){
    sortByMemoryUsageAction->setChecked(true);
    }
}

void Monitor::sortAscending(void){
  sortOrder = Qt::AscendingOrder;
  if( (sortAscendingAction != 0) && (sortDescendingAction != 0) ){
      sortAscendingAction->setChecked(true);
      sortDescendingAction->setChecked(false);
    }
}

void Monitor::sortDescending(void){
  sortOrder = Qt::DescendingOrder;
  if( (sortAscendingAction != 0) && (sortDescendingAction != 0) ){
      sortAscendingAction->setChecked(false);
      sortDescendingAction->setChecked(true);
    }
}

void Monitor::on_killButton_clicked()
{
  signalProcess(LINK_SIGKILL);
}

void Monitor::on_pauseButton_clicked()
{
  signalProcess(LINK_SIGSTOP);
}

void Monitor::on_continueButton_clicked()
{
  signalProcess(LINK_SIGCONT);
}

void Monitor::on_termButton_clicked()
{
  signalProcess(LINK_SIGTERM);
}

int Monitor::signalProcess(int signo){
  int row;
  int pid;
  int err;
  QTableWidgetItem * item;
  CNotify notify;

  if ( link()->isConnected() == false ){
      notify.execNotConnected();
      return -1;
    }

  row = ui->table->currentRow();
  if ( row != 0 ){
      item = ui->table->item(row, PROCESS_ID_COL);
      if ( item == NULL ){
          return -1;
        }

      pid = item->text().replace(" (thread)", "").toInt();
      if( pid != 0 ){
          if ( (err = link()->killPid(pid, signo)) < 0 ){
              notify.execError(errorMessage());
            } else {
              ui->table->removeRow(row);
            }
        }
    }
  return 0;
}
