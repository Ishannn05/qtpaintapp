#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scribbleArea.h"

MainWindow::MainWindow(QWidget *parent) //MainWindow Constructor
    : QMainWindow(parent)
{
    scribbleArea  = new ScribbleArea;
    setCentralWidget(scribbleArea); //setting it as the central widget so that it takes all of the screen available for the application
    createAction();
    createMenus();
    setWindowTitle(tr("Scribble")); //This is how the application name is set in Qt
    resize(500,500);
}

MainWindow::~MainWindow()
{
    delete ui; //Default constructor was present when the application was created
    delete scribbleArea;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(maybeSave()){
        event->accept();
    }
    else{
        event->ignore();
    }
}

void MainWindow::open(){
    if(maybeSave()){
        //this is a reference to the current application, tr("OPEN FILE") is the text that will appear on the dialogue, QDir::currentPath is the path where the dialogue will open for selecting the file
        QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
        if(!filename.isEmpty()){//Check if it is a valid filename
            scribbleArea->openImage(filename);
        }
    }
}

// Called when the user clicks Save As in the menu
void MainWindow::save()
{
    // A QAction represents the action of the user clicking
    QAction *action = qobject_cast<QAction *>(sender());

    // Stores the array of bytes of the users data
    QByteArray fileFormat = action->data().toByteArray();

    // Pass it to be saved
    saveFile(fileFormat);
}

void MainWindow::penColor(){
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if(newColor.isValid()){
        scribbleArea->setPenColor(newColor);
    }
}

void MainWindow::penWidth(){
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble") ,tr("Select pen width : "), scribbleArea->penWidth() ,1 , 50, 1, &ok); //Each time the user selects a new penwidth, call scribbleArea->penWidth. 1(min), 50(max), 1(step)
    if(ok) //if the user pressed Ok button
        scribbleArea->setPenWidth(newWidth);
}


void MainWindow::about(){
    QMessageBox::about(this, tr("About Scribble"), tr("<b>Scribble paint application</b>"));
    //first the title text then the about page text which can include html formating
}

/*All actions are created here. Add a new action under this function.*/
void MainWindow::createAction(){
    openAct = new QAction(tr("&Open"), this); //creating an Open button for the user to click and tying it to this application
    openAct->setShortcut(QKeySequence::Open); //Setting a shortcut for open
    connect(openAct, SIGNAL(triggered()), this, SLOT(open())); //Tying the openAct signal to the mainWindow and SLOT i.e. the action that needs to be executed
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) { //supportedImageFormats returns a list of all supported image formats
        QString text = tr("%1...").arg(QString(format).toUpper()); //The Qstring(format).upper will replace the %1 in the text
        QAction *action = new QAction(text, this);
        action->setData(format); //when the data is set
        connect(action, SIGNAL(triggered()), this, SLOT(save())); //whenever any of the format is selected, call the save function
        saveAsActs.append(action);
    }
    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print())); //triggered() means that it is clicked on

    exitAct = new QAction(tr("&Exit..."), this);
    exitAct->setShortcut(QKeySequence::Quit); //Setting a shortcut for open
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create pen color action and tie to MainWindow::penColor()
    penColorAct = new QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    // Create pen width action and tie to MainWindow::penWidth()
    penWidthAct = new QAction(tr("Pen &Width..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    // Create clear screen action and tie to MainWindow::clearImage()
    clrScrAct = new QAction(tr("&Clear Screen"), this);
    clrScrAct->setShortcut(tr("Ctrl+L"));
    connect(clrScrAct, SIGNAL(triggered()),scribbleArea, SLOT(clearImage()));

    // Create about action and tie to MainWindow::about()
    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    // Create about Qt action and tie to MainWindow::aboutQt()
    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}


// Create the menubar
void MainWindow::createMenus()
{
    // Create Save As option and the list of file types
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    // Attach all actions to File
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // Attach all actions to Options
    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clrScrAct);

    // Attach all actions to Help
    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    // Add menu items to the menubar
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

bool MainWindow::maybeSave()
{
    // Check for changes since last save
    if (scribbleArea->isModified()) {
       QMessageBox::StandardButton ret;

       // Scribble is the title
       // Add text and the buttons
       ret = QMessageBox::warning(this, tr("Scribble"),
                          tr("The image has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel);

       // If save button clicked call for file to be saved
        if (ret == QMessageBox::Save) {
            return saveFile("png");

        // If cancel do nothing
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    // Define path, name and default file type
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    // Get selected file from dialog
    // Add the proper file formats and extensions
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));

    // If no file do nothing
    if (fileName.isEmpty()) {
        return false;
    } else {

        // Call for the file to be saved
        return scribbleArea->saveImage(fileName, fileFormat.constData());
    }
}

// ---------- END mainwindow.cpp ----------

