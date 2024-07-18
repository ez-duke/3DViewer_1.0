#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QColorDialog>
#include <QFileDialog>
#include <QMainWindow>
#include <QMovie>
#include <QPixmap>
#include <QProcess>
#include <QScreen>
#include <QSettings>
#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <cstdio>
#include <locale>

#include "QMessageBox"
#include "QtGifImage/src/gifimage/qgifimage.h"
#include "glwidget.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Main window widget
 *
 * The core widget of the program.
 */
class MainWindow : public QWidget {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  void on_openFile_clicked();

  void on_screenshotJpg_clicked();
  void on_screenshotBmp_clicked();

  void on_startScreencast_clicked();
  void recording_gif_animation();
  void start_gif();

  void on_zoomPlus_clicked();
  void on_zoomMinus_clicked();

  void on_rotateX_valueChanged(int value);
  void on_rotateY_valueChanged(int value);
  void on_rotateZ_valueChanged(int value);

  void on_moveX_valueChanged(int value);
  void on_moveY_valueChanged(int value);
  void on_moveZ_valueChanged(int value);

  void on_parallel_clicked();
  void on_central_clicked();

  void on_none_clicked();
  void on_circle_clicked();
  void on_square_clicked();

  void on_dashed_clicked();
  void on_solid_clicked();

  void on_resetPosition_clicked();

  void on_verticeSize_valueChanged(int value);
  void on_edgeSize_valueChanged(int value);

  void on_vertexColor_clicked();
  void on_bgColor_clicked();
  void on_edgeColor_clicked();

  void on_pushButton_rotate_clicked();
  void on_pushButton_move_clicked();
  void on_pushButton_zoom_clicked();

  void saveSettings();
  void loadSettings();
  void resetSliders();
  void paintButtons();

 private:
  Ui::MainWindow *ui;
  QSettings settings;
  QString gif_filename;
  int fps_count;
  QGifImage *gif;
  QTimer *timer_gif;
  QFile *save_file_gif;

  float rotateX_val_abs = 0.0;
  float rotateY_val_abs = 0.0;
  float rotateZ_val_abs = 0.0;
  float moveX_val_abs = 0.0;
  float moveY_val_abs = 0.0;
  float moveZ_val_abs = 0.0;
};

#endif  // MAINWINDOW_H
