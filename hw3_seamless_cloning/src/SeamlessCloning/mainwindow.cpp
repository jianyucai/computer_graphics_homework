#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "seamlesscloning.h"
#include "utils.h"
#include <iostream>
#include <QtWidgets>
#include <string>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){

    //ui->setupUi(this);
    mdi_area = new QMdiArea;
    mdi_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdi_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdi_area);

    window_mapper = new QSignalMapper(this);
    connect(window_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(SetActiveSubWindow(QWidget*)));

    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
}


MainWindow::~MainWindow(){}


void MainWindow::CreateActions(){

    // File IO
    action_open = new QAction(tr("&Open..."), this);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setStatusTip(tr("Open an existing file"));
    connect(action_open, SIGNAL(triggered()), this, SLOT(Open()));

    action_save = new QAction(tr("&Save"), this);
    action_save->setShortcuts(QKeySequence::Save);
    action_save->setStatusTip(tr("Save the document to disk"));
    connect(action_save, SIGNAL(triggered()), this, SLOT(Save()));

    action_saveas = new QAction(tr("Save &As..."), this);
    action_saveas->setShortcuts(QKeySequence::SaveAs);
    action_saveas->setStatusTip(tr("Save the document under a new name"));
    connect(action_saveas, SIGNAL(triggered()), this, SLOT(SaveAs()));

    // Image processing
    action_invert = new QAction(tr("Inverse"), this);
    action_invert->setStatusTip(tr("Invert all pixel value in the image"));
    connect(action_invert, SIGNAL(triggered()), this, SLOT(Invert()));

    action_mirror = new QAction(tr("Mirror"), this);
    action_mirror->setStatusTip(tr("Mirror image vertically or horizontally"));
    connect(action_mirror, SIGNAL(triggered()), this, SLOT(Mirror()));

    action_gray = new QAction(tr("Grayscale"), this);
    action_gray->setStatusTip(tr("Gray-scale map"));
    connect(action_gray, SIGNAL(triggered()), this, SLOT(GrayScale()));

    action_restore = new QAction(tr("Restore"), this);
    action_restore->setStatusTip(tr("Show origin image"));
    connect(action_restore, SIGNAL(triggered()), this, SLOT(Restore()));

    // Poisson image editting
    action_choose_polygon = new QAction(tr("RectChoose"), this);
    connect(action_choose_polygon, SIGNAL(triggered()), this, SLOT(ChooseRect()));

    action_paste = new QAction(tr("Paste"), this);
    connect(action_paste, SIGNAL(triggered()), this, SLOT(Paste()));

    action_clone = new QAction(tr("Clone"), this);
    connect(action_clone, SIGNAL(triggered()), this, SLOT(Clone()));
}


void MainWindow::CreateMenus(){

    menu_file = menuBar()->addMenu(tr("&File"));
    menu_file->setStatusTip(tr("File menu"));
//	menu_file_->addAction(action_new_);
    menu_file->addAction(action_open);
    menu_file->addAction(action_save);
    menu_file->addAction(action_saveas);

    menu_edit = menuBar()->addMenu(tr("&Edit"));
    menu_edit->setStatusTip(tr("Edit menu"));
    menu_edit->addAction(action_invert);
    menu_edit->addAction(action_mirror);
    menu_edit->addAction(action_gray);
    menu_edit->addAction(action_restore);
}


void MainWindow::CreateToolBars(){

    toolbar_file = addToolBar(tr("File"));
//	toolbar_file_->addAction(action_new_);
    toolbar_file->addAction(action_open);
    toolbar_file->addAction(action_save);

    // Add separator in toolbar
    toolbar_file->addSeparator();
    toolbar_file->addAction(action_invert);
    toolbar_file->addAction(action_mirror);
    toolbar_file->addAction(action_gray);
    toolbar_file->addAction(action_restore);

    // Poisson Image Editing
    toolbar_file->addSeparator();
    toolbar_file->addAction(action_choose_polygon);
    toolbar_file->addAction(action_paste);
    toolbar_file->addAction(action_clone);
}


void MainWindow::CreateStatusBar(){
    statusBar()->showMessage(tr("Ready"));
}


void MainWindow::Open(){
    QString filename = QFileDialog::getOpenFileName(this);
    if(!filename.isEmpty()){
        QMdiSubWindow *existing = FindChild(filename);

        if (existing){
            mdi_area->setActiveSubWindow(existing);
            return;
        }

        ChildWindow *child = CreateChildWindow();
        if (child->LoadFile(filename)){
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();

            // Change size of the child window so it can fit image size
            int x = child->geometry().x();
            int y = child->geometry().y();
            int width = child->imagewidget->ImageWidth();
            int height = child->imagewidget->ImageHeight();
            mdi_area->activeSubWindow()->setFixedSize(width+2*x, height+x+y);
        }
        else{
            child->close();
        }
    }
}


void MainWindow::Save(){
    SaveAs();
}


void MainWindow::SaveAs(){
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());
    window->imagewidget->SaveAs();
}


ChildWindow *MainWindow::CreateChildWindow(){
    ChildWindow *child = new ChildWindow;
    mdi_area->addSubWindow(child);

    return child;
}


void MainWindow::SetActiveSubWindow(QWidget* window){
    if (!window){
        return;
    }

    mdi_area->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}


void MainWindow::Invert(){
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());
    window->imagewidget->Invert();
}


void MainWindow::Mirror(){
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());
    window->imagewidget->Mirror();
}


void MainWindow::GrayScale(){
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());
    window->imagewidget->TurnGray();
}


void MainWindow::Restore(){
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());
    window->imagewidget->Restore();
}


void MainWindow::ChooseRect(){
    // Set source child window
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());
    window->imagewidget->set_draw_status_to_choose();
    child_source = window;
}


void MainWindow::Paste(){
    // Paste image rect region to object image
    ChildWindow *child = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());
    child->imagewidget->set_draw_status_to_paste();
    child->imagewidget->set_source_window(child_source);
}


void MainWindow::Clone(){
    ChildWindow *child = qobject_cast<ChildWindow *>(mdi_area->activeSubWindow()->widget());

    QImage* source_img = this->child_source->imagewidget->image_;
    QImage* cloned_img = child->imagewidget->image_;
    vector<QPoint>s_list = child->imagewidget->source_point_list;
    vector<QPoint>c_list = child->imagewidget->cloned_point_list;


    SeamlessCloning* c = new SeamlessCloning(source_img, cloned_img, s_list, c_list);

    c->seamless_cloning();
}


QMdiSubWindow *MainWindow::FindChild(const QString &filename){
    QString canonical_filepath = QFileInfo(filename).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdi_area->subWindowList()){
        ChildWindow *child = qobject_cast<ChildWindow *>(window->widget());
        if (child->current_file() == canonical_filepath){
            return window;
        }
    }

    return 0;
}
