#include "student.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Using pointers, the will be able to swap numbers and the changed values will be saved.
//
void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
  
}

// Replaces every integer in the array with its absolute value.
//
void modifyarray(int array[], int length) {
  for (int i = 0; i < length; i++){
    int val = array[i];
    array[i] = abs(val);
  }

}

// Using recursion, the trifibonacci will get the value of the nth trifibonacci number.
//
int nthtrifibonacci(int n) {
  if (n == 0){
    return 0;
  }
  
  if (n == 1){
    return 1;
  }

  if (n == 2){
    return 1;
  }
  
  if (n > 2){
    return nthtrifibonacci(n-1) + nthtrifibonacci(n-2) + nthtrifibonacci(n-3);
  }
  return 0;
}

// Calculates the mean from 
double mean(int array[], int length) {
  double total = 0;
  for (int i = 0; i < length; i++){
    total += array[i];
  }

  return (total/length);
}

// Loops through each value in the array and pushes it to an earlier index if the 
// integer at the index before it is greater.
//
void insertionsort(int array[],int length) {
  for (int i = 1; i < length; i++) {
        int num = array[i];
        int j = i - 1;
        while (j >= 0 && array[j] > num) {
            array[j + 1] = array[j];
            j = j - 1;
        }

        array[j + 1] = num;
  }
}


// Mods a number by 10 to extract the digit and adds the digit to our answer.
// For every digit, the answer gets multiplied by 10 to make room for the next digit.
//
int reverseinteger(int n) {
  bool neg = false;
  if (n < 0){
    neg = true;
  }
  int answer = 0;
  int num = 0;
  while (n > 0){
    num = n % 10;
    answer = (answer*10) + num;
    n = n / 10;
  }
  if (neg){
    answer *= -1;
  }
  return answer;
}

// Using indices, the algorithm checks if the character at each index is the same,
// and meets at the middle.
//
int palindrome(char str[], int length) {
  if (length == 0){
    return 1;
  }
  int left = 0;
  int right = length - 1;
  while (left < right){
    if (str[left] != str[right]){
      return 0;
    }
    left++;
    right--;
  }
  
  return 1;
}