#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDropEvent>
#include <QMainWindow>
#include <QTextEdit>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow final : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    DELETE_COPY_MOVE_ROUTINES(MainWindow);

  protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

  private:
    Ui::MainWindow *ui;
    QWidget        *centralWidget;
    QTextEdit      *textEdit;
};

#endif // MAINWINDOW_H
