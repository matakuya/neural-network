#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define LETTER_NUM 100
#define LETTER_SIZE 64
#define INPUT_SIZE 64
#define MIDDLE_SIZE 16
#define OUTPUT_SIZE 14

// Struct {

// 重さを格納する
typedef struct Weight {
  double current_value;
  double next_value;
} weight_s;

// } Struct

// Util {

double generate_rand() {
  return rand() / (1.0 + RAND_MAX);
}

double sigmoid(double gain, double x) {
    return 1.0 / (1.0 + exp(-gain * x));
}

// } Util

// ファイルから文字データを取得する
// int fetch(double input[100][INPUT_SIZE]) {
int fetch(double input[LETTER_NUM][INPUT_SIZE]) {
  char row[LETTER_SIZE];
  char *file_name = "Data/hira0_00L.dat";
  FILE *fp = fopen(file_name, "r");

  if(fp == NULL) {
    printf("Error\n");
    return -1;
  }

  // 一文字ずつ取り出す
  for(int letter_num = 0; letter_num < 100; letter_num++) {
    for(int row_num = 0; row_num < LETTER_SIZE; row_num++ ) {
      fgets(row, 100, fp);
      for (int column_num = 0; column_num < LETTER_SIZE; column_num++) {
        // 黒画素の個数の計算
        if (row[column_num] == '1') {
          int conv_row_num = row_num / 8;
          int conv_column_num = column_num / 8;
          // 一次元配列への変換
          input[letter_num][8 * conv_row_num + conv_column_num] += 1;
        }
      }
    }
  }

  // メッシュ特徴量を求める
  for (int letter_num = 0; letter_num < 100; letter_num++) {
    for(int i = 0; i < INPUT_SIZE; i++) {
      input[letter_num][i] = 1.0 * input[letter_num][i] / INPUT_SIZE;
      // printf("%2d:%6.3lf\n", i, input[letter_num][i]);
    }
  }

  fclose(fp);
  return 0;
}

// 重さの初期化を行う
int init_weight(int before_size, int after_size, weight_s weight[before_size][after_size]) {
  for (int before_num = 0; before_num < before_size; before_num++) {
    for(int after_num = 0; after_num < after_size; after_num++) {
      weight[before_num][after_num].current_value = generate_rand();
      // weight[before_num][after_num].current_value = u_rand();
      // printf("%6.3lf ", weight[before_num][after_num].current_value);
    }
  }

  return 0;
}

// 順方向の学習を行う
int forward(
  int current_unit_num,
  int next_unit_num,
  double current_layer[current_unit_num],
  double next_layer[next_unit_num],
  weight_s weight[current_unit_num][next_unit_num]
) {
  for (int next_num = 0; next_num < next_unit_num; next_num++) {
    printf("next:%d\n", next_num);
    for (int current_num = 0; current_num < current_unit_num; current_num++) {
      next_layer[next_num] += weight[current_num][next_num].current_value * current_layer[current_num];
      printf("%6.3lf(+%6.3lf), ", next_layer[next_num], weight[current_num][next_num].current_value * current_layer[current_num]);
    }
    next_layer[next_num] = sigmoid(1, next_layer[next_num]);
    printf("%6.3lf\n\n", next_layer[next_num]);
  }
  return 0;
}

// 逆方向の学習を行う
int backward() {
  return 0;
}

int main() {
  puts("hola!");

  srand((unsigned int)time(NULL));

  double input[LETTER_NUM][INPUT_SIZE] = {{0}};
  double middle[MIDDLE_SIZE] = {0};
  double output[OUTPUT_SIZE] = {0};

  weight_s weight_middle[INPUT_SIZE][MIDDLE_SIZE];
  weight_s weight_output[MIDDLE_SIZE][OUTPUT_SIZE];

  fetch(input);

  init_weight(INPUT_SIZE, MIDDLE_SIZE, weight_middle);
  init_weight(MIDDLE_SIZE, OUTPUT_SIZE, weight_output);

  forward(INPUT_SIZE, MIDDLE_SIZE, input[0], middle, weight_middle);
  forward(MIDDLE_SIZE, OUTPUT_SIZE, middle, output, weight_output);


  return 0;
}
