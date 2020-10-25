#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); //This is the main application
    MainWindow w; //This is the main Window
    w.show(); //Showing the main window
    return a.exec(); //Display the main window
}
