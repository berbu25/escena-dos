#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QMessageBox>
#include <QVector>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int numBlocks, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updatePositions();
    void updatePlayerPosition();
    void onJumpButtonPressed();
    void onJumpButtonReleased();

private:
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

    void initializeRedBlocks(int numBlocks);
    void initializeStaticObjects();
    bool checkCollision(const Block &block);
    void resetBlueBlock();
    void endGame();

    QVector<Block> redBlocks;
    QVector<StaticObject> staticObjects;
    QTimer *bulletTimer;
    QTimer *playerTimer;
    int x;
    int attemptsLeft;
    bool moveRight;
    bool moveLeft;
    bool isJumping;
    int jumpHeight;
    int jumpSpeed;
    int jumpMaxHeight;
    static const int blockSize = 30;
    static const int redBlockSize = 20;
    static const int stepSize = 10;
    int originalPlayerY;
    QPushButton *jumpButton;
};

#endif // MAINWINDOW_H
