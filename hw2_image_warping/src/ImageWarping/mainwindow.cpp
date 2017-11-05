#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "ImageWidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setGeometry(300, 150, 800, 450);

    image_widget = new ImageWidget();
    setCentralWidget(image_widget);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* paintevent){}

void MainWindow::createActions(){

    action_open  = new QAction(tr("&Open"), this);
    action_open -> setShortcuts(QKeySequence::Open);
    action_open -> setStatusTip(tr("Open an existing file"));
    connect(action_open, SIGNAL(triggered()), image_widget, SLOT(Open()));

    action_save  = new QAction(tr("&Save"), this);
    action_save -> setShortcuts(QKeySequence::Save);
    action_save -> setStatusTip(tr("Save the document to disk"));
    connect(action_save, SIGNAL(triggered()), image_widget, SLOT(Save()));

    action_invert  = new QAction(tr("Inverse"), this);
    action_invert ->setStatusTip(tr("Invert all pixel value in the image"));
    connect(action_invert, SIGNAL(triggered()), image_widget, SLOT(Invert()));

    action_mirror  = new QAction(tr("Mirror"), this);
    action_mirror -> setStatusTip(tr("Mirror image vertically or horizontally"));
    connect(action_mirror, SIGNAL(triggered()), image_widget, SLOT(Mirror()));

    action_gray  = new QAction(tr("Grayscale"), this);
    action_gray ->setStatusTip(tr("Gray-scale map"));
    connect(action_gray, SIGNAL(triggered()), image_widget, SLOT(TurnGray()));

    action_restore  = new QAction(tr("Restore"), this);
    action_restore -> setStatusTip(tr("Show origin image"));
    connect(action_restore, SIGNAL(triggered()), image_widget, SLOT(Restore()));

    action_delete  = new QAction(tr("Delete"), this);
    action_delete -> setStatusTip(tr("Delete lines"));
    connect(action_delete, SIGNAL(triggered()), image_widget, SLOT(Delete()));

    action_idw  = new QAction(tr("IDW"), this);
    action_idw -> setStatusTip(tr("Image Warping with IDW"));
    connect(action_idw, SIGNAL(triggered()), image_widget, SLOT(IdwWarp()));

    action_rbf  = new QAction(tr("RBF"), this);
    action_rbf -> setStatusTip(tr("Image Warping with RBF"));
    connect(action_rbf, SIGNAL(triggered()), image_widget, SLOT(RbfWarp()));
}

void MainWindow::createMenus(){

    menu_file  = menuBar() -> addMenu(tr("&File"));
    menu_file -> setStatusTip(tr("File menu"));
    menu_file -> addAction(action_open);
    menu_file -> addAction(action_save);

    menu_edit  = menuBar() -> addMenu(tr("&Edit"));
    menu_edit -> setStatusTip(tr("Edit menu"));
    menu_edit -> addAction(action_invert);
    menu_edit -> addAction(action_mirror);
    menu_edit -> addAction(action_gray);
    menu_edit -> addAction(action_restore);
    menu_edit -> addAction(action_delete);
    menu_edit -> addAction(action_idw);
    menu_edit -> addAction(action_rbf);
}


void MainWindow::createToolBars(){
    toolbar_file  = addToolBar(tr("File"));
    toolbar_file -> addAction(action_open);
    toolbar_file -> addAction(action_save);

    // Add separator in toolbar
    toolbar_file ->addSeparator();
    toolbar_file ->addAction(action_invert);
    toolbar_file ->addAction(action_mirror);
    toolbar_file ->addAction(action_gray);
    toolbar_file ->addAction(action_restore);
    toolbar_file -> addAction(action_delete);
    toolbar_file -> addAction(action_idw);
    toolbar_file -> addAction(action_rbf);
}

void MainWindow::createStatusBar(){
    statusBar() -> showMessage(tr("Ready"));
}

