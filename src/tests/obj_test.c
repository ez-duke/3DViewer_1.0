#include "tests.h"

// gcc -Wall -Werror -Wextra -std=c11 obj_file_work.c tests.c test_obj.c -lcheck
// && ./a.out

START_TEST(obj_test1) {
  {
    const char* filename = "tests/test.obj";
    FILE* file = open_obj_file(filename);
    ck_assert_ptr_nonnull(file);
    fclose(file);
  }

  {
    const char* filename = "wrong_file.obj";
    FILE* file = open_obj_file(filename);
    ck_assert_ptr_null(file);
  }
}

START_TEST(obj_test2) {
  data_t data;
  data.count_of_vertices = 0;
  data.count_of_facets = 0;
  FILE* file = fopen("tests/test.obj", "r");
  count_vertices_and_facets(file, &data);
  ck_assert_int_eq(data.count_of_vertices, 12);
  ck_assert_int_eq(data.count_of_facets, 6);
  fclose(file);
}

START_TEST(obj_test3) {
  data_t test_data;
  test_data.count_of_vertices = 5;
  initialize_obj_matrix(&test_data);
  ck_assert(test_data.obj_matrix.matrix != NULL);
  ck_assert(test_data.obj_matrix.rows == test_data.count_of_vertices);
  ck_assert(test_data.obj_matrix.cols == 3);
  for (unsigned int i = 0; i < test_data.obj_matrix.rows; i++) {
    ck_assert(test_data.obj_matrix.matrix[i] != NULL);
  }
  for (unsigned int i = 0; i < test_data.obj_matrix.rows; ++i) {
    free(test_data.obj_matrix.matrix[i]);
  }
  free(test_data.obj_matrix.matrix);
}

START_TEST(obj_test4) {
  data_t data;
  data.count_of_vertices = 0;
  data.count_of_facets = 0;
  FILE* file = fopen("tests/test.obj", "r");
  count_vertices_and_facets(file, &data);
  initialize_obj_matrix(&data);
  rewind(file);
  copy_vertices_from_obj_to_matrix(file, &data);
  rewind(file);
  char line[256];
  int v_lines_counter = 0;
  double x = 0, y = 0, z = 0;
  while (fgets(line, sizeof(line), file)) {
    if (line[0] == 'v' && line[1] != 'n' && line[1] != 't') {
      sscanf(line, "%*s %lf %lf %lf\n", &x, &y, &z);
      ck_assert_float_eq(data.obj_matrix.matrix[v_lines_counter][0], x);
      ck_assert_float_eq(data.obj_matrix.matrix[v_lines_counter][1], y);
      ck_assert_float_eq(data.obj_matrix.matrix[v_lines_counter][2], z);
      v_lines_counter++;
    }
  }
  fclose(file);
  for (unsigned int i = 0; i < data.obj_matrix.rows; ++i) {
    free(data.obj_matrix.matrix[i]);
  }
  free(data.obj_matrix.matrix);
}

START_TEST(obj_test5) {
  data_t data;
  data.count_of_vertices = 0;
  data.count_of_facets = 0;
  FILE* file = fopen("tests/test.obj", "r");
  count_vertices_and_facets(file, &data);
  initialize_obj_matrix(&data);
  rewind(file);
  copy_vertices_from_obj_to_matrix(file, &data);
  rewind(file);
  count_vertices_in_facets(file, &data);
  rewind(file);
  ck_assert(data.obj_polygons != NULL);
  int f_lines_counter = 0;
  char line[256];
  unsigned int count_of_vertices = 0;
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
      ck_assert(
          data.obj_polygons[f_lines_counter].numbers_of_vertices_in_facets ==
          count_of_vertices);
      count_of_vertices = 0;
      f_lines_counter++;
    }
  }
  for (unsigned int i = 0; i < data.count_of_facets; i++) {
    ck_assert(data.obj_polygons[i].vertices != NULL);
  }
  free_memory(file, &data);
}

START_TEST(obj_test6) {
  data_t data;
  data.count_of_vertices = 0;
  data.count_of_facets = 0;
  FILE* file = fopen("tests/test.obj", "r");
  count_vertices_and_facets(file, &data);
  rewind(file);
  initialize_obj_matrix(&data);
  copy_vertices_from_obj_to_matrix(file, &data);
  rewind(file);
  count_vertices_in_facets(file, &data);
  rewind(file);
  copy_indexes_from_obj_to_struct(file, &data);
  rewind(file);
  int f_lines_counter = 0;
  char line[256];
  int counter = 0;
  while (fgets(line, sizeof(line), file)) {
    if (line[0] == 'f') {
      char* token = strtok(line, " ");
      while (token != NULL) {
        size_t vertex = 0;
        if (sscanf(token, "%zu", &vertex) == 1) {
          ck_assert(data.obj_polygons[f_lines_counter].vertices[counter] ==
                    vertex);
          counter++;
        }
        token = strtok(NULL, " ");
      }
      counter = 0;
      f_lines_counter++;
    }
  }
  free_memory(file, &data);
}

START_TEST(obj_test7) {
  data_t data;
  data.count_of_vertices = 0;
  data.count_of_facets = 0;
  FILE* file = fopen("tests/test.obj", "r");
  count_vertices_and_facets(file, &data);
  initialize_obj_matrix(&data);
  copy_vertices_from_obj_to_matrix(file, &data);
  count_vertices_in_facets(file, &data);
  copy_indexes_from_obj_to_struct(file, &data);
  free_memory(file, &data);
  ck_assert_ptr_null(data.obj_matrix.matrix);
}

Suite* obj_test_suite() {
  Suite* suite = suite_create("obj_test");
  TCase* tcase = tcase_create("obj_tests_case");

  tcase_add_test(tcase, obj_test1);
  tcase_add_test(tcase, obj_test2);
  tcase_add_test(tcase, obj_test3);
  tcase_add_test(tcase, obj_test4);
  tcase_add_test(tcase, obj_test5);
  tcase_add_test(tcase, obj_test6);
  tcase_add_test(tcase, obj_test7);

  suite_add_tcase(suite, tcase);

  return suite;
}

int obj_test() {
  Suite* suite = obj_test_suite();
  SRunner* srunner = srunner_create(suite);

  srunner_set_fork_status(srunner, CK_NOFORK);
  srunner_run_all(srunner, CK_NORMAL);
  int failed = srunner_ntests_failed(srunner);
  srunner_free(srunner);

  return failed;
}
