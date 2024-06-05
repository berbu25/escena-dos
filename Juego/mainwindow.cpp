#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), x(0), attemptsLeft(3), moveRight(false), moveLeft(false)
{
    setFixedSize(900, 600);

    initializeRedBlocks();

    // Temporizador para actualizar las posiciones de las balas
    bulletTimer = new QTimer(this);
    connect(bulletTimer, &QTimer::timeout, this, &MainWindow::updatePositions);
    bulletTimer->start(10); // Intervalo para las balas

    // Temporizador para actualizar la posición del jugador
    playerTimer = new QTimer(this);
    connect(playerTimer, &QTimer::timeout, this, &MainWindow::updatePlayerPosition);
    playerTimer->start(5); // Intervalo para el jugador
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeRedBlocks()
{
    redBlocks.clear();
    for (int i = 0; i < 5; ++i)
    {
        Block block;
        block.x = rand() % width();
        block.y = 0;
        block.dx = (rand() % 11) - 5; // Movimiento horizontal aleatorio
        block.dy = rand() % 10 + 5; // Movimiento vertical constante
        block.isParabolic = rand() % 2 == 0; // Algunos bloques tendrán movimiento parabólico
        redBlocks.append(block);
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Establecer el color azul
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::blue);

    // Dibujar el bloque azul en la parte inferior de la pantalla
    painter.drawRect(x + 30, height() - blockSize - 30, blockSize, blockSize);

    // Dibujar los bloques rojos
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);
    for (const auto &block : redBlocks)
    {
        painter.drawRect(block.x, block.y, redBlockSize, redBlockSize);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right)
    {
        moveRight = true;
    }
    else if (event->key() == Qt::Key_Left)
    {
        moveLeft = true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right)
    {
        moveRight = false;
    }
    else if (event->key() == Qt::Key_Left)
    {
        moveLeft = false;
    }
}

void MainWindow::updatePlayerPosition()
{
    if (moveRight)
    {
        if (x + blockSize + stepSize <= width())
        {
            x += stepSize;
            update(); // Redibujar la ventana
        }
    }
    if (moveLeft)
    {
        if (x - stepSize >= 0)
        {
            x -= stepSize;
            update(); // Redibujar la ventana
        }
    }
}

void MainWindow::updatePositions()
{
    for (auto &block : redBlocks)
    {
        block.y += block.dy;
        if (block.isParabolic)
        {
            block.x += block.dy * block.dy / 10; // Movimiento parabólico
        }
        else
        {
            block.x += block.dx; // Movimiento lineal
        }

        // Reiniciar la posición del bloque si sale de la pantalla
        if (block.y > height() || block.x < 0 || block.x > width())
        {
            block.x = rand() % width();
            block.y = 0;
            block.dx = (rand() % 11) - 5;
            block.dy = rand() % 10 + 5;
            block.isParabolic = rand() % 2 == 0;
        }

        // Verificar colisión con el bloque azul
        if (checkCollision(block))
        {
            attemptsLeft--;
            if (attemptsLeft > 0)
            {
                resetBlueBlock();
            }
            else
            {
                endGame();
                return;
            }
        }
    }
    update(); // Redibujar la ventana
}

bool MainWindow::checkCollision(const Block &block)
{
    QRect blueBlockRect(x + 30, height() - blockSize - 30, blockSize, blockSize);
    QRect redBlockRect(block.x, block.y, redBlockSize, redBlockSize);
    return blueBlockRect.intersects(redBlockRect);
}

void MainWindow::resetBlueBlock()
{
    x = 0; // Reiniciar la posición del bloque azul
    update(); // Redibujar la ventana
}

void MainWindow::endGame()
{
    bulletTimer->stop();
    playerTimer->stop();
    QMessageBox::information(this, "Fin del juego", "Has perdido. Fin del juego.");
    close(); // Cerrar la ventana
}
