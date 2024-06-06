#include "mainwindow.h"
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(int numBlocks, QWidget *parent)
    : QMainWindow(parent), x(0), attemptsLeft(3), moveRight(false), moveLeft(false), isJumping(false), jumpHeight(0), jumpSpeed(20), jumpMaxHeight(200)
{
    setFixedSize(900, 600);
    setFocusPolicy(Qt::StrongFocus); // Agregar esta línea

    initializeRedBlocks(numBlocks);
    initializeStaticObjects();

    // Temporizador para actualizar las posiciones de las balas
    bulletTimer = new QTimer(this);
    connect(bulletTimer, &QTimer::timeout, this, &MainWindow::updatePositions);
    bulletTimer->start(20); // Intervalo para las balas

    // Temporizador para actualizar la posición del jugador
    playerTimer = new QTimer(this);
    connect(playerTimer, &QTimer::timeout, this, &MainWindow::updatePlayerPosition);
    playerTimer->start(10); // Intervalo para el jugador

    // Crear botón de salto
    jumpButton = new QPushButton("Jump", this);
    jumpButton->setGeometry(10, 10, 75, 23);
    connect(jumpButton, &QPushButton::pressed, this, &MainWindow::onJumpButtonPressed);

    // Limpiar el archivo al inicio
    QFile file("simulation_results.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.close();

}

MainWindow::~MainWindow()
{
    delete bulletTimer;
    delete playerTimer;
}

void MainWindow::initializeRedBlocks(int numBlocks)
{
    redBlocks.clear();
    for (int i = 0; i < numBlocks; ++i)
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

void MainWindow::initializeStaticObjects()
{
    staticObjects.clear();
    // Añadir algunos objetos estáticos al azar
    for (int i = 0; i < 5; ++i)
    {
        StaticObject object;
        object.x = rand() % (width() - 50);
        object.y = height() - blockSize - 30; // Alineado verticalmente con el jugador
        object.width = 50;
        object.height = 100;
        staticObjects.append(object);
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Dibujar los objetos estáticos (edificios) en gris
    painter.setBrush(Qt::gray);
    painter.setPen(Qt::gray);
    for (const auto &object : staticObjects)
    {
        painter.drawRect(object.x, object.y - object.height, object.width, object.height); // Ajustar la altura
    }

    // Establecer el color azul
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::blue);

    // Dibujar el bloque azul en la parte inferior de la pantalla
    painter.drawRect(x + 30, height() - blockSize - 30 - jumpHeight, blockSize, blockSize);

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
    else if (event->key() == Qt::Key_Up && !isJumping)
    {
        isJumping = true;
        jumpSpeed = 20; // Inicializar la velocidad del salto al empezar
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

    // Mover jugador hacia arriba si está saltando
    if (isJumping)
    {
        jumpHeight += jumpSpeed;
        if (jumpHeight >= jumpMaxHeight)
        {
            jumpSpeed = -abs(jumpSpeed); // Invertir dirección para caer
        }
        update(); // Redibujar la ventana
    }
    else
    {
        // Caída del jugador
        jumpHeight -= gravity; // Simular caída
        if (jumpHeight <= 0)
        {
            jumpHeight = 0;
            handleElasticCollision(); // Manejar colisión elástica
        }
        update(); // Redibujar la ventana
    }
    // Guardar la posición actual del jugador
    QString playerData = QString("Player - X: %1, Jump Height: %2, Jump Speed: %3, Is Jumping: %4")
                             .arg(x)
                             .arg(jumpHeight)
                             .arg(jumpSpeed)
                             .arg(isJumping);
    writeToFile(playerData);

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

        QString blockData = QString("Red Block - X: %1, Y: %2, DX: %3, DY: %4, Parabolic: %5")
                                .arg(block.x)
                                .arg(block.y)
                                .arg(block.dx)
                                .arg(block.dy)
                                .arg(block.isParabolic);
        writeToFile(blockData); // Mover writeToFile dentro del bucle

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
    QRect blueBlockRect(x + 30, height() - blockSize - 30 - jumpHeight, blockSize, blockSize);
    QRect redBlockRect(block.x, block.y, redBlockSize, redBlockSize);
    return blueBlockRect.intersects(redBlockRect);
}

void MainWindow::handleElasticCollision()
{
    jumpSpeed = -jumpSpeed * restitution; // Cambiar la dirección del movimiento vertical y aplicar el coeficiente de restitución

    // Verificar si la velocidad es lo suficientemente pequeña para detener el salto
    if (abs(jumpSpeed) < 1)
    {
        jumpSpeed = 0;
        isJumping = false;
        jumpHeight = 0; // Asegurarse de que el jugador esté en el suelo
    }
}

void MainWindow::resetBlueBlock()
{
    x = 0; // Reiniciar la posición del bloque azul
    jumpHeight = 0; // Reiniciar la altura de salto
    jumpSpeed = 20; // Reiniciar la velocidad de salto
    update(); // Redibujar la ventana
}

void MainWindow::endGame()
{
    bulletTimer->stop();
    playerTimer->stop();
    QMessageBox::information(this, "Fin del juego", "Has perdido. Fin del juego.");
    close(); // Cerrar la ventana
}

void MainWindow::onJumpButtonPressed()
{
    if (!isJumping) // Solo iniciar el salto si no se está saltando
    {
        isJumping = true;
        jumpSpeed = 20; // Inicializar la velocidad del salto al empezar
    }
}

void MainWindow::writeToFile(const QString &data)
{
    QFile file("simulation_results.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << data << "\n";
        file.close();
    }
}

