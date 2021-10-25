#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>
#include<string.h>

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
/****************************Sub Func**********************************/
double average(uchar** img, int Row, int Col) { // 이미지 전체 픽셀들의 평균값 반환
	int i, j, sum = 0;
	double avg;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			sum += img[i][j];
		}
	}
	avg = sum / (Row * Col);
	return avg;
}
void Circle(uchar** Result, int Row, int Col, double diameter) {
	int i, j;
	double tmp, xSquare, ySquare;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			ySquare = (Row / 2 - i) * (Row / 2 - i);
			xSquare = (Col / 2 - j) * (Col / 2 - j);
			tmp = sqrt(ySquare + xSquare);
			if (tmp < diameter) Result[i][j] = 255;
			else Result[i][j] = 0;
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
void makeBinary(uchar** img, uchar** out, int Row, int Col, double avg) {
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > avg) out[i][j] = 255;	// 원본 이미지의 픽셀 밝기가 평균보다 크면 최대값으로 치환
			else out[i][j] = 0;						// 평균보다 작으면 0으로 치환
		}
	}
}
void AdaptiveBinary0(uchar** img, uchar** out, int Row, int Col) { // 밝기범위 내 200 치환, 나머지 0
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > 50 && img[i][j] < 150) out[i][j] = 200;
			else out[i][j] = 0;
		}
	}
}
void AdaptiveBinary1(uchar** img, uchar** out, int Row, int Col) { // 밝기범위 내 200 치환 나머지 원본 유지
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > 50 && img[i][j] < 150) out[i][j] = 200;
			else out[i][j] = img[i][j];
		}
	}
}
void AdaptiveBinary2(uchar** img, uchar** out, int Row, int Col) { // 밝기범위 내 원본 유지 나머지 0
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > 50 && img[i][j] < 150) out[i][j] = img[i][j];
			else out[i][j] = 0;
		}
	}
}
void PowImg(uchar** img, uchar** Result, int Row, int Col, double gamma) { // 이미지 감마 조절
	int i, j;
	double tmp;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			tmp = pow(img[i][j] / 255., 1 / gamma); // 앞 수의 1/gamma제곱을 구하는 함수 pow
			if (tmp * 255 > 255) tmp = 1.;	// 표현할 수 있는 밝기 범위 초과 시 값 치환
			else if (tmp * 255 < 0) tmp = 0.;
			Result[i][j] = tmp * 255;
		}
	}
}
void BitSlicing(uchar** img, uchar** Result, int Row, int Col, const char* filename) {
	int i, j, n;
	char resultname[32]; // 결과 파일의 이름을 저장할 변수
	char namenum[2];	// 결과 파일 이름 끝에 들어갈 숫자 저장 변수
	uchar mask = 0x01;	// 각 비트 자리의 값을 판별하기 위한 mask 값
	
	for (n = 0; n < 8; n++) {
		memset(resultname, NULL, 32); // 결과 파일 이름 변수 초기화
		strcpy(resultname, filename); // 결과 파일의 이름 복사
		for (i = 0; i < Row; i++) {
			for (j = 0; j < Col; j++) {
				if ((mask & img[i][j]) == pow(2, n)) // 뒤에서 n번째 비트가 1인지 판별
					Result[i][j] = 255;
				else Result[i][j] = 0;
			}
		}
		strcat(resultname, itoa(n, namenum, 10));	// 결과 파일의 이름 맨 끝에 비트 번호 부여
		write_ucmatrix(Col, Row, Result, resultname); // 결과 파일 쓰기
		mask <<= 1;
	}

}
void MaskOr(uchar** in1Img, uchar** in2Img, uchar** outImg, int ROW, int COL) {
	int i, j;
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			outImg[i][j] = in1Img[i][j] | in2Img[i][j];
		}
	}
}
/********************************Main******************************/
int main(int argc, char* argv[]) {
	int sel;
	int Col, Row;
	bool isEnded = false;
	uchar** img, ** Result;
	double value;

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
	printf("1. Negative\n2. Mosaic\n3. Blur\n4. makeBinary\n5. AdaptiveBinary\n");
	printf("6. PowImg\n7. BitSlicing\n8. MaskOr\n");
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
			printf("Mosaic(모자이크) 시작.( 블록 크기 : %d )\n", sel);
			mozaiq(img, Result, Row, Col, sel);
			printf("작업 종료\n");
		}
		break;
	case 3:
		printf("블러 처리 강도를 입력하세요. ( 2 ~ %d )\n", Col);
		scanf_s("%d", &sel);
		if (sel<2 || sel>Col) {
			printf("범위를 벗어났습니다.\n");
			exit(0);
		}
		else {
			printf("Blur(블러) 시작.( 블러 강도 : %d )\n", sel);
			blur(img, Result, Row, Col, sel);
			printf("작업 종료\n");
		}
		break;
	case 4:
		printf("makeBinary(이진파일화) 시작.\n");
		makeBinary(img, Result, Row, Col, average(img, Row, Col));
		printf("작업 종료\n");
		break;
	case 5:
		printf("변환 방식을 선택하세요. ( 밝기 범위 : 50 ~ 150 )\n");
		printf("1. 밝기 범위 내 200으로 치환, 나머지 0으로 치환\n");
		printf("2. 밝기 범위 내 200으로 치환, 나머지 원본 유지\n");
		printf("3. 밝기 범위 내 원본 유지, 나머지 0 치환\n");
		scanf_s("%d", &sel);
		switch (sel) {
			printf("%d번 선택. AdaptiveBinary(적응적 변환) 시작\n", sel);
		case 1:
			AdaptiveBinary0(img, Result, Row, Col);
			break;
		case 2:
			AdaptiveBinary1(img, Result, Row, Col);
			break;
		case 3:
			AdaptiveBinary2(img, Result, Row, Col);
			break;
		default:
			printf("없는 선택지입니다. 프로그램 종료\n");
			exit(0);
		}
		printf("작업 종료\n");
		break;
	case 6:
		printf("조정할 감마 값을 입력하세요.\n");
		scanf_s("%lf", &value);
		printf("PowImg(Gamma Correction) 시작.( 감마 값 : %lf )\n", value);
		PowImg(img, Result, Row, Col, value);
		printf("작업 종료\n");
		break;
	case 7:
		printf("BitSlicing(비트 분리) 시작.\n");
		BitSlicing(img, Result, Row, Col, argv[4]);
		printf("작업 종료\n");
		isEnded = true;
		break;
	case 8:
		printf("원의 반지름 값을 입력하세요.\n");
		scanf_s("%lf", &value);
		printf("MaksOr(or 연산) 시작.( 반지름 : %lf )\n", value);
		Circle(Result, Row, Col, value);
		MaskOr(img, Result, Result, Row, Col);
		printf("작업 종료\n");
		break;
	default:
		printf("없는 선택지입니다. 프로그램 종료\n");
		exit(0);
	}
		
	
	if(!isEnded)
		write_ucmatrix(Col, Row, Result, argv[4]);

	uc_free(Col, Row, img);
	uc_free(Col, Row, Result);

	return 0;
}

