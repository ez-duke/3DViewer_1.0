#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  loadSettings();
}

MainWindow::~MainWindow() {
  saveSettings();
  free_memory(ui->openGLWidget->fp, &ui->openGLWidget->data);
  delete ui;
}

/**
 * @brief Open a file event
 *
 * Event which happens when openFile button is clicked.
 */
void MainWindow::on_openFile_clicked() {
  // открытие диалогового окна с заданными фильтрами для выбора файла
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open .obj file"), "../../../objects/",
      tr("Object Files (*.obj);;All Files (*)"));

  // перевод Qstring в const char*
  QByteArray ba = fileName.toLocal8Bit();
  const char *c_filename = ba.data();

  // вызов функции открытия файла, если выбран конкретный файл
  if (c_filename[0] != '\0') {
    resetSliders();
    paintButtons();
    strcpy(ui->openGLWidget->filename, c_filename);

    // открытие файла
    ui->openGLWidget->openFile(c_filename);
  }
}

/**
 * @brief Create a .jpg image event
 *
 * Event which happens when screenshotJpg button is clicked.
 */
void MainWindow::on_screenshotJpg_clicked() {
  QWidget *MainWindow = this;
  QScreen *screen = QGuiApplication::primaryScreen();
  QPixmap screenshot = screen->grabWindow(MainWindow->winId());
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Screenshot"),
                                                  "", tr("Images (*.jpg)"));
  if (!fileName.isEmpty()) {
    screenshot.save(fileName, "jpg");
  }
}

/**
 * @brief Create a .bmp image event
 *
 * Event which happens when screenshotBmp button is clicked.
 */
void MainWindow::on_screenshotBmp_clicked() {
  QWidget *MainWindow = this;
  QScreen *screen = QGuiApplication::primaryScreen();
  QPixmap screenshot = screen->grabWindow(MainWindow->winId());
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Screenshot"),
                                                  "", tr("Images (*.bmp)"));
  if (!fileName.isEmpty()) {
    screenshot.save(fileName, "bmp");
  }
}

/**
 * @brief Create a .gif image event
 *
 * Event which happens when startScreencast button is clicked.
 */
void MainWindow::on_startScreencast_clicked() {
  gif_filename = QFileDialog::getSaveFileName(
      this, "save gif animation",
      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
      "*.gif");
  save_file_gif = new QFile(gif_filename);
  if (save_file_gif->open(QIODevice::WriteOnly)) {
    start_gif();
  } else {
    QMessageBox::critical(this, "error", "Gif animation not saved");
  }
}

/**
 * @brief Prepare to record gif
 *
 * Initialize gif with fixed resolution and timer.
 */
void MainWindow::start_gif() {
  gif = new QGifImage(QSize(640, 480));
  timer_gif = new QTimer(this);
  connect(timer_gif, &QTimer::timeout, this,
          &MainWindow::recording_gif_animation);
  timer_gif->start(100);
  fps_count = 0;
}

/**
 * @brief Record gif
 *
 * Happens after gif has been initialized.
 *
 * The record lasts 5 seconds.
 */
void MainWindow::recording_gif_animation() {
  const QRect rect(0, 0, ui->openGLWidget->width(), ui->openGLWidget->height());
  QPixmap pixmap = ui->openGLWidget->grab(rect);

  ui->startScreencast->setText("Recording Gif (" +
                               QString::number(fps_count / 10) + " / 5 sec)");

  QImage myImage = pixmap.scaled(640, 480).toImage();
  fps_count++;
  gif->addFrame(myImage, 100);
  if (fps_count == 50) {
    timer_gif->stop();
    gif->save(save_file_gif);
    save_file_gif->close();
    delete save_file_gif;
    save_file_gif = nullptr;
    delete gif;
    gif = nullptr;
    QMessageBox::information(this, "success", "Gif success saved");
    ui->startScreencast->setText("start");
  }
}

/**
 * @brief Save settings
 *
 * Saves 3DViewer image parameters on exit.
 *
 * Saved parameters are: Colors, size and form of vertices and edges (and
 * background color).
 */
void MainWindow::saveSettings() {
  settings.setValue("projectionMode", ui->openGLWidget->projectionMode);
  settings.setValue("vertexMode", ui->openGLWidget->vertexMode);
  settings.setValue("edgeMode", ui->openGLWidget->edgeMode);
  settings.setValue("vertexSize", ui->openGLWidget->vertexSize);
  settings.setValue("edgeWidth", ui->openGLWidget->edgeWidthVal);

  settings.setValue("vertexColorR", ui->openGLWidget->vertexColorArr[0]);
  settings.setValue("vertexColorG", ui->openGLWidget->vertexColorArr[1]);
  settings.setValue("vertexColorB", ui->openGLWidget->vertexColorArr[2]);

  settings.setValue("bgColorR", ui->openGLWidget->bgColorArr[0]);
  settings.setValue("bgColorG", ui->openGLWidget->bgColorArr[1]);
  settings.setValue("bgColorB", ui->openGLWidget->bgColorArr[2]);

  settings.setValue("edgeColorR", ui->openGLWidget->edgeColorArr[0]);
  settings.setValue("edgeColorG", ui->openGLWidget->edgeColorArr[1]);
  settings.setValue("edgeColorB", ui->openGLWidget->edgeColorArr[2]);
}

/**
 * @brief Load settings
 *
 * Loads 3DViewer image parameters on exit.
 *
 * Loaded parameters are: Colors, size and form of vertices and edges (and
 * background color).
 */
void MainWindow::loadSettings() {
  ui->openGLWidget->projectionMode = static_cast<GLWidget::projection_t>(
      settings.value("projectionMode").toInt());
  if (ui->openGLWidget->projectionMode == GLWidget::PARALLEL) {
    ui->parallel->setChecked(true);
  } else {
    ui->central->setChecked(true);
  }

  ui->openGLWidget->vertexMode =
      static_cast<GLWidget::vertex_t>(settings.value("vertexMode").toInt());
  if (ui->openGLWidget->vertexMode == GLWidget::ROUND) {
    ui->circle->setChecked(true);
  } else if (ui->openGLWidget->vertexMode == GLWidget::SQUARE) {
    ui->square->setChecked(true);
  } else {
    ui->none->setChecked(true);
  }

  ui->openGLWidget->edgeMode =
      static_cast<GLWidget::edge_t>(settings.value("edgeMode").toInt());
  if (ui->openGLWidget->edgeMode == GLWidget::SOLID) {
    ui->solid->setChecked(true);
  } else {
    ui->dashed->setChecked(true);
  }

  ui->verticeSize->setValue(settings.value("vertexSize").toFloat() * 20);
  ui->edgeSize->setValue(settings.value("edgeWidth").toFloat());

  ui->openGLWidget->vertexColorArr[0] = settings.value("vertexColorR").toUInt();
  ui->openGLWidget->vertexColorArr[1] = settings.value("vertexColorG").toUInt();
  ui->openGLWidget->vertexColorArr[2] = settings.value("vertexColorB").toUInt();

  ui->openGLWidget->bgColorArr[0] = settings.value("bgColorR").toUInt();
  ui->openGLWidget->bgColorArr[1] = settings.value("bgColorG").toUInt();
  ui->openGLWidget->bgColorArr[2] = settings.value("bgColorB").toUInt();

  ui->openGLWidget->edgeColorArr[0] = settings.value("edgeColorR").toUInt();
  ui->openGLWidget->edgeColorArr[1] = settings.value("edgeColorG").toUInt();
  ui->openGLWidget->edgeColorArr[2] = settings.value("edgeColorB").toUInt();
  paintButtons();
}

/**
 * @brief Paint buttons
 *
 * Paints color buttons with corresponding colors.
 *
 * Colored buttons are for vertex, background and edge colors.
 */
void MainWindow::paintButtons() {
  char rgb_color[40];
  snprintf(rgb_color, 40, "background-color: rgb(%d,%d,%d)",
           ui->openGLWidget->vertexColorArr[0],
           ui->openGLWidget->vertexColorArr[1],
           ui->openGLWidget->vertexColorArr[2]);
  ui->vertexColor->setStyleSheet(rgb_color);

  snprintf(rgb_color, 40, "background-color: rgb(%d,%d,%d)",
           ui->openGLWidget->bgColorArr[0], ui->openGLWidget->bgColorArr[1],
           ui->openGLWidget->bgColorArr[2]);
  ui->bgColor->setStyleSheet(rgb_color);

  snprintf(rgb_color, 40, "background-color: rgb(%d,%d,%d)",
           ui->openGLWidget->edgeColorArr[0], ui->openGLWidget->edgeColorArr[1],
           ui->openGLWidget->edgeColorArr[2]);
  ui->edgeColor->setStyleSheet(rgb_color);
}

/**
 * @brief Scale input field
 *
 * Happens when scale input is applied.
 */
void MainWindow::on_pushButton_zoom_clicked() {
  float zoom_line = ui->edit_zoom->text().toFloat();

  scale_even(&ui->openGLWidget->data.obj_matrix, zoom_line);
  ui->openGLWidget->update();
}

/**
 * @brief Scale increase button
 *
 * Happens when scale+ button is pressed.
 */
void MainWindow::on_zoomPlus_clicked() {
  scale_even(&ui->openGLWidget->data.obj_matrix, 1.1111f);
  ui->openGLWidget->update();
}

/**
 * @brief Scale decrease button
 *
 * Happens when scale- button is pressed.
 */
void MainWindow::on_zoomMinus_clicked() {
  scale_even(&ui->openGLWidget->data.obj_matrix, 0.9f);
  ui->openGLWidget->update();
}

/**
 * @brief Rotate input field
 *
 * Happens when rotate input is applied.
 */
void MainWindow::on_pushButton_rotate_clicked() {
  float x_rotate_line = ui->edit_rotateX->text().toFloat();
  float y_rotate_line = ui->edit_rotateY->text().toFloat();
  float z_rotate_line = ui->edit_rotateZ->text().toFloat();

  ui->rotateX->setValue(rotateX_val_abs + x_rotate_line);
  ui->rotateY->setValue(rotateY_val_abs + y_rotate_line);
  ui->rotateZ->setValue(rotateZ_val_abs + z_rotate_line);
}

/**
 * @brief Rotate around OX slider
 *
 * This is the logic of rotateX slider.
 */
void MainWindow::on_rotateX_valueChanged(int value) {
  rotate_by_ox(&ui->openGLWidget->data.obj_matrix, rotateX_val_abs - value);
  rotateX_val_abs = value;
  ui->openGLWidget->update();
}

/**
 * @brief Rotate around OY slider
 *
 * This is the logic of rotateY slider.
 */
void MainWindow::on_rotateY_valueChanged(int value) {
  rotate_by_oy(&ui->openGLWidget->data.obj_matrix, rotateY_val_abs - value);
  rotateY_val_abs = value;
  ui->openGLWidget->update();
}

/**
 * @brief Rotate around OZ slider
 *
 * This is the logic of rotateZ slider.
 */
void MainWindow::on_rotateZ_valueChanged(int value) {
  rotate_by_oz(&ui->openGLWidget->data.obj_matrix, rotateZ_val_abs - value);
  rotateZ_val_abs = value;
  ui->openGLWidget->update();
}

/**
 * @brief Move input field
 *
 * Happens when move input is applied.
 */
void MainWindow::on_pushButton_move_clicked() {
  float x_move_line = ui->edit_moveX->text().toDouble();
  float y_move_line = ui->edit_moveY->text().toDouble();
  float z_move_line = ui->edit_moveZ->text().toDouble();

  ui->moveX->setValue(moveX_val_abs + x_move_line);
  ui->moveY->setValue(moveY_val_abs + y_move_line);
  ui->moveZ->setValue(moveZ_val_abs + z_move_line);
}

/**
 * @brief Move by OX slider
 *
 * This is the logic of moveX slider.
 */
void MainWindow::on_moveX_valueChanged(int value) {
  move_by_ox(&ui->openGLWidget->data.obj_matrix,
             (value - moveX_val_abs) * 0.01f);
  moveX_val_abs = value;
  ui->openGLWidget->update();
}

/**
 * @brief Move by OY slider
 *
 * This is the logic of moveY slider.
 */
void MainWindow::on_moveY_valueChanged(int value) {
  move_by_oy(&ui->openGLWidget->data.obj_matrix,
             (value - moveY_val_abs) * 0.01f);
  moveY_val_abs = value;
  ui->openGLWidget->update();
}

/**
 * @brief Move by OZ slider
 *
 * This is the logic of moveZ slider.
 */
void MainWindow::on_moveZ_valueChanged(int value) {
  move_by_oz(&ui->openGLWidget->data.obj_matrix,
             (value - moveZ_val_abs) * 0.01f);
  moveZ_val_abs = value;
  ui->openGLWidget->update();
}

/**
 * @brief Set parallel projection
 *
 * This happens when radio button parallel is pressed.
 */
void MainWindow::on_parallel_clicked() {
  ui->openGLWidget->projectionMode = GLWidget::PARALLEL;
  ui->openGLWidget->update();
}

/**
 * @brief Set central projection
 *
 * This happens when radio button central is pressed.
 */
void MainWindow::on_central_clicked() {
  ui->openGLWidget->projectionMode = GLWidget::CENTRAL;
  ui->openGLWidget->update();
}

/**
 * @brief Draw no vertice
 *
 * This happens when radio button none is pressed.
 */
void MainWindow::on_none_clicked() {
  ui->openGLWidget->vertexMode = GLWidget::NOTHING;
  ui->openGLWidget->update();
}

/**
 * @brief Draw circle vertice
 *
 * This happens when radio button circle is pressed.
 */
void MainWindow::on_circle_clicked() {
  ui->openGLWidget->vertexMode = GLWidget::ROUND;
  ui->openGLWidget->update();
}

/**
 * @brief Draw square vertice
 *
 * This happens when radio button square is pressed.
 */
void MainWindow::on_square_clicked() {
  ui->openGLWidget->vertexMode = GLWidget::SQUARE;
  ui->openGLWidget->update();
}

/**
 * @brief Draw dashed edges
 *
 * This happens when radio button dashed is pressed.
 */
void MainWindow::on_dashed_clicked() {
  ui->openGLWidget->edgeMode = GLWidget::DASHED;
  ui->openGLWidget->update();
}

/**
 * @brief Draw normal edges
 *
 * This happens when radio button solid is pressed.
 */
void MainWindow::on_solid_clicked() {
  ui->openGLWidget->edgeMode = GLWidget::SOLID;
  ui->openGLWidget->update();
}

/**
 * @brief Reset object position
 *
 * Loads the object again hence resetting any changes to its position.
 */
void MainWindow::on_resetPosition_clicked() {
  resetSliders();
  ui->openGLWidget->openFile(ui->openGLWidget->filename);
}

/**
 * @brief Reset slider positions
 *
 * Resets sliders to its normal positions.
 */
void MainWindow::resetSliders() {
  ui->moveX->setValue(0);
  ui->moveY->setValue(0);
  ui->moveZ->setValue(0);
  ui->rotateX->setValue(0);
  ui->rotateY->setValue(0);
  ui->rotateZ->setValue(0);
}

/**
 * @brief Set vertice size
 *
 * This is the logic of verticeSize slider.
 */
void MainWindow::on_verticeSize_valueChanged(int value) {
  ui->openGLWidget->vertexSize = value * 0.05f;
  ui->openGLWidget->update();
}

/**
 * @brief Set edge size
 *
 * This is the logic of edgeSize slider.
 */
void MainWindow::on_edgeSize_valueChanged(int value) {
  ui->openGLWidget->edgeWidthVal = value;
  ui->openGLWidget->update();
}

/**
 * @brief Set vertice color
 *
 * Calls palette interface to choose vertice color.
 */
void MainWindow::on_vertexColor_clicked() {
  QColor vertexColor = QColorDialog::getColor();
  if (vertexColor.isValid()) {
    ui->openGLWidget->vertexColorArr[0] = vertexColor.red();
    ui->openGLWidget->vertexColorArr[1] = vertexColor.green();
    ui->openGLWidget->vertexColorArr[2] = vertexColor.blue();

    char rgb_color[40];
    snprintf(rgb_color, 40, "background-color: rgb(%d,%d,%d)", vertexColor.red(),
             vertexColor.green(), vertexColor.blue());
    ui->vertexColor->setStyleSheet(rgb_color);

    ui->openGLWidget->update();
  }
}

/**
 * @brief Set background color
 *
 * Calls palette interface to choose background color.
 */
void MainWindow::on_bgColor_clicked() {
  QColor bgColor = QColorDialog::getColor();
  if (bgColor.isValid()) {
    ui->openGLWidget->bgColorArr[0] = bgColor.red();
    ui->openGLWidget->bgColorArr[1] = bgColor.green();
    ui->openGLWidget->bgColorArr[2] = bgColor.blue();

    char rgb_color[40];
    snprintf(rgb_color, 40, "background-color: rgb(%d,%d,%d)", bgColor.red(),
             bgColor.green(), bgColor.blue());
    ui->bgColor->setStyleSheet(rgb_color);

    ui->openGLWidget->update();
  }
}

/**
 * @brief Set edges color
 *
 * Calls palette interface to choose edges color.
 */
void MainWindow::on_edgeColor_clicked() {
  QColor edgeColor = QColorDialog::getColor();
  if (edgeColor.isValid()) {
    ui->openGLWidget->edgeColorArr[0] = edgeColor.red();
    ui->openGLWidget->edgeColorArr[1] = edgeColor.green();
    ui->openGLWidget->edgeColorArr[2] = edgeColor.blue();

    char rgb_color[40];
    snprintf(rgb_color, 40, "background-color: rgb(%d,%d,%d)", edgeColor.red(),
             edgeColor.green(), edgeColor.blue());
    ui->edgeColor->setStyleSheet(rgb_color);

    ui->openGLWidget->update();
  }
}
