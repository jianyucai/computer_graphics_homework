#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include "childwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // File IO
//	void New();
    void Open();								// Open an existing file
    void Save();								// Save image to file
    void SaveAs();
    ChildWindow *CreateChildWindow();
    void SetActiveSubWindow(QWidget* window);

    // Image Processing
    void Invert();								// Invert each pixel's rgb value
    void Mirror();								// Mirror image vertically or horizontally
    void GrayScale();							// Turn image to gray-scale map
    void Restore();								// Restore image to origin

    // Poisson Image Editing
    void ChooseRect();							// Choose rectangle region
    void Paste();								// Paste rect region to object image
    void Clone();

private:
    void CreateActions();
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();

    QMdiSubWindow *FindChild(const QString &filename);

private:
    Ui::MainWindow *ui;

    QMenu						*menu_file;
    QMenu						*menu_edit;
    QMenu						*menu_help;
    QToolBar					*toolbar_file;
    QToolBar					*toolbar_edit;
//	QAction						*action_new_;
    QAction						*action_open;
    QAction						*action_save;
    QAction						*action_saveas;

    QAction						*action_invert;
    QAction						*action_mirror;
    QAction						*action_gray;
    QAction						*action_restore;

    QAction						*action_choose_polygon;
    QAction						*action_copy;
    QAction						*action_paste;

    QAction                     *action_clone;

    QMdiArea					*mdi_area;
    QSignalMapper				*window_mapper;

    ChildWindow					*child_source;
};

#endif // MAINWINDOW_H
