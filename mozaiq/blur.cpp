#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>

#pragma warning(disable:4996)

typedef unsigned char uchar;

void blur(uchar** img, uchar** out, int Row, int Col, int ratio) {
	int i, j, x, y;
	int tmp = 0, cnt = 0;
	for (i = 0; i < Row; i++) { // �࿡ ���Ͽ� �ݺ�
		for (j = 0; j < Col; j++) { // ���� ���Ͽ� �ݺ�
			for (y = i - ratio; y <= i + ratio; y++) { // ��� �ȼ� �»�ܺ��� ���� ���Ͽ� �ݺ�
				for (x = j - ratio; x <= j + ratio; x++) { // ��� �ȼ� �»�ܺ��� �࿡ ���Ͽ� �ݺ�
					if (x >= 0 && y >= 0 && x < Col && y < Row) { // �̹��� ������ ���� �ʴ� �κи� ����
						tmp += img[y][x]; // ��� �ȼ� �ֺ� ����������ŭ�� ������ ��� ����
						cnt++;
					}
				}
			}
			out[i][j] = (int) tmp / cnt; // ��� �ȼ��� ���� �ֺ� �������� ������ ������� ġȯ
			tmp = 0;
			cnt = 0;
		}
	}
}