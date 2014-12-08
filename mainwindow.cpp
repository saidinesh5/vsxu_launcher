#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    QRect screenGeometry = QApplication::desktop()->screenGeometry();

    ui->setupUi(this);

    ui->widthSpinBox->setValue(screenGeometry.width());
    ui->heightSpinBox->setValue(screenGeometry.height());

    connect(ui->launchButton, SIGNAL(clicked()), this, SLOT(onLaunchClicked()));
    connect(ui->binaryComboBox, SIGNAL(activated(int)), this, SLOT(onBinaryActivated(int)) );

    populateBinaryComboBox( "vsxu_player", "VSXu Player" );
    populateBinaryComboBox( "vsxu_artiste", "VSXu Artiste" );
    populateBinaryComboBox( "", "Manually specify.." );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateBinaryComboBox(QString filename, QString description)
{
    QString binaryname = filename;

#ifdef Q_OS_WIN
    binaryname = binaryname + ".exe";
#endif

    QDir d;
    if( (!filename.isEmpty()))
    {
        if(d.exists(binaryname))
            ui->binaryComboBox->addItem(description, d.absoluteFilePath(binaryname));
    }
    else
    {
        ui->binaryComboBox->addItem(description, "");
    }

}

void MainWindow::onLaunchClicked()
{
    QString binaryPath = ui->binaryComboBox->itemData(ui->binaryComboBox->currentIndex()).toString();
    if(binaryPath.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please Select the VSXu Binary to Launch"));
        msgBox.exec();
        return;
    }

    QStringList args;
    args << "-s" << QString("%1x%2").arg(QString::number(ui->widthSpinBox->value())).arg(QString::number(ui->heightSpinBox->value()));
    if(ui->fullscreenCheckBox->isChecked())
        args << "-f";

    QProcess vsxuProcess;
    vsxuProcess.startDetached(binaryPath,args);


    QApplication::exit();
}


void MainWindow::onBinaryActivated(int index)
{
    if(ui->binaryComboBox->itemData(index).toString().isEmpty())
    {
        // If the User activated the "Manually specify..." option, for which filepath is ""
        QString filepath = QFileDialog::getOpenFileName(this,
                                                        tr("Select VSXu Bianry"),
                                                        QDir::currentPath(),
                                                        "VSXu Player (vsxu_player vsxu_player.exe);;VSXu Artiste (vsxu_artiste vsxu_artiste.exe);; Any File (*)" );
        if(!filepath.isEmpty())
        {
            ui->binaryComboBox->insertItem(-1,filepath, filepath);
            ui->binaryComboBox->setCurrentIndex( ui->binaryComboBox->count() - 2 );
        }
    }
}
