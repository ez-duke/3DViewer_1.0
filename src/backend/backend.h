#ifndef BACKEND_H
#define BACKEND_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI_VAL 3.141593f
#define OFFSET_LIMIT 100.0f
#define SCALE_LIMIT_MIN -1.0f
#define SCALE_LIMIT_MAX 3.0f

/**
 * @brief Matrix of objects
 *
 * 2-dimension float array matrix with rows and cols of vertices to draw.
 *
 * @param matrix Matrix rows by cols
 * @param rows Number of vertices
 * @param cols Number of vertex coordinates (3)
 */
typedef struct Matrix_ {
  float** matrix;
  size_t rows;
  size_t cols;
} matrix_t;

/**
 * @brief Matrix of facets
 *
 * 2-dimension size_t array of vertices to connect.
 *
 * @param vertices Number of vertices
 * @param numbers_of_vertices_in_facets Array of vertices of the facet to
 * connect
 */
typedef struct Facets_ {
  // динамический массив номеров вершин
  size_t* vertices;
  // количество вершин в полигоне, необходимых для соединения
  size_t numbers_of_vertices_in_facets;
} polygon_t;

/**
 * @brief General matrix
 *
 * Mainly used structure with substructures.
 *
 * @param count_of_vertices Number of vertices
 * @param count_of_facets Number of facets
 * @param obj_matrix 2-dimension float array matrix with rows and cols of
 * vertices to draw
 * @param obj_polygons 2-dimension size_t array of vertices to connect
 * @param highest_vertex The highest vertex for scaling
 * @param lowest_vertex The lowest vertex for scaling
 * @param rightest_vertex The rightest vertex for scaling
 * @param leftest_vertex The leftest vertex for scaling
 */
typedef struct Data {
  // количество вершин
  size_t count_of_vertices;
  // количество полигонов
  size_t count_of_facets;
  // матрица вершин
  matrix_t obj_matrix;
  // динамический массив полигонов
  polygon_t* obj_polygons;

  // наивысшая вершина для масштабирования
  float highest_vertex;
  // наинизшая вершина для масштабирования
  float lowest_vertex;
  // самая правая вершина для масштабирования
  float rightest_vertex;
  // самая левая вершина для масштабирования
  float leftest_vertex;
} data_t;

// -------------------------AFFINE-START-------------------------

// перемещение по оси X
void move_by_ox(matrix_t* A, float offset);
// перемещение по оси Y
void move_by_oy(matrix_t* A, float offset);
// перемещение по оси Z
void move_by_oz(matrix_t* A, float offset);
// ограничение значения перемещения
float limit_offset(float offset);

// поворот на заданный угол по оси X
void rotate_by_ox(matrix_t* A, float angle);
// поворот на заданный угол по оси Y
void rotate_by_oy(matrix_t* A, float angle);
// поворот на заданный угол по оси Z
void rotate_by_oz(matrix_t* A, float angle);
// перевод градусов в радианы
float deg_to_rad(float angle);

// изменение масштаба фигуры по всем осям
void scale_even(matrix_t* A, float scale);
// изменение масштаба фигуры по оси X
void scale_by_ox(matrix_t* A, float scale);
// изменение масштаба фигуры по оси Y
void scale_by_oy(matrix_t* A, float scale);
// изменение масштаба фигуры по оси Z
void scale_by_oz(matrix_t* A, float scale);
// ограничение значения масштабирования
float limit_scale(float scale);

// -------------------------PARSER-START-------------------------

FILE* open_obj_file(const char* filename);
void count_vertices_and_facets(FILE* file, data_t* data);
int initialize_obj_matrix(data_t* data);
int matrix_mem_alloc(data_t* data);
void copy_vertices_from_obj_to_matrix(FILE* file, data_t* data);
int count_vertices_in_facets(FILE* file, data_t* data);
void copy_indexes_from_obj_to_struct(FILE* file, data_t* data);
void free_memory(FILE* file, data_t* data);

#endif
