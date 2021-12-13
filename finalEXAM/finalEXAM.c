/* 2021-2학기 영상처리 기말고사 작성일자 : 2021.12.13
20160973 권정우*/
/* DIP final exam */
#include<stdio.h>
#include<stdlib.h>
#include<math.h> // need -lm option if you compile in linux environment
#include<assert.h>
#include<string.h>
#define M_PI 3.14159265358979323846
#pragma warning(disable:4996) // in visual studio, scanf function causes C4996 warning. This macro solve this problem. 

#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5)) // transform actual number to whole number
typedef unsigned char uchar;


/*********************Standard Func*******************************/
uchar** uc_alloc(int size_x, int size_y) { // allocation of matrix memory
	uchar** m;
	int i;

	if ((m = (uchar**)calloc(size_y, sizeof(uchar*))) == NULL) { // alloc row size
		printf("d_alloc error 1\n");
		exit(-1);
	}
	for (i = 0; i < size_y; i++) { // alloc column size <row> times
		if ((m[i] = (uchar*)calloc(size_x, sizeof(uchar))) == NULL) {
			printf("d_alloc error 2\n");
			exit(-1);
		}
	}
	return m;
}

void uc_free(int size_x, int size_y, uchar** ucmatrix) { // release of matrix memory
	int i;
	for (i = 0; i < size_y; i++)
		free(ucmatrix[i]);
	free(ucmatrix);
}

void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { // read image file from hard disk
	int i;
	FILE* f;

	if ((f = fopen(filename, "rb")) == NULL) { // open binary file
		printf("%s File open Error!\n", filename);
		exit(-1);
	}
	for (i = 0; i < size_y; i++) {
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) { // save data in matrix
			printf("Data Read Error!\n");
			exit(-1);
		}
	}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { // write binary file
	int i;
	FILE* f;

	if ((f = fopen(filename, "wb")) == NULL) { // generate binary file named 'filename'
		printf("%s File open Error!\n", filename);
		exit(-1);
	}
	for (i = 0; i < size_y; i++) { // write data in binary file
		if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) {
			printf("Data Write Error!\n");
			exit(-1);
		}
	}
}

////////////////////////sub func//////////////////////////////////////////////'
double getMax(double** input, int Row, int Col) {
	int i, j;
	double max = -9999999999;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (max < input[i][j])
				max = input[i][j];
		}
	}
	return max;
}
double getMin(double **input, int Row, int Col) {
	int i, j;
	double min = 999999999;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (min > input[i][j])
				min = input[i][j];
		}
	}
	return min;
}
double getCxCy(int x, int y, int N) {
	if (x==0 && y==0) {
		return 1 / 2;
	}
	else if (x > 0 && y > 0) {
		return 1;
	}
	else {
		return 1 / sqrt(2);
	}
	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////effect func///////////////////////////////////////////////

void DCT(uchar** img, uchar** result, int Row, int Col, double** dresult) {
	int i, j, p, q;
	double sum = 0;
	double temp;
	double** temparray;
	temparray = (double**)calloc(sizeof(double*), Row);
	for (i = 0; i < Row; i++) {
		temparray[i] = (double*)calloc(sizeof(double), Col);
	}
	
	printf("DCT init\n");
	for (i = 0; i < Row; i++) {
		printf("%3d \n", i);
		for (j = 0; j < Col; j++) {
			for (p = i-(i % 8); p < i-(i % 8) + 8; p++) {
				for (q = j-(j % 8); q < j-(j % 8) + 8; q++) {
					if (p < Row && p < Col) {
						temp = 1;
						temp *= (double)img[p][q];
						temp *= cos(((2. * p + 1) * i * M_PI) / (2. * Row));
						temp *= cos(((2. * q + 1) * j * M_PI) / (2. * Col));
						sum += temp;
					}
				}
			}
			if (i == 0)
				sum /= sqrt(2);
			if (j == 0)
				sum /= sqrt(2);
			sum *= 4;
			sum /= 2. * Row;
			temparray[i][j] = sum;
			dresult[i][j] = sum;
			sum = 0;
		}
	}
	temp = getMin(temparray, Row, Col);
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			temparray[i][j] -= temp;
			result[i][j] = (uchar)round(temparray[i][j]);
		}
	}
	printf("DCT end\n");
}
void IDCT(double** img, uchar** result, int Row, int Col) {
	int i, j, p, q;
	double sum = 0;
	double temp;
	printf("IDCT init\n");
	for (i = 0; i < Row; i++) {
		printf("%3d\n", i);
		for (j = 0; j < Col; j++) {
			for (p = i-(i % 8); p < i-(i % 8) + 8; p++) {
				for (q = j-(j % 8); q < j-(j % 8) + 8; q++) {
					temp = 1;
					if (p % 8 == 0) {
						temp /= sqrt(2);
					}
					if (q % 8 == 0) {
						temp /= sqrt(2);
					}
					temp *= (double)img[p][q];
					temp *= cos(((2. * i + 1) * p * M_PI) / (2. * Row));
					temp *= cos(((2. * j + 1) * q * M_PI) / (2. * Col));
					sum += temp;
				}
			}
			sum *= 4;
			sum /= 2. * Row;
			result[i][j] = round(sum);	// transform type of sum
			sum = 0;
		}
	}
	printf("IDCT end\n");
}




/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) { // argv[1]:inimage argv[2]:outimage argv[3]:COL argv[4]:ROW
	uchar** img, ** Result, **Result2;
	int Col, Row;
	int i, sel;		// 1:DCT 2:DFT
	double** temparray;
	
	if (argc != 5) { // notice input form if wrong parameters are delivered
		fprintf(stderr, "Input Form:\n%s inImage outImage COL ROW\n", argv[0]);
		exit(0);
	}

	Col = atoi(argv[3]);
	Row = atoi(argv[4]);
	temparray = (double**)calloc(sizeof(double*), Row);
	for (i = 0; i < Row; i++) {
		temparray[i] = (double*)calloc(sizeof(double), Col);
	}
	img = uc_alloc(Col, Row);
	Result = uc_alloc(Col, Row);
	Result2 = uc_alloc(Col, Row);
	read_ucmatrix(Col, Row, img, argv[1]);
	printf("Select a transform...\n");
	printf("1 : Discrete Cosine Transform\n");
	printf("2 : DCT, IDCT Transform\n");
	scanf("%d", &sel);

	switch (sel) {
	case 1:		// DCT
		DCT(img, Result, Row, Col, temparray);
		break;
	case 2:		// IDCT
		DCT(img, Result, Row, Col, temparray);
		IDCT(temparray, Result, Row, Col);
		break;
	}
	write_ucmatrix(Col, Row, Result, argv[2]);
	printf("writing done\n");
	uc_free(Col, Row, img);
	uc_free(Col, Row, Result);
	return 0;
}