#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imagewidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    //void closeEvent(QCloseEvent* e);
    void paintEvent(QPaintEvent* e);


private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();


private:
    Ui::MainWindow *ui;

    QMenu		*menu_file;
    QMenu		*menu_help;
    QToolBar	*toolbar_file;
    QAction		*action_open;
    QAction     *action_draw;
    QAction     *action_drag;

    ImageWidget		*image_widget;
    DisplayWidget	*image_widget_warped;
};

#endif // MAINWINDOW_H
