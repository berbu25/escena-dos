#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QMessageBox>
#include <QVector>
#include <QRect>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updatePositions();
    void updatePlayerPosition();

private:
    struct Block {
        int x;
        int y;
        int dx;
        int dy;
        bool isParabolic;
    };

    void initializeRedBlocks();
    bool checkCollision(const Block &block);
    void resetBlueBlock();
    void endGame();

    QVector<Block> redBlocks;
    QTimer *bulletTimer;
    QTimer *playerTimer;
    int x;
    int attemptsLeft;
    bool moveRight;
    bool moveLeft;
    static const int blockSize = 30;
    static const int redBlockSize = 20;
    static const int stepSize = 10;
};

#endif // MAINWINDOW_H
