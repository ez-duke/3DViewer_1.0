#include "glwidget.h"

// файл для работы с openGLWidget

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget{parent}, infoLabel(new QLabel(this)) {
  // Настройка позиции и размеров QLabel
  infoLabel->setGeometry(10, 10, 700, 50);
  infoLabel->hide();
}

/**
 * @brief Initialize the window
 *
 * Initialize openGL window.
 */
void GLWidget::initializeGL() {}

/**
 * @brief Painting widget
 *
 * Rendering happens here.
 */
void GLWidget::paintGL() {
  glClearColor(bgColorArr[0] / 255.0f, bgColorArr[1] / 255.0f,
               bgColorArr[2] / 255.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (projectionMode == PARALLEL) {
    glOrtho(1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
  } else {
    glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(180, 0, 1, 0);
    glRotatef(-15, 1, 0, 0);
    glScalef(1.2, 1.2, 1.2);
  }

  drawVertices();
  drawFacets();
}

/**
 * @brief Draw vertices
 *
 * Draws vertices of the object.
 */
void GLWidget::drawVertices() {
  if (vertexMode == ROUND) {
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
  } else {  // SQUARE
    glDisable(GL_POINT_SMOOTH);
  }

  glPointSize(vertexSize);
  glColor3ub(vertexColorArr[0], vertexColorArr[1], vertexColorArr[2]);
  glBegin(GL_POINTS);

  for (size_t i = 0; data.obj_matrix.matrix != NULL && vertexMode != NOTHING &&
                     i < data.obj_matrix.rows;
       i++) {
    drawOneVertex(data.obj_matrix.matrix[i]);
  }
  glEnd();
}

/**
 * @brief Draw a vertex
 *
 * Draws one vertex of the object.
 *
 * @param matrix_ Coordinates of a drawn vertex
 */
void GLWidget::drawOneVertex(float *matrix_) {
  glVertex3f(matrix_[0], matrix_[1], matrix_[2]);
}

/**
 * @brief Draw polygons
 *
 * Draws polygons of the object.
 */
void GLWidget::drawFacets() {
  if (edgeMode == DASHED) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xFF00);
  } else {  // SOLID
    glDisable(GL_LINE_STIPPLE);
  }

  glLineWidth(edgeWidthVal);
  glColor3ub(edgeColorArr[0], edgeColorArr[1], edgeColorArr[2]);
  glBegin(GL_LINES);

  for (size_t i = 0; data.obj_polygons != NULL && i < data.count_of_facets;
       i++) {
    drawOneFacet(i);
  }
  glEnd();
}

/**
 * @brief Draw a polygon
 *
 * Draws one polygon of the object.
 *
 * @param index_ Index of a drawn polygon
 */
void GLWidget::drawOneFacet(size_t index_) {
  // соединяем попарно вершины
  size_t i = 0;
  for (; i < data.obj_polygons[index_].numbers_of_vertices_in_facets - 1; i++) {
    glVertex3f(
        data.obj_matrix.matrix[data.obj_polygons[index_].vertices[i] - 1][0],
        data.obj_matrix.matrix[data.obj_polygons[index_].vertices[i] - 1][1],
        data.obj_matrix.matrix[data.obj_polygons[index_].vertices[i] - 1][2]);

    glVertex3f(data.obj_matrix
                   .matrix[data.obj_polygons[index_].vertices[i + 1] - 1][0],
               data.obj_matrix
                   .matrix[data.obj_polygons[index_].vertices[i + 1] - 1][1],
               data.obj_matrix
                   .matrix[data.obj_polygons[index_].vertices[i + 1] - 1][2]);
  }

  // соединяем последнюю и первую вершины
  glVertex3f(
      data.obj_matrix.matrix[data.obj_polygons[index_].vertices[i] - 1][0],
      data.obj_matrix.matrix[data.obj_polygons[index_].vertices[i] - 1][1],
      data.obj_matrix.matrix[data.obj_polygons[index_].vertices[i] - 1][2]);

  glVertex3f(
      data.obj_matrix.matrix[data.obj_polygons[index_].vertices[0] - 1][0],
      data.obj_matrix.matrix[data.obj_polygons[index_].vertices[0] - 1][1],
      data.obj_matrix.matrix[data.obj_polygons[index_].vertices[0] - 1][2]);
}

/**
 * @brief Open a file
 *
 * Opens a file before drawing an object.
 *
 * @param filename Name of the file to be opened
 */
void GLWidget::openFile(const char *filename) {
  if (fp != NULL) {
    free_memory(fp, &data);
  }
  fp = open_obj_file(filename);
  if (fp != NULL) {
    count_vertices_and_facets(fp, &data);
    rewind(fp);
    int error_code = initialize_obj_matrix(&data);
    if (error_code == 0) {
      copy_vertices_from_obj_to_matrix(fp, &data);
      rewind(fp);
      error_code = count_vertices_in_facets(fp, &data);
      if (error_code == 0) {
        rewind(fp);
        copy_indexes_from_obj_to_struct(fp, &data);
        // автомасштабирование
        float init_scale;
        if (fabsf(data.highest_vertex + data.lowest_vertex) < 1e-6)
          init_scale = 1.0f / (fabsf(data.highest_vertex) + 0.1f);
        else
          init_scale = 2.0f / (fabsf(data.lowest_vertex) +
                               fabsf(data.highest_vertex) + 0.1f);
        scale_even(&data.obj_matrix, init_scale);
        // перемещение фигуры в центр
        data.rightest_vertex *= init_scale;
        data.leftest_vertex *= init_scale;
        move_by_ox(
            &data.obj_matrix,
            (fabsf(data.leftest_vertex) - fabsf(data.rightest_vertex)) / 2.0f);
        data.highest_vertex *= init_scale;
        data.lowest_vertex *= init_scale;
        move_by_oy(
            &data.obj_matrix,
            (fabsf(data.lowest_vertex) - fabsf(data.highest_vertex)) / 2.0f);
        update();
        // отображение названия, количества вершин и граней
        QString fileInfo =
            QString(
                "Opened file: %1\nCount of vertices: %2\nCount of facets: %3")
                .arg(filename)
                .arg(data.count_of_vertices)
                .arg(data.count_of_facets);
        infoLabel->setText(fileInfo);  // Установка текста для QLabel
        infoLabel->show();             // Показываем QLabel
      }
    }
  } else {
    qWarning() << "Failed to open file";
  }
}

/**
 * @brief Resize the window
 *
 * Sets the size of openGL window.
 *
 * @param w Window width
 * @param h Window height
 */
void GLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
  GLfloat aspectRatio = (GLfloat)w / (GLfloat)h;
  glOrtho(aspectRatio, -aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
}
