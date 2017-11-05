#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"imagewidget.h"

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
    QMenu		*menu_edit;
    QMenu		*menu_help;
    QToolBar	*toolbar_file;
    QToolBar	*toolbar_edit;
    QAction		*action_open;
    QAction		*action_save;
    QAction		*action_saveas;
    QAction		*action_invert;
    QAction		*action_mirror;
    QAction		*action_gray;
    QAction		*action_restore;
    QAction     *action_delete;
    QAction     *action_idw;
    QAction     *action_rbf;

    ImageWidget		*image_widget;
};

#endif // MAINWINDOW_H
