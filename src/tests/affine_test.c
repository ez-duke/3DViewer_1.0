#include "tests.h"

START_TEST(affine_test0) {
  data_t data = {.obj_matrix.rows = 3, .obj_matrix.cols = 3};
  matrix_mem_alloc(&data);
  ck_assert_ptr_nonnull(&data);

  data.obj_matrix.matrix[1][0] = 1;
  move_by_ox(&data.obj_matrix, 3);
  ck_assert_float_le(data.obj_matrix.matrix[1][0], 4);

  data.obj_matrix.matrix[1][1] = 2;
  move_by_oy(&data.obj_matrix, -3);
  ck_assert_float_le(data.obj_matrix.matrix[1][1], -1);

  data.obj_matrix.matrix[1][2] = 3;
  move_by_oz(&data.obj_matrix, 3);
  ck_assert_float_le(data.obj_matrix.matrix[1][2], 6);

  data.obj_matrix.matrix[1][0] = 4;
  move_by_ox(&data.obj_matrix, OFFSET_LIMIT * 3);
  ck_assert_float_le(data.obj_matrix.matrix[1][0], 4 + OFFSET_LIMIT);

  data.obj_matrix.matrix[1][1] = 5;
  move_by_oy(&data.obj_matrix, -OFFSET_LIMIT * 2.5);
  ck_assert_float_le(data.obj_matrix.matrix[1][1], 5 - OFFSET_LIMIT);

  free_memory(NULL, &data);
}

START_TEST(affine_test1) {
  data_t data = {.obj_matrix.rows = 3, .obj_matrix.cols = 3};
  matrix_mem_alloc(&data);
  ck_assert_ptr_nonnull(&data);

  data.obj_matrix.matrix[0][1] = 2;
  data.obj_matrix.matrix[0][2] = 3;
  rotate_by_ox(&data.obj_matrix, 15);
  ck_assert_float_le(fabs(data.obj_matrix.matrix[0][1] - 1.155395), 1e-6);
  ck_assert_float_le(fabs(data.obj_matrix.matrix[0][2] - 3.415416), 1e-6);

  data.obj_matrix.matrix[1][0] = -3;
  data.obj_matrix.matrix[1][2] = 1;
  rotate_by_oy(&data.obj_matrix, 167);
  ck_assert_float_le(fabs(data.obj_matrix.matrix[1][0] - 3.148061), 1e-6);
  ck_assert_float_le(fabs(data.obj_matrix.matrix[1][2] + 0.299518), 1e-6);

  data.obj_matrix.matrix[2][0] = 2;
  data.obj_matrix.matrix[2][1] = 0;
  rotate_by_oz(&data.obj_matrix, -36);
  ck_assert_float_le(fabs(data.obj_matrix.matrix[2][0] - 1.618034), 1e-6);
  ck_assert_float_le(fabs(data.obj_matrix.matrix[2][1] + 1.175571), 1e-6);

  free_memory(NULL, &data);
}

START_TEST(affine_test2) {
  data_t data = {.obj_matrix.rows = 3, .obj_matrix.cols = 3};
  matrix_mem_alloc(&data);
  ck_assert_ptr_nonnull(&data);

  data.obj_matrix.matrix[1][0] = 1;
  scale_by_ox(&data.obj_matrix, 3);
  ck_assert_float_le(data.obj_matrix.matrix[1][0], 3);

  data.obj_matrix.matrix[1][1] = 2;
  scale_by_oy(&data.obj_matrix, 2);
  ck_assert_float_le(data.obj_matrix.matrix[1][1], 4);

  data.obj_matrix.matrix[1][2] = 3;
  scale_by_oz(&data.obj_matrix, SCALE_LIMIT_MIN * 10);
  ck_assert_float_le(data.obj_matrix.matrix[1][2], 3 * SCALE_LIMIT_MIN);

  data.obj_matrix.matrix[1][0] = -4;
  scale_by_ox(&data.obj_matrix, SCALE_LIMIT_MAX * 5);
  ck_assert_float_le(data.obj_matrix.matrix[1][0], -4 * SCALE_LIMIT_MAX);

  free_memory(NULL, &data);
}

START_TEST(affine_test3) {
  data_t data = {.obj_matrix.rows = 3, .obj_matrix.cols = 3};
  matrix_mem_alloc(&data);
  ck_assert_ptr_nonnull(&data);

  data.obj_matrix.matrix[1][0] = 4;
  data.obj_matrix.matrix[1][1] = 5;
  data.obj_matrix.matrix[1][2] = -2;
  scale_even(&data.obj_matrix, 2.3);
  ck_assert_float_le(data.obj_matrix.matrix[1][0], 9.2);
  ck_assert_float_le(data.obj_matrix.matrix[1][1], 11.5);
  ck_assert_float_le(data.obj_matrix.matrix[1][2], -4.6);

  free_memory(NULL, &data);
}

Suite *affine_test_suite() {
  Suite *suite = suite_create("affine_test");
  TCase *tcase = tcase_create("affine_test_case");

  tcase_add_test(tcase, affine_test0);
  tcase_add_test(tcase, affine_test1);
  tcase_add_test(tcase, affine_test2);
  tcase_add_test(tcase, affine_test3);

  suite_add_tcase(suite, tcase);

  return suite;
}

int affine_tests() {
  Suite *suite = affine_test_suite();
  SRunner *srunner = srunner_create(suite);

  srunner_set_fork_status(srunner, CK_NOFORK);
  srunner_run_all(srunner, CK_NORMAL);
  int failed = srunner_ntests_failed(srunner);
  srunner_free(srunner);

  return failed;
}
