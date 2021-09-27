#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>

#pragma warning(disable:4996)

typedef unsigned char uchar;

uchar** uc_alloc(int size_x, int size_y) {
	uchar** m;
	int i;

	if ((m = (uchar**)calloc(size_y, sizeof(uchar*))) == NULL) {
		printf("d_alloc error 1\n");
		exit(-1);
	}
	for (i = 0; i < size_y; i++) {
		if ((m[i] = (uchar*)calloc(size_x, sizeof(uchar))) == NULL) {
			printf("d_alloc error 2\n");
			exit(-1);
		}
	}
	return m;
}

void uc_free(int size_x, int size_y, uchar** ucmatrix) {
	int i;
	for (i = 0; i < size_y; i++)
		free(ucmatrix[i]);
	free(ucmatrix);
}

void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { //하드디스크에서 이미지파일을 읽어 가져옴
	int i;
	FILE* f;

	if ((f = fopen(filename, "rb")) == NULL) { // read binary
		printf("%s File open Error!\n", filename);
		exit(-1);
	}
	for (i = 0; i < size_y; i++) {
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) { // 파일을 읽어 ucmatrix에 저장
			printf("Data Read Error!\n");
			exit(-1);
		}
	}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { //파일에 ucmatrix를 작성
	int i;
	FILE* f;

	if ((f = fopen(filename, "wb")) == NULL) { // write binary
		printf("%s File open Error!\n", filename);
		exit(-1);
	}
	for (i = 0; i < size_y; i++) { // 파일에 데이터 쓰기
		if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) { // 한 번에 한 줄씩 작성
			printf("Data Write Error!\n");
			exit(-1);
		}
	}
}

void mozaiq(uchar** img, uchar** out, int Row, int Col, int Block) {
	int i, j, x, y;
	int tmp = 0, cnt=0;
	for (i = 0; i < Row; i += Block) { // 행에 대하여 블록 단위로 진행
		for (j = 0; j < Col; j += Block) { // 열에 대하여 블록 단위로 진행
			for (y = 0; y < Block; y++) { // 블록 내 행에 대하여 반복
				for (x = 0; x < Block; x++) { // 블록 내 열에 대하여 반복
					if (j + x < Row && i + y < Col) { // 이미지 파일을 벗어나지 않는 범위만 접근
						tmp += img[i + y][j + x];	// 블록 내 각 픽셀들의 데이터 더함
						cnt++;		// 블록 내 픽셀의 개수 세기
					}
				}
			}
			tmp /= cnt;	// 블록의 각 픽셀들의 평균치
			for (y = 0; y < Block; y++) {
				for (x = 0; x < Block; x++) {
					if (j + x < Row && i + y < Col) // 이미지 파일을 벗어나지 않는 범위만 접근
						out[i + y][j + x] = tmp;	// 블록 내 픽셀들을 모두 평균치로 치환
				}
			}
			tmp = 0;
			cnt = 0;
		}
	}
}

int main(int argc, char* argv[]) {
	int Block;
	int Col, Row;
	uchar** img, ** Result;

	if (argc != 5) {
		fprintf(stderr, "\n %s InputImg x-size y-size ResultImg!!\n", argv[0]);
		exit(-1);
	}

	Col = atoi(argv[2]);
	Row = atoi(argv[3]);

	img = uc_alloc(Col, Row);
	Result = uc_alloc(Col, Row);

	read_ucmatrix(Col, Row, img, argv[1]);

	printf("모자이크 비율을 입력하세요 >");
	scanf_s("%d", &Block);
	if (Block > Row || Block > Col) {
		printf("모자이크 비율이 너무 큽니다.\n");
		exit(-1);
	}
	else if (Block < 1) {
		printf("모자이크 비율이 너무 작습니다.\n");
		exit(-1);
	}
	printf("모자이크 비율 : %d\n", Block);
	mozaiq(img, Result, Row, Col, Block);
	write_ucmatrix(Col, Row, Result, argv[4]);

	uc_free(Col, Row, img);
	uc_free(Col, Row, Result);

	return 0;
}

