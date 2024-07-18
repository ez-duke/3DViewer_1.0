#include "backend.h"

/**
 * @brief Open file
 *
 * Opens .obj file.
 *
 * @param filename Name of the object to be opened
 */
FILE* open_obj_file(const char* filename) {
  FILE* file = fopen(filename, "r");
  return file;
}

/**
 * @brief Count vertices and facets
 *
 * Counts vertices and facets from file and writes it to data.
 *
 * @param file Opened file pointer
 * @param data Data structure with all parameters
 */
void count_vertices_and_facets(FILE* file, data_t* data) {
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    if (line[0] == 'v' && line[1] != 'n' && line[1] != 't')
      data->count_of_vertices++;
    if (line[0] == 'f') data->count_of_facets++;
  }
}

/**
 * @brief Initialize object matrix
 *
 * Initializes object matrix and allocates memory for it.
 *
 * @param data Data structure with all parameters
 */
int initialize_obj_matrix(data_t* data) {
  data->obj_matrix.rows = data->count_of_vertices;
  data->obj_matrix.cols = 3;
  int error_code = 0;
  error_code = matrix_mem_alloc(data);

  return error_code;
}

/**
 * @brief Allocate memory for object matrix
 *
 * Allocates memory for object matrix.
 *
 * @param data Data structure with all parameters
 */
int matrix_mem_alloc(data_t* data) {
  int error_code = 0;

  data->obj_matrix.matrix = calloc(data->obj_matrix.rows, sizeof(float*));

  if (data->obj_matrix.matrix != NULL) {
    for (size_t i = 0; i < data->obj_matrix.rows; i++) {
      data->obj_matrix.matrix[i] = calloc(data->obj_matrix.cols, sizeof(float));

      if (data->obj_matrix.matrix[i] == NULL) {
        error_code = 1;
        for (size_t k = 0; k < i; k++) {
          free(data->obj_matrix.matrix[k]);
        }
        free(data->obj_matrix.matrix);
        data->obj_matrix.matrix = NULL;
      }
    }
  } else {
    error_code = 1;
  }

  return error_code;
}

/**
 * @brief Copy vertices from object to matrix
 *
 * Copies vertices from object file to object matrix.
 *
 * @param file Opened file pointer
 * @param data Data structure with all parameters
 */
void copy_vertices_from_obj_to_matrix(FILE* file, data_t* data) {
  char line[256];
  size_t v_lines_counter = 0;
  data->rightest_vertex = 0.0f;
  data->leftest_vertex = 0.0f;
  data->highest_vertex = 0.0f;
  data->lowest_vertex = 0.0f;

  while (fgets(line, sizeof(line), file)) {
    if (line[0] == 'v' && line[1] != 'n' && line[1] != 't') {
      sscanf(line, "%*s %f %f %f\n",
             &data->obj_matrix.matrix[v_lines_counter][0],
             &data->obj_matrix.matrix[v_lines_counter][1],
             &data->obj_matrix.matrix[v_lines_counter][2]);

      // вычисление граничных значений для масштабирования
      if (data->obj_matrix.matrix[v_lines_counter][0] > data->rightest_vertex) {
        data->rightest_vertex = data->obj_matrix.matrix[v_lines_counter][0];
      } else if (data->obj_matrix.matrix[v_lines_counter][0] <
                 data->leftest_vertex) {
        data->leftest_vertex = data->obj_matrix.matrix[v_lines_counter][0];
      }

      if (data->obj_matrix.matrix[v_lines_counter][1] > data->highest_vertex) {
        data->highest_vertex = data->obj_matrix.matrix[v_lines_counter][1];
      } else if (data->obj_matrix.matrix[v_lines_counter][1] <
                 data->lowest_vertex) {
        data->lowest_vertex = data->obj_matrix.matrix[v_lines_counter][1];
      }

      v_lines_counter++;
    }
  }
}

/**
 * @brief Count vertices in facets
 *
 * Counts vertices in facets from file and writes it to data.
 *
 * @param file Opened file pointer
 * @param data Data structure with all parameters
 */
int count_vertices_in_facets(FILE* file, data_t* data) {
  int error_code = 0;
  data->obj_polygons = calloc((data->count_of_facets), sizeof(polygon_t));

  if (data->obj_polygons != NULL) {
    size_t f_lines_counter = 0;
    char line[256];
    int count_of_vertices = 0;

    while (fgets(line, sizeof(line), file)) {
      if (line[0] == 'f') {
        char* token = strtok(line, " ");
        while (token != NULL) {
          size_t vertex = 0;
          if (sscanf(token, "%zu", &vertex) == 1) {
            count_of_vertices++;
          }
          token = strtok(NULL, " ");
        }
        data->obj_polygons[f_lines_counter].numbers_of_vertices_in_facets =
            count_of_vertices;
        count_of_vertices = 0;
        f_lines_counter++;
      }
    }

    for (size_t i = 0; i < data->count_of_facets; i++) {
      data->obj_polygons[i].vertices = calloc(
          data->obj_polygons[i].numbers_of_vertices_in_facets, sizeof(size_t));

      if (data->obj_polygons[i].vertices == NULL) {
        error_code = 1;
        for (size_t k = 0; k < i; k++) {
          free(data->obj_polygons[k].vertices);
        }
        free(data->obj_polygons);
        data->obj_polygons = NULL;
      }
    }
  } else {
    error_code = 1;
  }
  return error_code;
}

/**
 * @brief Copy indexes from object to struct
 *
 * Copies indexes from object file to polygon array.
 *
 * @param file Opened file pointer
 * @param data Data structure with all parameters
 */
void copy_indexes_from_obj_to_struct(FILE* file, data_t* data) {
  size_t f_lines_counter = 0;
  char line[256];
  int counter = 0;

  while (fgets(line, sizeof(line), file)) {
    if (line[0] == 'f') {
      char* token = strtok(line, " ");
      while (token != NULL) {
        size_t vertex = 0;
        if (sscanf(token, "%zu", &vertex) == 1) {
          data->obj_polygons[f_lines_counter].vertices[counter] = vertex;
          counter++;
        }
        token = strtok(NULL, " ");
      }
      counter = 0;

      f_lines_counter++;
    }
  }
}

/**
 * @brief Free memory
 *
 * Frees allocated memory and closes file pointer.
 *
 * @param file Opened file pointer
 * @param data Data structure with all parameters
 */
void free_memory(FILE* file, data_t* data) {
  // закрытие файла
  if (file != NULL) {
    fclose(file);
    file = NULL;
  }

  // очистка матрицы вершин
  if (data->obj_matrix.matrix != NULL) {
    for (size_t i = 0; i < data->obj_matrix.rows; i++) {
      free(data->obj_matrix.matrix[i]);
      data->obj_matrix.matrix[i] = NULL;
    }
    free(data->obj_matrix.matrix);
    data->obj_matrix.matrix = NULL;
  }

  // очистка матрицы полигонов
  if (data->obj_polygons != NULL) {
    for (size_t i = 0; i < data->count_of_facets; i++) {
      free(data->obj_polygons[i].vertices);
      data->obj_polygons[i].vertices = NULL;
    }
    free(data->obj_polygons);
    data->obj_polygons = NULL;
  }

  data->count_of_vertices = 0;
  data->count_of_facets = 0;
}
