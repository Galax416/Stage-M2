#include <QApplication>

#include "Mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create main window
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
    
}