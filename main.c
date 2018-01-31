// 17w2083h
// 牧野 拓也

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// あ〜との20文字
// 文字の種類
#define CHAR_CLASS 20
// 各文字セット中の文字数
#define LETTER_NUM 100
// ファイルから入力するデータの横の文字数
#define LETTER_SIZE 64

// 入力層ユニット
#define INPUT_SIZE 64
// 中間層ユニット
#define MIDDLE_SIZE 32
// 出力層ユニット
// あ〜との20文字
#define OUTPUT_SIZE 20

// 学習定数
#define ETA 0.6
// 安定化定数
#define ALPHA 0.3
// 平均二乗誤差のしきい値
#define ERROR_THRESHOLD 0.005

// Struct {

// 重さを格納する
typedef struct Weight {
  // 重み
  double value;
  // 更新した量
  double delta_value;
} weight_s;

// } Struct

// Util {

double generate_rand() {
  return rand() / (1.0 + RAND_MAX);
}

double sigmoid(double gain, double x) {
    return 1.0 / (1.0 + exp(-gain * x));
}

// 平均二乗誤差
double square_error(double output[OUTPUT_SIZE], double output_hat[OUTPUT_SIZE]) {
  double output_sum = 0;
  for (int output_num = 0; output_num < OUTPUT_SIZE; output_num++) {
    output_sum += (output_hat[output_num] - output[output_num]) * (output_hat[output_num] - output[output_num]);
  }
  return output_sum / OUTPUT_SIZE;
}

int print_array(int num, double array[num]) {
  printf("{");
  for (int i = 0; i < num; i++) {
    printf("%lf, ", array[i]);
  }
  printf("}\n");
  return 0;
}

// } Util

// ファイルから文字データを取得する
int fetch(double input[LETTER_NUM][INPUT_SIZE + 1], char *file_name) {
  char row[LETTER_SIZE];
  FILE *fp = fopen(file_name, "r");

  if(fp == NULL) {
    printf("Error\n");
    return -1;
  }

  // 一文字ずつ取り出す
  for(int letter_num = 0; letter_num < LETTER_NUM; letter_num++) {
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
  for (int letter_num = 0; letter_num < LETTER_NUM; letter_num++) {
    for(int i = 0; i < INPUT_SIZE; i++) {
      input[letter_num][i] = 1.0 * input[letter_num][i] / INPUT_SIZE;
      // printf("%2d:%6.3lf\n", i, input[letter_num][i]);
    }
    // 常に1を出力する入力層のユニット
    input[letter_num][INPUT_SIZE] = 1;
  }

  fclose(fp);
  return 0;
}

// 重さの初期化を行う
int init_weight(int before_size, int after_size, weight_s weight[before_size][after_size]) {
  for (int before_num = 0; before_num < before_size; before_num++) {
    for(int after_num = 0; after_num < after_size; after_num++) {
      weight[before_num][after_num].value = generate_rand();
      weight[before_num][after_num].delta_value = 0;
    }
  }
  return 0;
}

// 順方向の伝播を行う
int forward(
  int current_unit_num,
  int next_unit_num,
  double current_layer[current_unit_num],
  double next_layer[next_unit_num],
  weight_s weight[current_unit_num][next_unit_num]
) {
  for (int next_num = 0; next_num < next_unit_num; next_num++) {
    // printf("next:%d\n", next_num);
    for (int current_num = 0; current_num < current_unit_num; current_num++) {
      next_layer[next_num] += weight[current_num][next_num].value * current_layer[current_num];
      // printf("%6.3lf(+%6.3lf), ", next_layer[next_num], weight[current_num][next_num].value * current_layer[current_num]);
    }
    next_layer[next_num] = sigmoid(1.0, next_layer[next_num]);
    // printf("%6.3lf\n\n", next_layer[next_num]);
  }
  return 0;
}

// 出力層の逆伝播
int update_output_weight(
  double output_hat[OUTPUT_SIZE],
  double middle[MIDDLE_SIZE + 1],
  double output[OUTPUT_SIZE],
  weight_s weight_output[MIDDLE_SIZE + 1][OUTPUT_SIZE]
) {
  for(int output_num = 0; output_num < OUTPUT_SIZE; output_num++) {
    for (int middle_num = 0; middle_num < MIDDLE_SIZE + 1; middle_num++) {
      weight_output[middle_num][output_num].delta_value = ETA * (output_hat[output_num] - output[output_num]) * output[output_num] * (1 - output[output_num]) * middle[middle_num] + ALPHA * weight_output[middle_num][output_num].delta_value;
      weight_output[middle_num][output_num].value += weight_output[middle_num][output_num].delta_value;
    }
  }
  return 0;
}

// 隠れ層の逆伝播
int update_middle_weight(
  double output_hat[OUTPUT_SIZE],
  double input[INPUT_SIZE + 1],
  double middle[MIDDLE_SIZE + 1],
  double output[OUTPUT_SIZE],
  weight_s weight_middle[INPUT_SIZE + 1][MIDDLE_SIZE],
  weight_s weight_output[MIDDLE_SIZE + 1][OUTPUT_SIZE]
) {
  for (int middle_num = 0; middle_num < MIDDLE_SIZE; middle_num++) {
    double sigma_middle = 0;
    for (int input_num = 0; input_num < INPUT_SIZE + 1; input_num++) {
      for(int output_num = 0; output_num < OUTPUT_SIZE; output_num++) {
        sigma_middle += (output_hat[output_num] - output[output_num]) * output[output_num] * (1 - output[output_num]) * weight_output[middle_num][output_num].value;
      }
      weight_middle[input_num][middle_num].delta_value = ETA * middle[middle_num] * (1 - middle[middle_num]) * input[input_num] * sigma_middle + ALPHA * weight_middle[input_num][middle_num].delta_value;
      weight_middle[input_num][middle_num].value += weight_middle[input_num][middle_num].delta_value;
    }
  }
  return 0;
}

int main() {
  srand((unsigned int)time(NULL));

  // 正解データ
  // 20 * 20
  double output_hat[CHAR_CLASS][OUTPUT_SIZE] = {{0}};
  // 正解データ作成
  for (int i = 0; i < CHAR_CLASS; i++) {
    output_hat[i][i] = 1;
    // print_array(OUTPUT_SIZE, output_hat[i]);
  }

  // ニューラルネットワーク学習用
  char train_file_name[256] = {'\0'};
  double input[CHAR_CLASS][LETTER_NUM][INPUT_SIZE + 1] = {{{0}}};
  double middle[MIDDLE_SIZE + 1] = {0};
  // 常に1を出力する中間層のユニット
  middle[MIDDLE_SIZE] = 1;
  double output[OUTPUT_SIZE] = {0};
  weight_s weight_middle[INPUT_SIZE][MIDDLE_SIZE];
  weight_s weight_output[MIDDLE_SIZE][OUTPUT_SIZE];
  // 大きな値ならなんでもいい
  double error = 100;

  // 重み初期化
  init_weight(INPUT_SIZE + 1, MIDDLE_SIZE, weight_middle);
  init_weight(MIDDLE_SIZE + 1, OUTPUT_SIZE, weight_output);

  // 文字の種類ごと
  for (int char_class = 0; char_class < CHAR_CLASS; char_class++) {
    sprintf(train_file_name, "Data/hira0_%02dL.dat", char_class);
    // ファイルから読み込み
    fetch(input[char_class], train_file_name);
  }

  // しきい値以下の誤差になるまで学習をする
  // for (int i = 0; i < 200; i++) {
  while (error >= ERROR_THRESHOLD) {
    error = 0;
    // // 文字の種類ごと
    // for (int char_class = 7; char_class < 8; char_class++) {
    // // for (int char_class = 0; char_class < CHAR_CLASS; char_class++) {
    //   // 入力パターン個数分の処理
    //   for (int letter_num = 0; letter_num < LETTER_NUM; letter_num++) {

    // 入力パターン個数分の処理
    for (int letter_num = 0; letter_num < LETTER_NUM; letter_num++) {
      // 文字の種類ごと
      // for (int char_class = 0; char_class < 2; char_class++) {
      for (int char_class = 0; char_class < CHAR_CLASS; char_class++) {

        // (1)
        forward(INPUT_SIZE + 1, MIDDLE_SIZE, input[char_class][letter_num], middle, weight_middle);
        // (2)
        forward(MIDDLE_SIZE + 1, OUTPUT_SIZE, middle, output, weight_output);
        // ここで二乗誤差の中間結果を計算しておく
        // print_array(OUTPUT_SIZE, output);
        double tmp_error = square_error(output, output_hat[char_class]);
        error += tmp_error;
        // printf("Square Error:%lf\n", tmp_error);
        // (5)
        update_output_weight(output_hat[char_class], middle, output, weight_output);
        // (6)
        update_middle_weight(output_hat[char_class], input[char_class][letter_num], middle, output, weight_middle, weight_output);
        // print_array(INPUT_SIZE + 1, input[letter_num]);
        // print_array(MIDDLE_SIZE + 1, middle);
      }
    }
    error = error / (LETTER_NUM * CHAR_CLASS);
    printf("Final Square Error:%lf\n", error);
  }

  // 識別する
  // ニューラルネットワークテスト用
  char test_file_name[256] = {'\0'};
  double test_input[CHAR_CLASS][LETTER_NUM][INPUT_SIZE + 1] = {{{0}}};

  // 文字の種類ごと
  for (int char_class = 0; char_class < CHAR_CLASS; char_class++) {
    printf("----------------------------------------------------\n");
    // sprintf(test_file_name, "Data/hira0_%02dL.dat", char_class);
    sprintf(test_file_name, "Data/hira0_%02dT.dat", char_class);
    // ファイルから読み込み
    fetch(test_input[char_class], test_file_name);

    // 分布確認用
    int histgram[CHAR_CLASS] = {0};

    // 入力パターン個数分の処理
    for (int letter_num = 0; letter_num < LETTER_NUM; letter_num++) {
      // (1)
      forward(INPUT_SIZE + 1, MIDDLE_SIZE, input[char_class][letter_num], middle, weight_middle);
      // (2)
      forward(MIDDLE_SIZE + 1, OUTPUT_SIZE, middle, output, weight_output);
      // print_array(OUTPUT_SIZE, output);

      // 最大値を求める
      double max = 0;
      int index = 0;
      for (int char_class = 0; char_class < CHAR_CLASS; char_class++) {
        if(output[char_class] >= max) {
          max = output[char_class];
          index = char_class;
        }
      }

      histgram[index]++;
    }

    printf("Correct: %d (%d/%d){", char_class, histgram[char_class], LETTER_NUM);
    for (int i = 0; i < CHAR_CLASS; i++) {
      printf("'%d':%d, ", i, histgram[i]);
    }
    printf("}\n");
  }


  return 0;
}
