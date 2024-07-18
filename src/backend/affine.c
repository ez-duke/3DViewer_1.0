#include "backend.h"

/**
 * @brief Move by OX
 *
 * Moves the object by OX axis by offset value.
 *
 * @param A Object matrix
 * @param offset Value to move the object by
 */
void move_by_ox(matrix_t *A, float offset) {
  offset = limit_offset(offset);

  for (size_t i = 0; i < A->rows; i++) {
    A->matrix[i][0] += offset;
  }
}

/**
 * @brief Move by OY
 *
 * Moves the object by OY axis by offset value.
 *
 * @param A Object matrix
 * @param offset Value to move the object by
 */
void move_by_oy(matrix_t *A, float offset) {
  offset = limit_offset(offset);

  for (size_t i = 0; i < A->rows; i++) {
    A->matrix[i][1] += offset;
  }
}

/**
 * @brief Move by OZ
 *
 * Moves the object by OZ axis by offset value.
 *
 * @param A Object matrix
 * @param offset Value to move the object by
 */
void move_by_oz(matrix_t *A, float offset) {
  offset = limit_offset(offset);

  for (size_t i = 0; i < A->rows; i++) {
    A->matrix[i][2] += offset;
  }
}

/**
 * @brief Limit offset
 *
 * Limits offset value when moving the object.
 *
 * @param offset Value to move the object by
 */
float limit_offset(float offset) {
  if (offset > OFFSET_LIMIT)
    offset = OFFSET_LIMIT;
  else if (offset < -OFFSET_LIMIT)
    offset = -OFFSET_LIMIT;

  return offset;
}

/**
 * @brief Rotate by OX
 *
 * Rotates the object by OX axis by angle value.
 *
 * @param A Object matrix
 * @param angle Value to rotate the object by
 */
void rotate_by_ox(matrix_t *A, float angle) {
  angle = deg_to_rad(angle);

  for (size_t i = 0; i < A->rows; i++) {
    float temp_y = A->matrix[i][1];
    float temp_z = A->matrix[i][2];
    A->matrix[i][1] = cosf(angle) * temp_y - sinf(angle) * temp_z;
    A->matrix[i][2] = sinf(angle) * temp_y + cosf(angle) * temp_z;
  }
}

/**
 * @brief Rotate by OY
 *
 * Rotates the object by OY axis by angle value.
 *
 * @param A Object matrix
 * @param angle Value to rotate the object by
 */
void rotate_by_oy(matrix_t *A, float angle) {
  angle = deg_to_rad(angle);

  for (size_t i = 0; i < A->rows; i++) {
    float temp_x = A->matrix[i][0];
    float temp_z = A->matrix[i][2];
    A->matrix[i][0] = cosf(angle) * temp_x + sinf(angle) * temp_z;
    A->matrix[i][2] = -sinf(angle) * temp_x + cosf(angle) * temp_z;
  }
}

/**
 * @brief Rotate by OZ
 *
 * Rotates the object by OZ axis by angle value.
 *
 * @param A Object matrix
 * @param angle Value to rotate the object by
 */
void rotate_by_oz(matrix_t *A, float angle) {
  angle = deg_to_rad(angle);

  for (size_t i = 0; i < A->rows; i++) {
    float temp_x = A->matrix[i][0];
    float temp_y = A->matrix[i][1];
    A->matrix[i][0] = cosf(angle) * temp_x - sinf(angle) * temp_y;
    A->matrix[i][1] = sinf(angle) * temp_x + cosf(angle) * temp_y;
  }
}

/**
 * @brief Convert degrees into radians
 *
 * Converts degrees into radians before feeding it to trigonometric functions.
 *
 * @param angle Value to rotate the object by
 */
float deg_to_rad(float angle) {
  angle *= PI_VAL / 180.0;
  return angle;
}

/**
 * @brief Scale by OX OY OZ
 *
 * Scales the object by 3 axes by scale value.
 *
 * @param A Object matrix
 * @param scale Value to scale the object by
 */
void scale_even(matrix_t *A, float scale) {
  scale = limit_scale(scale);

  for (size_t i = 0; i < A->rows; i++) {
    A->matrix[i][0] *= scale;
    A->matrix[i][1] *= scale;
    A->matrix[i][2] *= scale;
  }
}

/**
 * @brief Scale by OX
 *
 * Scales the object by OX axis by scale value.
 *
 * @param A Object matrix
 * @param scale Value to scale the object by
 */
void scale_by_ox(matrix_t *A, float scale) {
  scale = limit_scale(scale);

  for (size_t i = 0; i < A->rows; i++) {
    A->matrix[i][0] *= scale;
  }
}

/**
 * @brief Scale by OY
 *
 * Scales the object by OY axis by scale value.
 *
 * @param A Object matrix
 * @param scale Value to scale the object by
 */
void scale_by_oy(matrix_t *A, float scale) {
  scale = limit_scale(scale);

  for (size_t i = 0; i < A->rows; i++) {
    A->matrix[i][1] *= scale;
  }
}

/**
 * @brief Scale by OZ
 *
 * Scales the object by OZ axis by scale value.
 *
 * @param A Object matrix
 * @param scale Value to scale the object by
 */
void scale_by_oz(matrix_t *A, float scale) {
  scale = limit_scale(scale);

  for (size_t i = 0; i < A->rows; i++) {
    A->matrix[i][2] *= scale;
  }
}

/**
 * @brief Limit scale
 *
 * Limits scale value when scaling the object.
 *
 * @param scale Value to scale the object by
 */
float limit_scale(float scale) {
  if (scale < SCALE_LIMIT_MIN)
    scale = SCALE_LIMIT_MIN;
  else if (scale > SCALE_LIMIT_MAX)
    scale = SCALE_LIMIT_MAX;

  return scale;
}
