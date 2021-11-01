/* 2021-2학기 영상처리 중간고사 작성일자 : 2021.11.01
20160973 권정우*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>
#include<string.h>

#pragma warning(disable:4996)

typedef unsigned char uchar;


/*********************Standard Func*******************************/
uchar** uc_alloc(int size_x, int size_y) { // 2차 배열 메모리 할당
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

void uc_free(int size_x, int size_y, uchar** ucmatrix) { // 2차 배열의 메모리 해제
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

/////////////////////////////////////////////////////////////////////////////////////
/****************************************Sub Func***********************************/
int isInCircle(int i, int j, int rowPos, int colPos, int diameter) { // (i, j)가 지정범위 원 이내일경우 1 리턴
	double tmp, xSquare, ySquare;									// 그렇지 않으면 0 리턴
	ySquare = (rowPos - i) * (rowPos - i);
	xSquare = (colPos - j) * (colPos - j);
	tmp = sqrt(ySquare + xSquare);
	if (tmp <= diameter) return 1;
	else return 0;
}
int isInImage(int i, int j, int Row, int Col) { // (i, j)가 이미지 크기 이내에 포함될 경우 1 리턴
	if (i > 0 && j > 0 && i < Row && j < Col)	// 그렇지 않을 경우 0 리턴
		return 1;
	else
		return 0;
}
/////////////////////////////////////////////////////////////////////////////////////
/****************************************Effect Func*********************************/
void Negative(int Row, int Col, int rowPos, int colPos, int diameter, uchar** img, uchar** Result) { // 밝기 반전 함수
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInImage(i, j, Row, Col)) { // (i, j)가 전체 이미지에서 벗어나는지 확인
				if (isInCircle(i, j, rowPos, colPos, diameter)) // 지정 범위 내일 경우만 변환
					Result[i][j] = 255 - img[i][j];
				else
					Result[i][j] = img[i][j]; // 지정 범위 외일 경우 원본 유지
			}
		}
	}
}
void mosaic(int Row, int Col, int rowPos, int colPos, int diameter, int Block_size, uchar** img, uchar** Result) {
	int i, j, x, y;
	int tmp = 0, cnt = 0;
	for (i = 0; i < Row; i += Block_size) { // 행에 대하여 블록 단위로 진행
		for (j = 0; j < Col; j += Block_size) { // 열에 대하여 블록 단위로 진행
			for (y = 0; y < Block_size; y++) { // 블록 내 행에 대하여 반복
				for (x = 0; x < Block_size; x++) { // 블록 내 열에 대하여 반복
					if (j + x < Row && i + y < Col) { // 이미지 파일을 벗어나지 않는 범위만 접근
						tmp += img[i + y][j + x];	// 블록 내 각 픽셀들의 데이터 더함
						cnt++;		// 블록 내 픽셀의 개수 세기
					}
				}
			}
			tmp /= cnt;	// 블록의 각 픽셀들의 평균치
			for (y = 0; y < Block_size; y++) {
				for (x = 0; x < Block_size; x++) {
					if (j + x < Row && i + y < Col) { // 이미지 파일을 벗어나지 않는 범위만 접근
						if (isInCircle(i + y, j + x, rowPos, colPos, diameter))
							Result[i + y][j + x] = tmp;	// 블록 내 픽셀들을 모두 평균치로 치환
						else
							Result[i + y][j + x] = img[i + y][j + x];
					}
				}
			}
			tmp = 0;
			cnt = 0;
		}
	}
}











////////////////////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[]) {
	uchar** img, ** Result;
	int Col, Row, sel;
	int Block_Size, rowPos, colPos, diameter;
	if (argc != 9) { // 
		fprintf(stderr, "Input Form:\n%s inImage outImage COL ROW Block_size row_pos col_pos diameter\n", argv[0]);
		exit(0);
	}
	/* transform input parameters to integer...*/
	Col = atoi(argv[3]);
	Row = atoi(argv[4]);
	Block_Size = atoi(argv[5]);
	rowPos = atoi(argv[6]);
	colPos = atoi(argv[7]);
	diameter = atoi(argv[8]);
	//////////////////////////////////////////
	img = uc_alloc(Col, Row);
	Result = uc_alloc(Col, Row);
	read_ucmatrix(Col, Row, img, argv[1]);
	printf("Which effect do you want?\n");
	printf("1. Negative\n2. Mosaic\n3. Blur\n4. makeBinary\n5. AdaptiveBinary\n");
	printf("6. PowImg\n7. BitSlicing\n8. MaskOr\n");
	scanf("%d", &sel);
	switch (sel) {
	case 1:
		printf("You selected Negative.\n");
		Negative(Row, Col, rowPos, colPos, diameter, img, Result);
		printf("Negative process finished.\n");
		break;
	case 2:
		printf("You selected Mosaic.\n");
		mosaic(Row, Col, rowPos, colPos, diameter, Block_Size, img, Result);
		printf("Mosaic process finished.\n");
		break;
	}

	write_ucmatrix(Col, Row, Result, argv[2]);

	uc_free(Col, Row, img);
	uc_free(Col, Row, Result);
	return 0;
}