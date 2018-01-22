#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int fetch(int writer_num, char data_mode) {
  char row[64];
  int letter[100][64][64];
  char *file_name = "Data/hira0_00L.dat";
  FILE *fp = fopen(file_name, "r");

  if(fp == NULL) {
    printf("Error\n");
    return -1;
  }
  rewind( fp );

  for(int letter_num = 0; letter_num < 10; letter_num++) {
    for(int i = 0; i < 64; i++ ) {
      fgets(row, 100, fp);
      for (int j = 0; j < 64; j++) {
        letter[letter_num][i][j] = row[j] - '0';
      }
    }
  }

  for (int letter_num = 0; letter_num < 5; letter_num++) {
    for(int i = 0; i < 64; i++) {
      for(int j = 0; j < 64; j++) {
        printf("%d", letter[letter_num][i][j]);
      }
      printf("\n");
    }
    printf("\n");
  }

  fclose(fp);
  return 0;
}

int main() {
  puts("Hello, world!");
  calcFeatureValue(0, 'T');
  return 0;
}
