/*
 *=BEGIN GPL
 *
 * This file is part of the QtShowRunner program.
 *
 * Copyright(c) 2020 Darrell Duffy
 * http://www.windyweather.net/
 *
 * This file may be licensed under the terms of of the
 * GNU General Public License Version 3 (the ``GPL'').
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the GPL for the specific language
 * governing rights and limitations.
 *
 * For a copy of the GPL please visit https://www.gnu.org/licenses/gpl-3.0.en.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *=END GPL
 */

/*
 * QtShowRunner is a program to chain together multiple LibreOffice impress slide shows.
 * The problem being solved is that when slide shows are displayed, they stop at the end
 * with a "mostly" black screen and require user interaction to exit.
 * So, in order to "chain" together multiple shows, and loop them around, something external
 * is required to sense that a show has ended so the next one can be started.
 *
 * ScreenShot Looking for Black Attempt
 *
 * A ScreenShot approach was tried to watch for the black end screen and terminating the
 * impress program. This worked on a Windows 10 system with an Nvidia 2060 card, but failed
 * on a Gbrix Intel CPU with integrated graphics. The program worked as follows:
 * QtShowRunner watches the screen by taking periodic screenshots and looking at the right
 * 80% of the screen to see if it is totally black. When a slide show ends and the
 * message is displayed at the top the left 20% or so of the screen is
 * apparently remains as the background of the master slide. The screenshot does not capture
 * the message. Apparently this is displayed as an overlay window that is invisible to the
 * screenshot.
 *
 * Also, during the display of the slideshow, the screen apparently flashes black, which
 * causes single screen shots to erroneously detect end of show, so, the screen is watched
 * repeatedly over about 5 seconds time to see if it remains black and then the show is
 * "terminated" or in severe cases "killed" and the next show is then displayed.
 * The process tree of Impress is so complicated that QProcess::terminate or QProcess::kill
 * does not work. Killing the parent process - soffice.exe - is a bad idea since that
 * causes impress to want to "recover" the file it was just displaying.
 * This screenshot attempt has been abandended.
 *
 * Mouse Click Solution
 *
 * The solution that does work is sending a key to the top window on the screen, which
 * triggers the end of show and causes impress and soffice to cleanly exit.
 * The QtShowRunner program just clicks the mouse every few seconds during the entire show.
 * This requires that the show settings disable "Click Mouse to Advance Slide".
 * This solution works on both the Windows 10 Nvidia and Windows 10 Intel Integrated Graphics
 * systems.
 *
 * This complicated process gets around the fact that LibreOffice Impress apparently has no
 * options or scripting features available to accomplish chaining and looping of multiple
 * slide shows together.
 *
 * The reason for wanting to chain multiple shows is that after 100 or so slides with images
 * the save times and editing for shows becomes cumbersome. This is quite reasonable for at this
 * point the show file size may become 100MB or more.
 *
 * Thanks to the many and several folks who provided the very nice Qt Development environment and
 * Qt example code on which this program is based. And note that this program is Open Source and
 * GPL so feel free to use these ideas to solve your own problems and pass along the ideas.
 *
 * enjoy.
 * April 2020
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <aboutdialog.h>
#include <helpdialog.h>

#include <QFileDialog>
#include <QListWidgetItem>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QTextStream>
#include <QMessageBox>
#include <QModelIndexList>
#include <QWindow>
#include <QScreen>
#include <QDir>
#include <QDebug>

// support mouse click
#include <mouseclick.h>

//
// Slideshows are stopped by just clicking the mouse every few seconds
// during the whole show. And the show must be set to not advance
// on mouse clicks. Click this often. Allow some time to grab mouse
// to stop the shows inbetween clicks.
//
#define WAIT_SECONDS 5      // seconds to wait between mouse clicks

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showsBusy = false;      // no shows running
    showIndex = 0;          // do we have shows in list?
    showStopper = false;    // not stopping shows

    if (!mouseopen() )
    {
        setStatus(tr("Mouse Not Opened"));
    }

#ifdef Q_OS_LINUX
    ui->le_ImpressPath->setText(tr("libreoffice"));
    ui->le_ShowOption->setText(tr("--impress --show"));
    ui->le_ShowPath->setText(tr("/home/darrell/ImpressTests"));
#endif // Q_OS_LINUX
#ifdef Q_OS_WIN
    ui->le_ImpressPath->setText(tr("C:/Program Files/LibreOffice/program/soffice.exe"));
    ui->le_ShowOption->setText(tr("--impress --show"));
    ui->le_ShowPath->setText(tr("C:/Pictures/ImpressSlideShows"));
#endif

    restoreDefaults();
}

MainWindow::~MainWindow()
{
    saveDefaults();
    mouseclose();
    delete ui;
}

//
// Settings files are .ini file format and are stored per user
// We store the site list, including the last state of the site and the last date/time checked.
//
#define SETTINGS_VERSION "100"
#define SETTINGS_VERSION_LAST "100"
void MainWindow::createDefaults()
{
    // QSettings ( Format format, Scope scope, const QString & organization, const QString & application = QString(), QObject * parent = 0 )
    m_pSettings = new QSettings( QSettings::IniFormat, QSettings::UserScope,
                                  QString("WindyWeather"), QString("QtShowRunner") );

    QString     sFile = m_pSettings->fileName();
    qDebug()<<"Defaults filepath "<<sFile;

}

//
// save paths to the impress program, options and the path we
// were last using for shows. Show lists are saved in their own
// files, so we don't store those here.
// No warning on unsaved show lists on exit.
//
void    MainWindow::saveDefaults()
{
    createDefaults();

    qDebug()<<"MainWindow::saveDefaults saveDefaults";
    m_pSettings->beginGroup("WindowSettings");
    m_pSettings->setValue("size", this->size());
    m_pSettings->setValue("pos", this->pos());
    m_pSettings->endGroup();

    m_pSettings->beginGroup("Global");
    m_pSettings->setValue("version", SETTINGS_VERSION );
    m_pSettings->endGroup();

    m_pSettings->beginGroup("Defaults");
    m_pSettings->setValue("version", SETTINGS_VERSION );

    m_pSettings->setValue( "impress_path", ui->le_ImpressPath->text());
    m_pSettings->setValue( "show_option", ui->le_ShowOption->text());
    m_pSettings->setValue( "show_path", ui->le_ShowPath->text());
    qDebug()<<"saveDefaults ShowPath saved as "<<ui->le_ShowPath->text();

    m_pSettings->endGroup();
    delete m_pSettings;
    m_pSettings = NULL;

}

//
// restore everything we saved
// take care of the case that we didn't save a file. First Time we Run
// so we don't blow away the programmed defaults. After that they are restored
// from last exit.
//
void    MainWindow::restoreDefaults()
{
    createDefaults();

#if 0 // let's wait a bit on this
    // Get the size and position of the window as it is first created
    // as the default.
    QSize   defSize = this->size();
    QPoint  defPos = this->pos();

    m_pSettings->beginGroup("WindowSettings");
    this->resize(m_pSettings->value("size", defSize ).toSize());
    this->move(m_pSettings->value("pos", defPos ).toPoint());
    m_pSettings->endGroup();
    qDebug("MainWindow::restoreDefaults - Window restored");
#endif //wait a bit

    m_pSettings->beginGroup("Global");
    QString versionParams = m_pSettings->value("version", QString("") ).toString();
    m_pSettings->endGroup();
    if (versionParams.count() == 0 )
    {
        // not an error, just don't foul up the program defaults the first time
        qDebug()<<"MainWindow::restoreDefaults - no default settings";
        delete m_pSettings;
        m_pSettings = NULL;
        return;
    }
    m_pSettings->beginGroup("Defaults");

    QString versionSites = m_pSettings->value("version", QString(SETTINGS_VERSION) ).toString();

    setStatus(QString("Restoring Defaults"));
    qDebug() << "MainWindow::restoreDefaults - Restoring Defaults";

    ui->le_ImpressPath->setText( m_pSettings->value( "impress_path", "" ).toString() );
    ui->le_ShowOption->setText( m_pSettings->value( "show_option", "" ).toString() );
    ui->le_ShowPath->setText( m_pSettings->value( "show_path", "" ).toString() );
    qDebug()<<"MainWindow::restoreDefaults found show_path as "<<m_pSettings->value( "show_path", "" ).toString();


    m_pSettings->endGroup();
    delete m_pSettings;
    m_pSettings = NULL;

    setStatus(QString("Restored Defaults"));
    qDebug()<<"MainWindow::restoreDefaults - Restored Defaults";

}


void MainWindow::on_actionOpen_triggered()
{
    if ( showsBusy )
    {
        setStatus(tr("Stop Shows First"));
        return;
    }
    restoreShow();
}

void MainWindow::on_actionSave_triggered()
{
    if ( showsBusy )
    {
        setStatus(tr("Stop Shows First"));
        return;
    }
    saveShow();
}

//
// Save the list of shows as an ini file with the count and
// list of shows.
//
void    MainWindow::saveShow()
{

    if ( ui->lw_ShowList->count() == 0)
    {
        setStatus(tr("No shows to save"));
        return;
    }
    // Get file name to save the show list
    // default dir is where the shows are

    QDir d = QFileInfo(ui->le_ShowPath->text()).absoluteDir();
    QString absolute=d.absolutePath();

    QFileDialog saveFileDlg;
    saveFileDlg.setDefaultSuffix("show");
    QString sShowFile = saveFileDlg.getSaveFileName(this,
                    tr("Save Show File"),
                    absolute,
                    tr("ShowFile(*.show);;All Files(*.*)"));

    if ( 0 == sShowFile.count() )
    {
        // file dialog cancelled
        return;
    }

    // tidy up the filename by adding the suffix. Seems to be there on Windows but now Linux
    // but do it anyway for everyone.
    if ( !sShowFile.contains(tr(".show"),Qt::CaseInsensitive) )
    {
        sShowFile += tr(".show"); // add the missing suffix
    }

    // make our own private file, but using the INI format and settings
    // class to make this easy.
    QSettings* pShowFile = new QSettings( sShowFile, QSettings::IniFormat);

    qDebug() <<"MainWindow::saveShow "<<sShowFile;

    pShowFile->beginGroup("Global");
    pShowFile->setValue("version", SETTINGS_VERSION );
    pShowFile->endGroup();

    pShowFile->beginGroup("Show");
    pShowFile->setValue("version", SETTINGS_VERSION );

    int shows = ui->lw_ShowList->count();
    pShowFile->setValue( "show_count", shows);
    for (int i=0; i<shows; i++)
    {
        QString showpath;
        QString key = QString("show_%1").arg(i);
        showpath = ui->lw_ShowList->item(i)->text();
        pShowFile->setValue( key, showpath );
    }


    pShowFile->endGroup();
    delete pShowFile;
    pShowFile = NULL;
    setStatus(tr("Show Saved"));
    qDebug("MainWindow::saveShow - Show saved");

}

//
// Restore a show list from an INI settings file.
//
void    MainWindow::restoreShow()
{
    // Get file name to restore the show list
    // default dir is where the shows are

    QDir d = QFileInfo(ui->le_ShowPath->text()).absoluteDir();
    QString absolute=d.absolutePath();

    QString sShowFile = QFileDialog::getOpenFileName(this,
                    tr("Open Show File"),
                    absolute,
                    tr("ShowFiles(*.show);;All Files(*.*)"));
    if ( 0 == sShowFile.count() )
    {
        // user just cancelled out of the open dialog
        return;
    }

    // use the handy settings class
    QSettings* pShowFile = new QSettings( sShowFile, QSettings::IniFormat);

    qDebug() <<"MainWindow::restoreShow "<<sShowFile;


    // someday we might check the settings format and refuse, but just ignore for now

    pShowFile->beginGroup("Global");
    QString versionParams = pShowFile->value("version", QString(SETTINGS_VERSION) ).toString();
    pShowFile->endGroup();

    pShowFile->beginGroup("Show");

    QString versionSites = pShowFile->value("version", QString(SETTINGS_VERSION) ).toString();

    ui->lw_ShowList->clear();

    int nItems = pShowFile->value("show_count", 0 ).toInt();

    qDebug()<<QString("Restoring %1 shows").arg(nItems);

    for ( int i=0; i < nItems; i++ )
    {
        QString showpath;
        QString key = QString("show_%1").arg(i);
        showpath =  pShowFile->value( key, "" ).toString();
        ui->lw_ShowList->addItem(showpath);
      }

    pShowFile->endGroup();
    delete pShowFile;
    pShowFile = NULL;

    setStatus(tr("Restored %1 shows").arg(nItems));
    qDebug()<<QString("Restored %1 shows").arg(nItems);

}


void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_pb_BrowseShowPath_clicked()
{
    if ( showsBusy )
    {
        setStatus(tr("Stop shows first"));
        return;
    }
    setStatus(tr("Find an Impress slide show"));
    QString showfile = QFileDialog::getOpenFileName(this, tr("Impress Slide Show"),
                                                     ui->le_ShowPath->text(),
                                                     tr("Impress (*.odp);;All Files(*.*)"));

    if ( showfile.count() != 0 ) // if cancel, then empty string
    {
        ui->le_ShowPath->setText(showfile);
        setStatus(tr("Impress slide show file set"));
    }
    else
    {
        setStatus(tr(""));
    }
}

void MainWindow::on_pb_AddShow_clicked()
{
    if ( showsBusy )
    {
        setStatus(tr("Stop shows first"));
        return;
    }
    if ( ui->le_ShowPath->text().count() != 0 ) // if path set
    {
        ui->lw_ShowList->addItem(ui->le_ShowPath->text());
        setStatus(tr("Impress slide show added"));
    }
    else
    {
        setStatus(tr("Use (...) to find a slide show"));
    }
}

void MainWindow::on_pb_RemoveShow_clicked()
{
    if ( showsBusy )
    {
        setStatus(tr("Stop shows first"));
        return;
    }
    int selectedRow = ui->lw_ShowList->currentRow();
    if ( selectedRow != -1 )
    {
        QListWidgetItem* removedRow = ui->lw_ShowList->takeItem(selectedRow);
        delete removedRow;
        setStatus(tr("Show removed from list"));
    }
    else
    {
        setStatus(tr("Select a show in the list Ffirst"));
    }

}

void MainWindow::on_pb_StartShows_clicked()
{
    if (showsBusy)
    {
        setStatus(tr("Stop Shows First"));
        return;
    }
    if ( ui->lw_ShowList->count() == 0) // any shows in list
    {
        setStatus(tr("Add some shows to list first"));
        return;
    }

    showStopper = false;
    showsBusy = true;
    showIndex = 0;  // start with first show
    setStatus(tr("Starting first show"));
    startNextShow(); // starting first as next
}

void MainWindow::on_pb_StopShow_clicked()
{
    if ( !showsBusy )
    {
        setStatus(tr("Shows not running"));
        return;
    }
    setStatus(tr("Stopping shows"));
    showStopper = true;
    if ( theShow.state() == QProcess::Running )
    {
        endOfShow(); // stop current show
    }
    showsBusy = false; // shows are not running

}

void MainWindow::on_pb_BrowseImpressPath_clicked()
{
    if (showsBusy)
    {
        setStatus(tr("Shows are running. Stop first."));
        return;
    }
    setStatus(tr("Browse for Impress Program"));
    QString impressexe = QFileDialog::getOpenFileName(this, tr("Impress Program"),
                                                     ui->le_ImpressPath->text(),
                                                     tr("Executable (*.exe)"));

    if ( impressexe.count() != 0 ) // if cancel, then empty string
    {
        ui->le_ImpressPath->setText(impressexe);
        setStatus(tr("Impress program path set"));
    }
    else
    {
        setStatus(tr("Use (...) to find simpress.exe"));
    }
}

void MainWindow::on_pb_RemoveAllShows_clicked()
{
    if ( showsBusy )
    {
        setStatus(tr("Stop shows first"));
        return;
    }
    ui->lw_ShowList->clear();
    setStatus(tr("All Shows Removed from the List"));
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();              // show the About box as a modal dialog to keep it simple
}

void MainWindow::on_actionHelp_triggered()
{
    HelpDialog dialog(this);
    dialog.exec();              // show the About box as a modal dialog to keep it simple
}

//
// Just start clicking the mouse until the show ends
//
void MainWindow::clickingToEndShow()
{
    setStatus(tr("Waiting for show to end"));
    endOfShowSeen = false;
    stopClicking = false; // don't stop clicking now

    // kick the whole thing off with a timer
    QTimer::singleShot(WAIT_SECONDS * 1000, this, &MainWindow::clickToEnd);
}

//
// Look for black screen every few seconds.
// End of an impress slide show is an almost completely black screen
//
void MainWindow::clickToEnd()
{
    if ( stopClicking || showStopper )
    {
        setStatus(tr("Mouse clicks stopped"));
        return;
    }

    //
    // eventually the show will end when we click on the black screen
    //
    if ( theShow.state() != QProcess::Running )
    {
        if (ui->cbBeepOnShowEnd->isChecked())
        {
            QApplication::beep();
        }

        setStatus(tr("Show end"));
        endOfShowSeen = true;
        endOfShow();    // start next show
        return;
    }

    // click the mouse every WAIT_SECONDS and eventually
    // the show will end.
    mouseClick();

    QTimer::singleShot(WAIT_SECONDS * 1000, this, &MainWindow::clickToEnd);
}

//
// we have seen end of a show
//
void MainWindow::endOfShow()
{
    if ( !showsBusy )
    {
        setStatus(tr("endOfShow - but not showsBusy"));
        return;
    }
    // clobber the show if we have one.
    if ( theShow.state() == QProcess::Running )
    {
        if ( !theShow.waitForFinished( 5*1000 ) )
        {
            // if it won't stop, try one last time
            if ( theShow.state() == QProcess::Running )
            {
                theShow.kill();
                theShow.waitForFinished( 5*1000 );
            }
        }
    }

    // if we are stopping shows, then clean up
    if ( showStopper )
    {
        showsBusy = false;
        showStopper = false;
        setStatus(tr("Shows manually stopped"));
        return;
    }
    showIndex++;

    // any more shows left? If Not, then just loop back and play again
    if ( showIndex >= ui->lw_ShowList->count() )
    {
        setStatus(tr("End of show list. Looping."));
        showIndex = 0;
    }
    // Wait a few seconds and then start next show
    int secsToWait = ui->sb_BlackSeconds->value();
    QTimer::singleShot(secsToWait * 1000, this, &MainWindow::startNextShow);
}

//
// time to start the next show
//
void MainWindow::startNextShow()
{
    if ( !showsBusy )
    {
        setStatus(tr("startNextShow bad trigger at %1").arg(showIndex));
        return; // not supposed to be here now
    }
    // any show here to play?
    if ( showIndex >= ui->lw_ShowList->count() )
    {
        setStatus(tr("Can't start show at %1").arg(showIndex));
        showIndex = 0;
        return;
    }
    // launch the show and start waiting on black screen that is
    // end of a show.
    QString showName = ui->lw_ShowList->item(showIndex)->text();
    QString impress = ui->le_ImpressPath->text();
    QStringList option = ui->le_ShowOption->text().split(tr(" "));
    QStringList args = option << showName;

    setStatus(tr("Starting show %1").arg(showName));
    // start a child process of us - not working well
    // If we start it this way, we can't terminate it
    // but if we just keep clicking the mouse, and set the show to
    // not advance on mouse clicks, then it will eventually
    // stop by itself and we will see that.
    theShow.start(impress, args, QProcess::ReadWrite );


    // if it didn't start after 10 sec, it's an error
    theShow.waitForStarted(10*1000);
    if ( theShow.state() != QProcess::Running )
    {
        setStatus(tr("Show didn't start %1").arg(showName));
        showsBusy = false;
        return;
    }

    // just keep clicking mouse to end show eventually
    QTimer::singleShot(WAIT_SECONDS * 1000, this, &MainWindow::clickingToEndShow);
}



//
// Set the slide show to not advance on mouse clicks
// and then just click away every second until at the
// end of the show, it terminates and exits itself.
// What we do to make it work. Wheeew.
//
#ifdef Q_OS_LINUX // Linux

void MainWindow::mouseClick()
{
    // On Linux there is a command line tool to click the mouse
    // let's use that since nothing else freaking works.
    //
    // Launch the cmd line tool and wait for it to finish.
    // Should not take very long.
    // BTW, you'll need to
    // sudo apt-get install -y xdotool
    // to make sure you have the tool
    //
    QProcess cmd;
    cmd.execute(QString(tr("xdotool click 1")));
    // wait until that finishes, since it won't take very long
    // nothing else to do anyway, the show is running

}
#endif // Q_OS_LINUX

#ifdef Q_OS_WINDOWS // Windows Only

void MainWindow::mouseClick()
{
    // use the windows OS solution for mouse clicks that does work
    if (!mouseclick())
    {
        setStatus(tr("mouseclick failed"));
        qDebug()<<"mouseclick failed";

    }
}

#endif // Q_OS_WINDOWS



