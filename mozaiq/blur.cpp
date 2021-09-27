#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>

#pragma warning(disable:4996)

typedef unsigned char uchar;

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
			out[i][j] = (int) tmp / cnt; // 대상 픽셀의 값을 주변 지정범위 값들의 평균으로 치환
			tmp = 0;
			cnt = 0;
		}
	}
}