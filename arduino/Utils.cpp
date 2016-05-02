#include "Utils.h"

/*
* Takes an array of integers as input and a new integer value
* the new int value will be added and the oldest value of the
* array will be removed. Oldest value is at the top.
*/
int Utils::fifo(int array[], int newValue, short size) {
  int sum = 0;
  for (int i = 0; i < size - 1; i++) {
    array[i] = array[i + 1];
    sum += array[i + 1];
  }
  array[size - 1] = newValue;
  sum += newValue;
  return sum / size;
}
