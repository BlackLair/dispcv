/* 2021-2학기 영상처리 중간고사 작성일자 : 2021.11.01
20160973 권정우*/
/* DIP mid term exam */
#include<stdio.h>
#include<stdlib.h>
#include<math.h> // need -lm option if you compile in linux environment
#include<assert.h>
#include<string.h>

#pragma warning(disable:4996) // in visual studio, scanf function causes C4996 warning. This macro solve this problem. 

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

/////////////////////////////////////////////////////////////////////////////////////
/****************************************Sub Func***********************************/
int isInCircle(int i, int j, int rowPos, int colPos, int diameter) { // if(i, j) is included in the circle section:return 1
	double tmp, xSquare, ySquare;									// else, return 0
	ySquare = (rowPos - i) * (rowPos - i);
	xSquare = (colPos - j) * (colPos - j);
	tmp = sqrt(ySquare + xSquare);
	if (tmp <= diameter) return 1;
	else return 0;
}
int isInImage(int i, int j, int Row, int Col) { // if (i, j) is included in the input Image:return 1
	if (i > 0 && j > 0 && i < Row && j < Col)	// else, return 0
		return 1;
	else
		return 0;
}
double average(uchar** img, int Row, int Col, int rowPos, int colPos, int diameter) { // return average of the circle section
	int i, j, sum = 0, cnt=0;
	double avg;
	for (i = rowPos-diameter; i <= rowPos+diameter; i++) { // reduce unnecessary calculation
		for (j = colPos-diameter; j < colPos+diameter; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)&&isInImage(i, j, Row, Col)) { // check is (i, j) is valid
				sum += img[i][j]; // add all pixels' brightness values of the circle section
				cnt++; // count number of pixels in the circle section
			}
		}
	}
	avg = sum / cnt; // get average brightness of circle section
	return avg;
}
void inttoa(int i, char *namenum) { // transform type int(0~9) to char
	sprintf(namenum, "%d", i);
}
/////////////////////////////////////////////////////////////////////////////////////
/****************************************Effect Func*********************************/
// Negative: reverse brightness of circle section
void Negative(int Row, int Col, int rowPos, int colPos, int diameter, uchar** img, uchar** Result) { 
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInImage(i, j, Row, Col)) { // check (i, j) is included in image
				if (isInCircle(i, j, rowPos, colPos, diameter)) // check (i, j) is included in the circle section
					Result[i][j] = 255 - img[i][j];
				else
					Result[i][j] = img[i][j]; // keep original brightness outside of the circle section
			}
		}
	}
}
// mosaic : mosaic brightness of circle section by block size
void mosaic(int Row, int Col, int rowPos, int colPos, int diameter, int Block_size, uchar** img, uchar** Result) {
	int i, j, x, y;
	int tmp = 0, cnt = 0;
	for (i = 0; i < Row; i += Block_size) { // block-by-block loop of rows
		for (j = 0; j < Col; j += Block_size) { // block-by-block loop of columns
			for (y = 0; y < Block_size; y++) { // pixel-by-pixel loop of rows
				for (x = 0; x < Block_size; x++) { // pixel-by-pixel loop of columns
					if (isInImage(i+y, j+x, Row, Col)) { // check (i+y, j+x) is included in the input Image
						tmp += img[i + y][j + x];	// add all brightness values of block
						cnt++;		// count number of pixels in the block
					}
				}
			}
			tmp /= cnt;	// get average of brightness of each block
			for (y = 0; y < Block_size; y++) {
				for (x = 0; x < Block_size; x++) {
					if (isInImage(i+y, j+x, Row, Col)) { // check (i+y, j+x) is included in the input Image
						if (isInCircle(i + y, j + x, rowPos, colPos, diameter)) // check (i+y, j+x) is included in the circle section
							Result[i + y][j + x] = tmp;	// replace block pixels to average brightness of block
						else
							Result[i + y][j + x] = img[i + y][j + x]; // keep original brightness outside of the circle section
					}
				}
			}
			tmp = 0; // initialize for next loop calculation
			cnt = 0;
		}
	}
}
// blur : blur brightness of circle section by block size ( each pixel have different block )
void blur(int Row, int Col, int rowPos, int colPos, int diameter, int Block_size, uchar** img, uchar** Result) {
	int i, j, x, y;
	int tmp = 0, cnt = 0;
	for (i = 0; i < Row; i++) { 
		for (j = 0; j < Col; j++) { 
			for (y = i - Block_size; y <= i + Block_size; y++) { // loop through row from top left of target pixel's block
				for (x = j - Block_size; x <= j + Block_size; x++) { // Repeat through columns from top left of target pixel's block
					if (isInImage(y, x, Row, Col)) { // check (y, x) is included in the input image
						tmp += img[y][x]; // add all brightness values of block
						cnt++; // count number of pixels in the block
					}
				}
			}
			if (isInCircle(i, j, rowPos, colPos, diameter)) // check (i, j) is included in the circle section
				Result[i][j] = (int)tmp / cnt; // replace the pixel's value to average of block(each pixels have different block section)
			else
				Result[i][j] = img[i][j]; // keep original brightness outside of the circle section
			tmp = 0; // initialize for next loop calculation
			cnt = 0;
		}
	}
}
// makeBinary : calculate average brightness of circle section and 
void makeBinary(int Row, int Col, int rowPos, int colPos, int diameter, double avg, uchar** img, uchar** Result) {
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)) { // check (i, j) is included in the circle section
				if (img[i][j] > avg) Result[i][j] = 255;	// replace pixels brightness to 255 which are over average brightness of the circle section
				else Result[i][j] = 0;						// replace pixels brightness to 0 which are under average brightness of the circle section
			}
			else
				Result[i][j] = img[i][j]; // keep original brightness outside of the circle section
		}
	}
}
// AdaptiveBinary0 : Replace brightness in range to threshold, Replace brightness out of range to threshold
// AdaptiveBinary1 : Replace brightness in range to threshold, Keep original brightness out of range 
// AdaptiveBinary2 : Keep original brightness out of range, Replace brightness in range to 0
void AdaptiveBinary0(int Row, int Col, int rowPos, int colPos, int diameter, int min, int max, int threshold, uchar** img, uchar** Result) { 
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)) {
				if (img[i][j] > min && img[i][j] < max) Result[i][j] = threshold; // check (i, j) is in range and replace to threshold
				else Result[i][j] = 0;											// replace to 0 out of range
			}
			else
				Result[i][j] = img[i][j];	// keep original out of the circle section
		}
	}
}
void AdaptiveBinary1(int Row, int Col, int rowPos, int colPos, int diameter, int min, int max, int threshold, uchar** img, uchar** Result) { 
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)) {
				if (img[i][j] > min && img[i][j] < max) Result[i][j] = threshold; // check (i, j) is in range and replace to threshold
				else Result[i][j] = img[i][j];									// keep original out of range
			}
			else
				Result[i][j] = img[i][j];	// keep original out of the circle section
		}
	}
}
void AdaptiveBinary2(int Row, int Col, int rowPos, int colPos, int diameter, int min, int max, int threshold, uchar** img, uchar** Result) { 
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)) {
				if (img[i][j] > min && img[i][j] < max) Result[i][j] = img[i][j]; // check (i, j) is in range and keep original
				else Result[i][j] = 0;											// replace to 0 out of range
			}
			else
				Result[i][j] = img[i][j];	// keep original out of the circle section
		}
	}
}
// PowImg : set Gray Level
void PowImg(int Row, int Col, int rowPos, int colPos, int diameter, uchar** img, uchar** Result, double gamma) {
	int i, j;
	double tmp;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)) {
				tmp = pow(img[i][j] / 255., 1 / gamma); // divide original brightness by 255, and powered by 1/gamma
				if (tmp * 255 > 255) tmp = 1.;	// set gamma 255/255 which are over 255/255
				else if (tmp * 255 < 0) tmp = 0.; // set brightness 0 which are under 0
				Result[i][j] = tmp * 255; 
			}
			else
				Result[i][j] = img[i][j];	// keep original out of the circle section
		}
	}
}
// BitSlicing : extract the images by each bit digit
void BitSlicing(int Row, int Col, int rowPos, int colPos, int diameter, uchar** img, uchar** Result, const char* filename) {
	int i, j, n;
	char resultname[32]; // for set each file name different
	char namenum[2];
	uchar mask = 0x01;	// for read each bit digit with bit shifting and bit operation

	for (n = 0; n < 8; n++) {
		memset(resultname, '\0', 32); // initialize for the output file name
		memset(namenum, '\0', 2);
		strcpy(resultname, filename); // copy the output file name
		for (i = 0; i < Row; i++) {
			for (j = 0; j < Col; j++) {
				if (isInCircle(i, j, rowPos, colPos, diameter)) {
					if ((mask & img[i][j]) == pow(2, n)) // check nth bit digit is 1 or 0
						Result[i][j] = 255;
					else Result[i][j] = 0;
				}
				else 
					Result[i][j] = img[i][j];	// keep original out of the circle section
			}
		}
		inttoa(n, namenum);
		// itoa function is written in stdlib.h by MS independently. So it is not included in standard C library.
		// in this source code, inttoa function is defined above.
		strcat(resultname, namenum);	// Bit number at the end of the name of the result file's name
		write_ucmatrix(Col, Row, Result, resultname); // write result file
		mask <<= 1; // bit shift 1 bit left
	}

}

////////////////////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[]) {
	uchar** img, ** Result;
	int Col, Row, sel;
	int min, max, threshold;
	int isDone = 0;
	int Block_size, rowPos, colPos, diameter;
	double avg, gamma;
	if (argc != 9) { // notice input form if wrong parameters are delivered
		fprintf(stderr, "Input Form:\n%s inImage outImage COL ROW Block_size row_pos col_pos diameter\n", argv[0]);
		exit(0);
	}
	/******************** transform input parameters to integer...********/
	Col = atoi(argv[3]);
	Row = atoi(argv[4]);
	Block_size = atoi(argv[5]);
	rowPos = atoi(argv[6]);
	colPos = atoi(argv[7]);
	diameter = atoi(argv[8]);
	//////////////////////////////////////////////////////////////////////
	/******************matrix allocation and read input image************/
	img = uc_alloc(Col, Row);
	Result = uc_alloc(Col, Row);
	read_ucmatrix(Col, Row, img, argv[1]);
	//////////////////////////////////////////////////////////////////////
	/****************************print input parameter information******/
	printf("==========================mid_termDIP========================\n");
	printf("Input Image name : %s    Output Image name : %s\n", argv[1], argv[2]);
	printf("Size of input Image : Row:%d Col:%d\n", Row, Col);
	printf("Center of Section : (Row:%d, Col:%d)\n", rowPos, colPos);
	printf("Diameter of Section : %d\n", diameter);
	printf("Block Size : %d   (Block size will be used in Mosaic, Blur.)\n", Block_size);
	printf("=============================================================\n\n");
	//////////////////////////////////////////////////////////////////////
	/********************************select effect***********************/
	printf("Which effect do you want?\n");
	printf("1. Negative\n2. Mosaic\n3. Blur\n4. makeBinary\n5. AdaptiveBinary\n");
	printf("6. PowImg\n7. BitSlicing\n>");
	scanf("%d", &sel);
	//////////////////////////////////////////////////////////////////////
	switch (sel) { // execute selected effect
	case 1:
		printf("You selected Negative.\n");
		Negative(Row, Col, rowPos, colPos, diameter, img, Result);
		printf("Negative process finished.\n");
		break;
	case 2:
		printf("You selected Mosaic.\n");
		mosaic(Row, Col, rowPos, colPos, diameter, Block_size, img, Result);
		printf("Mosaic process finished.\n");
		break;
	case 3:
		printf("You selected Blur.\n");
		blur(Row, Col, rowPos, colPos, diameter, Block_size, img, Result);
		printf("Blur process finished.\n");
		break;
	case 4:
		printf("You selected MakeBinary.\n");
		avg = average(img, Row, Col, rowPos, colPos, diameter);
		printf("Average of section : %lf\n", avg);
		makeBinary(Row, Col, rowPos, colPos, diameter, avg, img, Result);
		printf("makeBinary process finished.\n");
		break;
	case 5:
		printf("You selected AdaptiveBinary.\n");
		printf("Input brightness section. ( input Form : min max )\n>");
		scanf("%d %d", &min, &max);
		if (min > max) {
			printf("invalid value...\n");
			uc_free(Col, Row, img); // release matrix memory
			uc_free(Col, Row, Result);
			exit(0);
		}
		printf("Input threshold ( 0~255 ) \n");
		scanf("%d", &threshold);
		if (threshold > 255 || threshold < 0) {
			printf("invalid value...\n");
			uc_free(Col, Row, img); // release matrix memory
			uc_free(Col, Row, Result);
			exit(0);
		}
		printf("Select transform type.\n");
		printf("1. Replace (%d~%d) to %d, Replace rest section to 0\n", min, max, threshold);
		printf("2. Replace (%d~%d) to %d, Keep Rest section Original\n", min, max, threshold);
		printf("3. Keep (%d~%d) Original, Replace rest section to 0\n>", min, max);
		scanf("%d", &sel);
		printf("=======================================\n"); // shows what you selected
		printf("section Brightness min : %d max : %d\n", min, max);
		printf("Threshold : %d\n", threshold);
		printf("transform type : %d\n", sel);
		printf("=======================================\n");
		switch (sel) {
		case 1:
			AdaptiveBinary0(Row, Col, rowPos, colPos, diameter, min, max, threshold, img, Result);
			printf("AdaptiveBinary process finished.\n");
			break;
		case 2:
			AdaptiveBinary1(Row, Col, rowPos, colPos, diameter, min, max, threshold, img, Result);
			printf("AdaptiveBinary process finished.\n");
			break;
		case 3:
			AdaptiveBinary2(Row, Col, rowPos, colPos, diameter, min, max, threshold, img, Result);
			printf("AdaptiveBinary process finished.\n");
			break;
		default:
			printf("invalid seletion...\n");
			uc_free(Col, Row, img); // release matrix memory
			uc_free(Col, Row, Result);
			exit(0);
		}
		break;
	case 6:
		printf("You selected PowImg.\n");
		printf("Input Gamma value.\n>");
		scanf("%lf", &gamma);
		PowImg(Row, Col, rowPos, colPos, diameter, img, Result, gamma);
		printf("PowImg process finished.\n");
		break;
	case 7:
		printf("You selected BitSlicing.\n");
		BitSlicing(Row, Col, rowPos, colPos, diameter, img, Result, argv[2]);
		printf("BitSlicing process finished.\n");
		isDone = 1; // file has already been generated in BitSlicing Function...
		break;
	default:
		printf("invalid selection...\n");
		uc_free(Col, Row, img); // release matrix memory
		uc_free(Col, Row, Result);
		exit(0);
	}
	if(isDone==0)
		write_ucmatrix(Col, Row, Result, argv[2]); // generate output file

	uc_free(Col, Row, img); // release matrix memory
	uc_free(Col, Row, Result);
	return 0;
}