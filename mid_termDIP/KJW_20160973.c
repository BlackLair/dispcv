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

int main(int argc, char* argv[]) {
	uchar** img, ** Result;
	int Col, Row;
	if (argc != 9) { // 
		fprintf(stderr, "Input Form:\n%s inImage outImage COL ROW Block_size row_pos col_pos diameter\n", argv[0]);
		exit(0);
	}
	Col = atoi(argv[3]);
	Row = atoi(argv[4]);
	img = uc_alloc(Col, Row);
	Result = uc_alloc(Col, Row);
	read_ucmatrix(Col, Row, img, argv[1]);

	return 0;
}