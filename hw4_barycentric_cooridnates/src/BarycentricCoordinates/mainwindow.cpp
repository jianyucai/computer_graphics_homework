#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setGeometry(300, 150, 800, 450);

    // Initialization of ImageWidgets
    image_widget = new ImageWidget();
    image_widget_warped = image_widget->display_widget;

    // Set layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(image_widget);
    layout->addWidget(image_widget_warped);

    // Set layout in QWidget
    QWidget *window = new QWidget();
    window->setLayout(layout);

    // Set QWidget as the central layout of the main window
    setCentralWidget(window);

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
    action_open -> setStatusTip(tr("Open an existing file"));
    connect(action_open, SIGNAL(triggered()), image_widget, SLOT(Open()));

    action_draw  = new QAction(tr("&Draw Polygon"), this);
    action_draw -> setStatusTip(tr("Draw a Polygon"));
    connect(action_draw, SIGNAL(triggered()), image_widget, SLOT(Draw()));

    action_drag  = new QAction(tr("&Drag Vertex"), this);
    action_drag -> setStatusTip(tr("Drag a Polygon Vertex"));
    connect(action_drag, SIGNAL(triggered()), image_widget, SLOT(Drag()));

}

void MainWindow::createMenus(){
    menu_file  = menuBar() -> addMenu(tr("&File"));
    menu_file -> setStatusTip(tr("File menu"));
    menu_file -> addAction(action_open);
    menu_file -> addAction(action_draw);
    menu_file -> addAction(action_drag);
}


void MainWindow::createToolBars(){
    toolbar_file  = addToolBar(tr("File"));
    toolbar_file -> addAction(action_open);
    toolbar_file -> addAction(action_draw);
    toolbar_file -> addAction(action_drag);
}

void MainWindow::createStatusBar(){
    statusBar() -> showMessage(tr("Ready"));
}

