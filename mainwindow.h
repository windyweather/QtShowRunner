#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QStatusBar>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setStatus( QString txt )
    { statusBar()->showMessage(txt);
    }

private slots:
    void on_actionQuit_triggered();

    void on_pb_BrowseShowPath_clicked();

    void on_pb_AddShow_clicked();

    void on_pb_RemoveShow_clicked();

    void on_pb_StartShows_clicked();

    void on_pb_StopShow_clicked();

    void on_pb_BrowseImpressPath_clicked();

    void on_pb_RemoveAllShows_clicked();

    void on_actionAbout_triggered();

    void on_actionHelp_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;

    QSettings*  m_pSettings;        // pointer to a settings file


    // save and restore settings and shows
    void    createDefaults();
    void    saveDefaults();         // save default settings
    void    restoreDefaults();      // restore defaults
    void    saveShow();             // save a show file
    void    restoreShow();          // restore a show file


    void clickingToEndShow();       // start waiting on end of show
    void clickToEnd();              // Just click mouse eventually ending show
    void endOfShow();               // we have seen end of a show
    void startNextShow();           // time to start the next show
    void mouseClick();              // just click the mouse

    bool    showsBusy;              // we are busy showing
    int     showIndex;              // index of show we are running
    bool    showStopper;            // true for manual stopping of shows
    bool    endOfShowSeen;          // have we seen end of current slide show?
    bool    stopClicking;           // true for stop clicking
    QProcess theShow;               // the running slide show
    qint64  showPid;                // pid of show for startDetached
};

#endif // MAINWINDOW_H
