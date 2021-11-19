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
void getPdf(int Row, int Col, uchar** img, double* pdf) { // 히스토그램(pdf) 구하기
	int i, j, sum=0;
	
	for (i = 0; i < 256; i++) // 메모리 초기화
		pdf[i] = 0;
	
	for (i = 0; i < Row; i++) { // 이미지에서 밝기별 픽셀 개수를 셈
		for (j = 0; j < Col; j++) {
			pdf[img[i][j]]+=1;
		}
	}
	sum = Row * Col; // 모든 픽셀 개수 구하기
	for (i = 0; i < 256; i++) { // 각 밝기별로 픽셀의 개수를 총 픽셀 개수로 나눔
		pdf[i] = (pdf[i] / sum);
	}
}
void setPdfforDiagram(double* pdf) { // pdf를 이미지로 표현하기 위해 픽셀 수가 가장 많은 밝기를 1로 기준으로 하는 데이터 생성
	double max = 0;
	int i;
	for (i = 0; i < 256; i++) { // 이미지에서 가장 많은 픽셀을 포함한 밝기 값의 확률을 탐색
		if (pdf[i] > max)
			max = pdf[i];
	}
	for (i = 0; i < 256; i++) { // 각 확률들을 최댓값으로 나눈 값을 pdf에 넣음
		pdf[i] = (pdf[i] / max);
	} // pdf의 각 데이터들은 0에서 1 사이의 값을 가진다. 가장 큰 값은 무조건 1이 된다.
}
void getCdf(double* pdf, double* cdf) { // 확률분포 pdf를이용하여 누적확률분포 cdf를 구함
	int i, j;

	for (i = 0; i < 256; i++) {
		cdf[i] = 0;			// 메모리 초기화
		for (j = 0; j<=i; j++) { 
			cdf[i] += pdf[j];	// i번째 cdf 값은 0부터 i까지 pdf의 값을 모두 더한 값
		}
	}
}
void getCdfSymmetry(double* cdf, double* targetCdf) { // 역함수를 구함(y=x에 대하여 대칭하는 함수 구하기)
	int i, j, temp;
	for (i = 0; i < 256; i++)	// 메모리 초기화
		targetCdf[i] = 0;
	for (i = 0; i < 256; i++) {
		temp = 255 * cdf[i];	// 누적확률 값(0, 1 사이)에 255를 곱해 최소 0, 최대 255가 되도록 한다.
		targetCdf[temp] =(double) i / 255; // 
	}
	
	for (i = 1; i < 256; i++) { // CDF는 단조증가함수이므로 함수의 값이 비어있는 곳을 채움
		if (targetCdf[i] < targetCdf[i - 1])
			targetCdf[i] = targetCdf[i - 1];
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
		memset(resultname, '\0', 32); // 결과 파일 이름 변수 초기화
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
void makeDiagram(double* diagram, const char* filename) { // pdf나 cdf를 그래프 이미지로 변환 및 생성
	int i, j;
	uchar** result;
	result = uc_alloc(256, 256);
	for (j = 0; j < 256; j++) {
		for (i = 255; i > -1; i--) { // 이미지의 최하단부터 시작
			if (diagram[j] * 255 >= 255 - i) // 현재 픽셀이 함수 값보다 아래쪽이면 밝기 255
				result[i][j] = 255;
			else
				result[i][j] = 0;	// 현재 픽셀이 함수값 이상이면 밝기 0
		}
	}
	write_ucmatrix(256, 256, result, filename);
	uc_free(256, 256, result);
}
void HistoMatch(int Row, int Col, uchar** img, int targetRow, int targetCol, uchar** target, uchar** result) { // 히스토그램 지정
	double imgCdf[256], imgPdf[256]; // 원본 이미지 PDF, CDF
	double targetCdf[256], targetPdf[256]; // 지정 이미지 PDF, CDF
	int i, j, k;
	// 원본 이미지와 지정 이미지의 PDF, CDF 각각 구함
	getPdf(Row, Col, img, imgPdf);
	getPdf(targetRow, targetCol, target, targetPdf);
	getCdf(imgPdf, imgCdf);
	getCdf(targetPdf, targetCdf);
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			for (k = 0; k < 256; k++) { // 지정 이미지 CDF의 앞부터 검사
				if (targetCdf[k]>=imgCdf[img[i][j]]) { // 원본 이미지 픽셀 밝기의 CDF값과 지정이미지 밝기가 k인 지점 CDF 값이 같아지면 밝기를 k로 변환
					result[i][j] = k;
					break;
				}
			}
		}
	}
}
void HistoEqual(int Row, int Col, uchar** img, uchar** result) { // 히스토그램 평활화
	int i, j, k;
	double imgPdf[256], imgCdf[256];
	double targetCdf[256];
	getPdf(Row, Col, img, imgPdf);
	getCdf(imgPdf, imgCdf);
	for (i = 0; i < 256; i++) // 원점과 점 (255, 1)을 연결하는 직선 함수가 targetCDF가 된다.
		targetCdf[i] = (double)i / 255;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			for (k = 0; k < 256; k++) {
				if (targetCdf[k] >= imgCdf[img[i][j]]) {
					result[i][j] = k;
					break;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
void makePDFCDFFile(int Row, int Col, uchar** img, double* pdf, double* cdf, char* filename, char* argv) { // PDF, CDF를 이용해 이미지 파일 생성
	printf("결과 이미지의 PDF, CDF 그래프 생성 시작.\n");
	getPdf(Row, Col, img, pdf);			// 결과 이미지의 PDF 생성
	getCdf(pdf, cdf);					// 결과 이미지의 CDF 생성
	strcpy(filename, argv);
	strcat(filename, "_CDF");			// CDF 이미지의 파일명 지정
	makeDiagram(cdf, filename);			// CDF 이미지 파일 생성
	setPdfforDiagram(pdf);				// PDF 배열을 이미지 생성을 위한 값으로 변환
	memset(filename, '\0', sizeof(filename));	
	strcpy(filename, argv);
	strcat(filename, "_PDF");			// PDF 이미지의 파일명 지정
	makeDiagram(pdf, filename);			// PDF 이미지 파일 생성
	printf("생성 종료\n");
}
////////////////////////////////////////////////////////////////////
/********************************Main******************************/
int main(int argc, char* argv[]) {
	int sel;
	int Row, Col, tRow, tCol;
	int isEnded = 0;
	uchar** img, ** Result, **target;
	double value;
	double pdf[256], cdf[256], tcdf[256];
	char filename[32];

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
	printf("6. PowImg\n7. BitSlicing\n8. MaskOr\n9. getPdf\n10. getCdf\n11. HistoMatch\n12. HistoEqual\n");
	scanf("%d", &sel);
	switch (sel) {
	case 1:
		printf("Negative(밝기 반전) 시작\n");
		Negative(Col, Row, img, Result);
		printf("작업 종료\n");
		break;
	case 2:
		printf("모자이크 비율을 입력하세요. ( 2 ~ %d )\n", Col);
		scanf("%d", &sel);
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
		scanf("%d", &sel);
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
		scanf("%d", &sel);
		switch (sel) {
		case 1:
			printf("%d번 선택. AdaptiveBinary(적응적 변환) 시작\n", sel);
			AdaptiveBinary0(img, Result, Row, Col);
			break;
		case 2:
			printf("%d번 선택. AdaptiveBinary(적응적 변환) 시작\n", sel);
			AdaptiveBinary1(img, Result, Row, Col);
			break;
		case 3:
			printf("%d번 선택. AdaptiveBinary(적응적 변환) 시작\n", sel);
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
		scanf("%lf", &value);
		printf("PowImg(Gamma Correction) 시작.( 감마 값 : %lf )\n", value);
		PowImg(img, Result, Row, Col, value);
		printf("작업 종료\n");
		break;
	case 7:
		printf("BitSlicing(비트 분리) 시작.\n");
		BitSlicing(img, Result, Row, Col, argv[4]);
		printf("작업 종료\n");
		isEnded = 1;
		break;
	case 8:
		printf("원의 반지름 값을 입력하세요.\n");
		scanf("%lf", &value);
		printf("MaksOr(or 연산) 시작.( 반지름 : %lf )\n", value);
		Circle(Result, Row, Col, value); // 원의 범위 이내일 경우 255, 밖일 경우 0
		MaskOr(img, Result, Result, Row, Col); // 원본 영상과 Or 연산 수행
		printf("작업 종료\n");
		break;
	case 9:
		printf("Pdf 이미지 생성 시작.(256 * 256)\n");
		getPdf(Row, Col, img, pdf); // PDF값이 저장된 배열 만듦
		setPdfforDiagram(pdf);		// PDF 그래프 값들의 분포를 보기 쉽게 변환
		makeDiagram(pdf, argv[4]);	// PDF 그래프 이미지 생성
		printf("작업 종료\n");
		isEnded = 1;				// 결과 파일이 이미 생성되었으므로 더 생성할 필요 없음을 알림
		break;
	case 10:
		printf("Cdf 이미지 생성 시작.(256 * 256)\n");
		getPdf(Row, Col, img, pdf);	// PDF값이 저장된 배열 만듦
		getCdf(pdf, cdf);			// PDF 함수를 이용해서 CDF를 알아냄
		makeDiagram(cdf, argv[4]);	// CDF 그래프 이미지 생성
		isEnded = 1;				// 결과 파일이 이미 생성되었으므로 더 생성할 필요 없음을 알림
		printf("작업 종료\n");
		break;
	case 11:
		printf("타겟 이미지 이름을 입력하세요.\n");
		while (getchar() != '\n');	// 입력 버퍼 비우기
		gets(filename);				// 지정 이미지 파일 이름을 입력받음
		printf("타겟 이미지 행, 열 수를 입력하세요.\n");
		scanf("%d %d", &tRow, &tCol);	// 지정 이미지의 크기를 입력함
		target = uc_alloc(tRow, tCol);
		printf("HistoMatch(히스토그램 지정) 시작.\n");
		read_ucmatrix(tRow, tCol, target, filename);
		HistoMatch(Row, Col, img, tRow, tCol, target, Result);	// 원본 이미지의 CDF를 지정 이미지에 Matching
		printf("작업 종료\n");
		makePDFCDFFile(Row, Col, Result, pdf, cdf, filename, argv[4]); // 결과 이미지를 이용해 PDF, CDF 그래프 이미지 파일 생성
		uc_free(tRow, tCol, target);	// 메모리 해제
		break;
	case 12:
		printf("HistoEqual(히스토그램 평활화) 시작.\n");
		HistoEqual(Row, Col, img, Result);	// 히스토그램 평활화
		printf("작업 종료\n");
		makePDFCDFFile(Row, Col, Result, pdf, cdf, filename, argv[4]); // 결과 이미지를 이용해 PDF, CDF 그래프 이미지 파일 생성
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

