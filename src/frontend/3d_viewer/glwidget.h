#ifndef GLWIDGET_H
#define GLWIDGET_H

#define GL_SILENCE_DEPRECATION
#include <QDebug>
#include <QLabel>  // для отображения названия, количества вершин и граней
#include <QOpenGLWidget>
#include <QTimer>

extern "C" {
#include "../../backend/backend.h"
}

/**
 * @brief OpenGL widget
 *
 * The widget for rendering and drawing the object.
 */
class GLWidget : public QOpenGLWidget {
  Q_OBJECT
 public:
  explicit GLWidget(QWidget *parent = nullptr);

  QLabel *infoLabel;

  FILE *fp = NULL;
  data_t data = {};
  char filename[256] = {};

  GLfloat vertexSize = 5.0;
  GLubyte vertexColorArr[3] = {0, 255, 255};
  enum vertex_t { NOTHING = 0, ROUND, SQUARE } vertexMode = ROUND;

  GLubyte bgColorArr[3] = {51, 51, 51};

  GLfloat edgeWidthVal = 1.0;
  GLubyte edgeColorArr[3] = {0, 255, 255};
  enum edge_t { SOLID = 0, DASHED } edgeMode = SOLID;

  enum projection_t { PARALLEL = 0, CENTRAL } projectionMode = PARALLEL;

  void openFile(const char *filename);

  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);

  void drawVertices();
  void drawOneVertex(float *matrix_);

  void drawFacets();
  void drawOneFacet(size_t index_);

 private:
  QTimer timer;
};

#endif  // GLWIDGET_H
