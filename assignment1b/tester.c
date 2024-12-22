#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "reference.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// Unit test functions.

int test_swap();
int test_modifyarray();
int test_nthtrifibonacci();
int test_mean();
int test_insertionsort();
int test_reverseinteger();
int test_palindrome();

// Utility functions used by the unit tests.
bool arrays_differ(int arrray1[], int array2[], int length);
char *array_to_string(int array[], int length);

int main() {
  int score = 0;
  
  score += test_swap();
  score += test_modifyarray();
  score += test_nthtrifibonacci();
  score += test_mean();
  score += test_insertionsort();
  score += test_reverseinteger();
  score += test_palindrome();

  printf("Total sample test cases passed : %d/%d\n", score, 8);
  return 0;
}

int test_swap() {
  printf("running %s: ", __func__);

  int a = 1, b = 2;
  swap(&a, &b);
  if (a != 2 || b != 1) {
    printf("sample test case failed:\n  got: a=%d b=%d\n  expected: a=2 b=1\n", a, b);
    return 0;
  }
  printf("sample test case passed\n");
  return 1;
}

int test_modifyarray() {
  printf("running %s: ", __func__);
  int out[]      = {-2, 15, 29, -43, 1, -21, 58, -12, -100, 64};
  int expected[] = {-2, 15, 29, -43, 1, -21, 58, -12, -100, 64};

  modifyarray(out, ARRAY_SIZE(out));
  ref_modifyarray(expected, ARRAY_SIZE(expected));

  if (arrays_differ(out, expected, ARRAY_SIZE(out))) {
    char *out_s = array_to_string(out, ARRAY_SIZE(out));
    char *expected_s = array_to_string(expected, ARRAY_SIZE(expected));

    printf("sample test case failed:\n  got: %s\n  expected: %s\n", out_s, expected_s);

    free(out_s);
    free(expected_s);
    return 0;
  }

  printf("sample test case passed\n");
  return 1;
}


int test_nthtrifibonacci() {
  printf("running %s: ", __func__);
  int n = 8;
  int out = nthtrifibonacci(n);
  int expected = ref_nthtrifibonacci(n);

  if (out != expected) {
    printf("sample test case failed:\n  got: %d\n  expected: %d\n", out, expected);
    return 0;
  }

  printf("sample test case passed\n");
  return 1;
}

int test_mean() {
  {
    printf("running %s: ", __func__);

    int in[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    double out = mean(in, ARRAY_SIZE(in));
    double expected = ref_mean(in, ARRAY_SIZE(in));

    if (out != expected) {
      printf("sample test case failed:\n  got: %f\n  expected: %f\n", out, expected);
      return 0;
    }
    printf("sample test case passed\n");
  }
  return 1;
}

int test_insertionsort() {
  {
    printf("running %s: ", __func__);
    int out[]      = {8, 2, 2, 34, 234, -100, 1, 59, -15, 32, 87234};
    int expected[] = {8, 2, 2, 34, 234, -100, 1, 59, -15, 32, 87234};

    insertionsort(out, ARRAY_SIZE(out));
    ref_insertionsort(expected, ARRAY_SIZE(expected));

    if (arrays_differ(out, expected, ARRAY_SIZE(out))) {
      char *out_s = array_to_string(out, ARRAY_SIZE(out));
      char *expected_s = array_to_string(expected, ARRAY_SIZE(expected));

      printf("sample test case failed:\n  got: %s\n  expected: %s\n", out_s, expected_s);

      free(out_s);
      free(expected_s);
      return 0;
    }
    printf("sample test case passed\n");
  }
  return 1;
}

int test_reverseinteger() {
  printf("running %s: ", __func__);
  int n = 371;
  int out = reverseinteger(n);
  int expected = ref_reverseinteger(n);

  if (out != expected) {
    printf("sample test case failed:\n  got: %d\n  expected: %d\n", out, expected);
    return 0;
  }

  printf("sample test case passed\n");
  return 1;
}

int test_palindrome() {
  printf("running %s: ", __func__);
  char str[] = {"mimic"};
  int length = strlen(str);

  int out = palindrome(str, length);
  int expected = ref_palindrome(str, length);

  if (out != expected) {
    printf("sample test case failed:\n  got: %d\n  expected: %d\n", out, expected);
    return 0;
  }

  printf("sample test case passed\n");
  return 2;
}

bool arrays_differ(int array1[], int array2[], int length) {
  int i;
  for ( i = 0; i < length; ++i)
    if (array1[i] != array2[i])
      return true;
  return false;
}

char *array_to_string(int array[], int length) {
  int i,n;
  char *p = (char *)malloc(length * 10);
  for (i = 0, n = 0; i < length; ++i) {
    n += sprintf(p + n, "%d ", array[i]);
  }
  return p;
}


