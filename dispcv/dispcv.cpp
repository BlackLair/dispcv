#include <stdio.h>
#include <opencv2/opencv.hpp>
typedef unsigned char uchar;

using namespace std;
using namespace cv;

uchar** uc_alloc(int size_x, int size_y) // uchar x*y 크기의 메모리를 할당
{
	uchar** m;
	int i;

	if ((m = (uchar**)calloc(size_y, sizeof(uchar*))) == NULL) {
		printf("d_alloc error 1\n");
		exit(0);
	}
	for (i = 0; i < size_y; i++) {
		if ((m[i] = (uchar*)calloc(size_x, sizeof(uchar))) == NULL) {
			printf("d_alloc error 2\n");
			exit(0);
		}
	}
	return m;
}
void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { //하드디스크에서 이미지파일을 읽어 가져옴
	int i;
	FILE* f;

	if ((f = fopen(filename, "rb")) == NULL) { // read binary
		printf("%s File open Error!\n", filename);
		exit(0);
	}
	for (i = 0; i < size_y; i++) {
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) { // 파일을 읽어 ucmatrix에 저장
			printf("Data Read Error!\n");
			exit(0);
		}
	}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { //파일에 ucmatrix를 작성
	int i;
	FILE* f;

	if ((f = fopen(filename, "wb")) == NULL) { // write binary
		printf("%s File open Error!\n", filename);
		exit(0);
	}
	for (i = 0; i < size_y; i++) { // 파일에 데이터 쓰기
		if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) { // 한 번에 한 줄씩 작성
			printf("Data Write Error!\n");
			exit(0);
		}
	}
}

void Negative(int Col, int Row, uchar** img, uchar** Result) {
	int i, j;
	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
			Result[i][j] = 255 - img[i][j];
}

void mozaiq(int col, int row, uchar** img, uchar** result) {
	int sum = 0;
	int i, j, k, l;
	int ratio;
	printf("모자이크 비율을 입력하세요 >");
	scanf_s("%d", &ratio);  // 모자이크를 할 비율을 입력. 4를 입력할 경우 4픽셀*4픽셀 범위가 하나의 값으로 통일됨
	if (ratio < 2) // 모자이크 비율이 2보다 작은 값을 입력할 경우 모자이크 하지 않음
		return;
	for (i = 0; i < col; i+=ratio) { // column에 대해 작업
		if (col - i < ratio)  // 남은 공간이 ratio보다 작을 경우 스킵
			break;
		for (j = 0; j < row; j+=ratio) { //  row에 대해 작업
			if (row - j < ratio) // 남은 공간이 ratio보다 작을 경우 스킵
				break;
			for (k = 0; k < ratio; k++)// ratio * ratio 범위의 총합을 구함 ( img[i][j]를 좌측 상단 꼭짓점으로 하는 정사각형 범위 )
				for (l = 0; l < ratio; l++)
					sum += img[i+k][j+l]; 
			sum = sum / (ratio*ratio);   // ratio * ratio 범위의 평균치를 구함
			if (sum > 255)	// 밝기 최대치를 넘길 경우 한계점으로 설정
				sum = 255;
			for (k = 0; k < ratio; k++)  // 평균치를 구했던 정사각형 구간의 모든 값을 평균치로 치환
				for (l = 0; l < ratio; l++)
					result[i+k][j+l] = sum;  
		}
	}

}

int main(int argc, char* argv[])
{
	int i, j, Row, Col;
	uchar** img, **result;
	if (argc != 4) {
		printf("Exe imgData x_size y_size\n");
		exit(0);
	}

	Col = atoi(argv[2]); // ascii to integer
	Row = atoi(argv[3]);
	img = uc_alloc(Col, Row);
	result = uc_alloc(Col, Row);
	read_ucmatrix(Col, Row, img, argv[1]);

//	mozaiq(Col, Row, img, result);
	Negative(Row, Col, img,result); 



	//////////// 화면에 이미지 띄우는 과정 /////////////////////
	Mat cvImg(Row, Col, CV_8UC(1)); 

	for(i=0;i<Row;i++)
		for (j = 0; j < Col; j++) {
			//(cvImg.data+cvImg.step*i)[j]=img[i][j];
			cvImg.at<uchar>(i, j) = result[i][j];
		}
	namedWindow(argv[1], WINDOW_AUTOSIZE); // 창 세팅
	imshow(argv[1], cvImg);	// 최종 출력
	/////////////////////////////////////////////////////
	
	waitKey(0); // 아무 키나 누르면 다음으로 진행 ( getchar(); )
	return 0;
}