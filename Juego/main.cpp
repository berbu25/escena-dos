// main.cpp
#include <QApplication>
#include "mainwindow.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Mostrar cuadro de diálogo de bienvenida
    QMessageBox::information(nullptr, "Bienvenido", "¡Bienvenido al juego!");

    int numBlocks = 10; // Valor predeterminado
    if (argc > 1) {
        numBlocks = std::atoi(argv[1]);
    }

    MainWindow w(numBlocks);
    w.show();

    return a.exec();
}
