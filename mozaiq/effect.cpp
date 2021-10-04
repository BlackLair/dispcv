#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>

#pragma warning(disable:4996)

typedef unsigned char uchar;


/*********************Standard Func*******************************/
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
/***************************Effect Func*************************************/
void Negative(int Col, int Row, uchar** img, uchar** Result) { // 밝기 반전 함수
	int i, j;
	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
			Result[i][j] = 255 - img[i][j];
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

void blur(uchar** img, uchar** out, int Row, int Col, int ratio) {
	int i, j, x, y;
	int tmp = 0, cnt = 0;
	for (i = 0; i < Row; i++) { // 행에 대하여 반복
		for (j = 0; j < Col; j++) { // 열에 대하여 반복
			for (y = i - ratio; y <= i + ratio; y++) { // 대상 픽셀 좌상단부터 열에 대하여 반복
				for (x = j - ratio; x <= j + ratio; x++) { // 대상 픽셀 좌상단부터 행에 대하여 반복
					if (x >= 0 && y >= 0 && x < Col && y < Row) { // 이미지 범위를 넘지 않는 부분만 접근
						tmp += img[y][x]; // 대상 픽셀 주변 지정범위만큼의 값들을 모두 더함
						cnt++;
					}
				}
			}
			out[i][j] = (int)tmp / cnt; // 대상 픽셀의 값을 주변 지정범위 값들의 평균으로 치환
			tmp = 0;
			cnt = 0;
		}
	}
}

/********************************Main******************************/
int main(int argc, char* argv[]) {
	int sel;
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
	printf("적용할 효과를 선택하세요.\n");
	printf("1. Negative\n2. Mosaic\n3. Blur\n");
	scanf_s("%d", &sel);
	switch (sel) {
	case 1:
		printf("Negative(밝기 반전) 시작\n");
		Negative(Col, Row, img, Result);
		printf("작업 종료\n");
		break;
	case 2:
		printf("모자이크 비율을 입력하세요. ( 2 ~ %d )\n", Col);
		scanf_s("%d", &sel);
		if (sel<2 || sel>Col) {
			printf("범위를 벗어났습니다.\n");
			exit(0);
		}
		else {
			printf("Mosaic(모자이크) 시작.(블록 크기 : %d\n", sel);
			mozaiq(img, Result, Row, Col, sel);
			printf("작업 종료\n");
		}
		break;
	case 3:
		printf("블러 처리 강도를 입력하세요. ( 2 ~ %d)\n", Col);
		scanf_s("%d", &sel);
		if (sel<2 || sel>Col) {
			printf("범위를 벗어났습니다.\n");
			exit(0);
		}
		else {
			printf("Blur(블러) 시작.(블러 강도 : %d\n", sel);
			blur(img, Result, Row, Col, sel);
			printf("작업 종료\n");
		}
		break;
	}
		
	
	
	write_ucmatrix(Col, Row, Result, argv[4]);

	uc_free(Col, Row, img);
	uc_free(Col, Row, Result);

	return 0;
}

