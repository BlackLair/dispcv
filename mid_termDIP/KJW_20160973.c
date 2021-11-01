/* 2021-2�б� ����ó�� �߰���� �ۼ����� : 2021.11.01
20160973 ������*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>
#include<string.h>

#pragma warning(disable:4996)

typedef unsigned char uchar;


/*********************Standard Func*******************************/
uchar** uc_alloc(int size_x, int size_y) { // 2�� �迭 �޸� �Ҵ�
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

void uc_free(int size_x, int size_y, uchar** ucmatrix) { // 2�� �迭�� �޸� ����
	int i;
	for (i = 0; i < size_y; i++)
		free(ucmatrix[i]);
	free(ucmatrix);
}

void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { //�ϵ��ũ���� �̹��������� �о� ������
	int i;
	FILE* f;

	if ((f = fopen(filename, "rb")) == NULL) { // read binary
		printf("%s File open Error!\n", filename);
		exit(-1);
	}
	for (i = 0; i < size_y; i++) {
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) { // ������ �о� ucmatrix�� ����
			printf("Data Read Error!\n");
			exit(-1);
		}
	}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) { //���Ͽ� ucmatrix�� �ۼ�
	int i;
	FILE* f;

	if ((f = fopen(filename, "wb")) == NULL) { // write binary
		printf("%s File open Error!\n", filename);
		exit(-1);
	}
	for (i = 0; i < size_y; i++) { // ���Ͽ� ������ ����
		if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) { // �� ���� �� �پ� �ۼ�
			printf("Data Write Error!\n");
			exit(-1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/****************************************Sub Func***********************************/
int isInCircle(int i, int j, int rowPos, int colPos, int diameter) { // (i, j)�� �������� �� �̳��ϰ�� 1 ����
	double tmp, xSquare, ySquare;									// �׷��� ������ 0 ����
	ySquare = (rowPos - i) * (rowPos - i);
	xSquare = (colPos - j) * (colPos - j);
	tmp = sqrt(ySquare + xSquare);
	if (tmp <= diameter) return 1;
	else return 0;
}
int isInImage(int i, int j, int Row, int Col) { // (i, j)�� �̹��� ũ�� �̳��� ���Ե� ��� 1 ����
	if (i > 0 && j > 0 && i < Row && j < Col)	// �׷��� ���� ��� 0 ����
		return 1;
	else
		return 0;
}
double average(uchar** img, int Row, int Col, int rowPos, int colPos, int diameter) { // �̹��� ��ü �ȼ����� ��հ� ��ȯ
	int i, j, sum = 0, cnt=0;
	double avg;
	for (i = rowPos-diameter; i <= rowPos+diameter; i++) { // �ʿ���� ���� Ƚ���� ����
		for (j = colPos-diameter; j < colPos+diameter; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)) { // ���� ���� �� �̳��� ��հ��� ���Ѵ�.
				sum += img[i][j];
				cnt++;
			}
		}
	}
	avg = sum / cnt;
	return avg;
}
/////////////////////////////////////////////////////////////////////////////////////
/****************************************Effect Func*********************************/
void Negative(int Row, int Col, int rowPos, int colPos, int diameter, uchar** img, uchar** Result) { // ��� ���� �Լ�
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInImage(i, j, Row, Col)) { // (i, j)�� ��ü �̹������� ������� Ȯ��
				if (isInCircle(i, j, rowPos, colPos, diameter)) // ���� ���� ���� ��츸 ��ȯ
					Result[i][j] = 255 - img[i][j];
				else
					Result[i][j] = img[i][j]; // ���� ���� ���� ��� ���� ����
			}
		}
	}
}
void mosaic(int Row, int Col, int rowPos, int colPos, int diameter, int Block_size, uchar** img, uchar** Result) {
	int i, j, x, y;
	int tmp = 0, cnt = 0;
	for (i = 0; i < Row; i += Block_size) { // �࿡ ���Ͽ� ��� ������ ����
		for (j = 0; j < Col; j += Block_size) { // ���� ���Ͽ� ��� ������ ����
			for (y = 0; y < Block_size; y++) { // ��� �� �࿡ ���Ͽ� �ݺ�
				for (x = 0; x < Block_size; x++) { // ��� �� ���� ���Ͽ� �ݺ�
					if (j + x < Row && i + y < Col) { // �̹��� ������ ����� �ʴ� ������ ����
						tmp += img[i + y][j + x];	// ��� �� �� �ȼ����� ������ ����
						cnt++;		// ��� �� �ȼ��� ���� ����
					}
				}
			}
			tmp /= cnt;	// ����� �� �ȼ����� ���ġ
			for (y = 0; y < Block_size; y++) {
				for (x = 0; x < Block_size; x++) {
					if (j + x < Row && i + y < Col) { // �̹��� ������ ����� �ʴ� ������ ����
						if (isInCircle(i + y, j + x, rowPos, colPos, diameter))
							Result[i + y][j + x] = tmp;	// ��� �� �ȼ����� ��� ���ġ�� ġȯ
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
void blur(int Row, int Col, int rowPos, int colPos, int diameter, int Block_size, uchar** img, uchar** Result) {
	int i, j, x, y;
	int tmp = 0, cnt = 0;
	for (i = 0; i < Row; i++) { // �࿡ ���Ͽ� �ݺ�
		for (j = 0; j < Col; j++) { // ���� ���Ͽ� �ݺ�
			for (y = i - Block_size; y <= i + Block_size; y++) { // ��� �ȼ� �»�ܺ��� ���� ���Ͽ� �ݺ�
				for (x = j - Block_size; x <= j + Block_size; x++) { // ��� �ȼ� �»�ܺ��� �࿡ ���Ͽ� �ݺ�
					if (x >= 0 && y >= 0 && x < Col && y < Row) { // �̹��� ������ ���� �ʴ� �κи� ����
						tmp += img[y][x]; // ��� �ȼ� �ֺ� ����������ŭ�� ������ ��� ����
						cnt++;
					}
				}
			}
			if (isInCircle(i, j, rowPos, colPos, diameter))
				Result[i][j] = (int)tmp / cnt; // ��� �ȼ��� ���� �ֺ� �������� ������ ������� ġȯ
			else
				Result[i][j] = img[i][j];
			tmp = 0;
			cnt = 0;
		}
	}
}
void makeBinary(int Row, int Col, int rowPos, int colPos, int diameter, double avg, uchar** img, uchar** Result) {
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (isInCircle(i, j, rowPos, colPos, diameter)) {
				if (img[i][j] > avg) Result[i][j] = 255;	// ���� �̹����� �ȼ� ��Ⱑ ��պ��� ũ�� �ִ밪���� ġȯ
				else Result[i][j] = 0;						// ��պ��� ������ 0���� ġȯ
			}
			else
				Result[i][j] = img[i][j];
		}
	}
}









////////////////////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[]) {
	uchar** img, ** Result;
	int Col, Row, sel;
	int Block_size, rowPos, colPos, diameter;
	double avg;
	if (argc != 9) { // 
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
	printf("Input Image name : %s    Output Image name : %s\n", argv[1], argv[2]);
	printf("Size of input Image : %d", diameter);
	printf("Center of Section Position : (Row:%d, Col:%d)\n", rowPos, colPos);
	printf("Diameter of Section : %d\n", diameter);
	//////////////////////////////////////////////////////////////////////
	/********************************select effect***********************/
	printf("Which effect do you want?\n");
	printf("1. Negative\n2. Mosaic\n3. Blur\n4. makeBinary\n5. AdaptiveBinary\n");
	printf("6. PowImg\n7. BitSlicing\n8. MaskOr\n>");
	scanf("%d", &sel);
	//////////////////////////////////////////////////////////////////////
	switch (sel) {
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
	}

	write_ucmatrix(Col, Row, Result, argv[2]);

	uc_free(Col, Row, img);
	uc_free(Col, Row, Result);
	return 0;
}