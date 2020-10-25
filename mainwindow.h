#ifndef MAINWINDOW_H //This makes sure that this header or information isn't loaded twice
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>


class ScribbleArea;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void createAction();
    void createMenus();
    bool maybeSave();
    bool saveFile(const QByteArray &fileFormat);
    ScribbleArea *scribbleArea;
    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;
    QAction *openAct;
    QList<QAction *> saveAsActs;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *penColorAct;
    QAction *penWidthAct;
    QAction *printAct;
    QAction *clrScrAct;
    QAction *exitAct;

protected:
    void closeEvent(QCloseEvent *event) override; //This will close an event i.e. stop it from executing whenever we want it to stop.

private slots:
    void open();
    void save();
    void penColor();
    void penWidth();
    void about();

};
#endif // MAINWINDOW_H
