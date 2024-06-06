// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTimer>

class QPushButton;

struct Block {
    int x;
    int y;
    int dx;
    int dy;
    bool isParabolic;
};

struct StaticObject {
    int x;
    int y;
    int width;
    int height;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int numBlocks, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updatePlayerPosition();
    void updatePositions();
    void onJumpButtonPressed();

private:
    void initializeRedBlocks(int numBlocks);
    void initializeStaticObjects();
    void handleElasticCollision();
    bool checkCollision(const Block &block);
    void resetBlueBlock();
    void endGame();
    void writeToFile(const QString &data); // Declaración de la función writeToFile

    int x;
    int attemptsLeft;
    bool moveRight;
    bool moveLeft;
    bool isJumping;
    int jumpHeight;
    int jumpSpeed;
    int jumpMaxHeight;
    const int blockSize = 50;
    const int redBlockSize = 30;
    const int stepSize = 10;
    const int gravity = 10;
    const double restitution = 0.5;
    QVector<Block> redBlocks;
    QVector<StaticObject> staticObjects;
    QTimer *bulletTimer;
    QTimer *playerTimer;
    QPushButton *jumpButton;
};

#endif // MAINWINDOW_H
