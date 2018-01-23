#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LETTER_NUM 100
#define LETTER_SIZE 64
#define INPUT_SIZE 8

// ファイルから文字データを取得する
int input(double input[100][INPUT_SIZE][INPUT_SIZE]) {
  int letter[100][LETTER_SIZE][LETTER_SIZE] = {{{0}}};
  char row[LETTER_SIZE];
  char *file_name = "Data/hira0_00L.dat";
  FILE *fp = fopen(file_name, "r");

  if(fp == NULL) {
    printf("Error\n");
    return -1;
  }
  rewind( fp );

  int conv_row_num, conv_column_num;
  for(int letter_num = 0; letter_num < 10; letter_num++) {
    for(int row_num = 0; row_num < LETTER_SIZE; row_num++ ) {
      fgets(row, 100, fp);
      for (int column_num = 0; column_num < LETTER_SIZE; column_num++) {
        // 黒画素の個数の計算
        if (row[column_num] == '1') {
          conv_row_num = row_num / INPUT_SIZE;
          conv_column_num = column_num / INPUT_SIZE;
          input[letter_num][conv_row_num][conv_column_num] += 1;
        }
      }
    }
  }

  // メッシュ特徴量を求める
  for (int letter_num = 0; letter_num < 5; letter_num++) {
    for(int i = 0; i < INPUT_SIZE; i++) {
      for(int j = 0; j < INPUT_SIZE; j++) {
        input[letter_num][i][j] = input[letter_num][i][j] / (INPUT_SIZE * INPUT_SIZE);
        // printf("%6.3lf ", input[letter_num][i][j]);
      }
    }
  }
  return 0;

  fclose(fp);
  return 0;
}

int main() {
  puts("hola!");
  double input[100][8][8] = {{{0}}};
  fetch(input);
  return 0;
}
