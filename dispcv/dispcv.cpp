#include <stdio.h>
#include <opencv2/opencv.hpp>

#pragma warning(disable:4996)

typedef unsigned char uchar;

using namespace std;
using namespace cv;

uchar** uc_alloc(int size_x, int size_y) // uchar x*y 크기의 메모리를 할당
{
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



int main(int argc, char* argv[])
{
	int i, j, Row, Col;
	uchar** img, **result;
	if (argc != 4) {
		printf("Exe imgData x_size y_size\n");
		exit(-1);
	}

	Col = atoi(argv[2]); // ascii to integer
	Row = atoi(argv[3]);
	img = uc_alloc(Col, Row);
	result = uc_alloc(Col, Row);
	read_ucmatrix(Col, Row, img, argv[1]);


	//////////// 화면에 이미지 띄우는 과정 /////////////////////
	Mat cvImg(Row, Col, CV_8UC(1)); 

	for(i=0;i<Row;i++)
		for (j = 0; j < Col; j++) {
			//(cvImg.data+cvImg.step*i)[j]=img[i][j];
			cvImg.at<uchar>(i, j) = img[i][j];
		}
	namedWindow(argv[1], WINDOW_AUTOSIZE); // 창 세팅
	imshow(argv[1], cvImg);	// 최종 출력
	/////////////////////////////////////////////////////
	
	waitKey(0); // 아무 키나 누르면 다음으로 진행 ( getchar(); )
	return 0;
}