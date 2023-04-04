
#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<opencv2\opencv.hpp>
using namespace cv;
#define var uchar
 
var **conv2(var **img, int w[3][3], int rows, int cols, int bm=3, int bn=3) {
	int n1 = rows + bm - 1;
	int n2 = cols + bn - 1;
	var **result = (var**)malloc(n1 * sizeof(var*));
	for (int i = 0; i < n1; i++) {
		result[i] = (var*)malloc(n2 * sizeof(var*));
		for (int j = 0; j < n2; j++) {
			int sum = 0;
			for (int m = 0; m < bm; m++) {
				for (int n = 0; n < bn; n++) {
					int rm = i - m;
					int rn = j - n;
					 /* Complemento 0 procesamiento */
					if (rm >= 0 && rm<rows&&rn >= 0 && rn<cols)
						sum += img[rm][rn] * w[m][n];
				}
			}
			 / * Tome el valor absoluto, 255 se considera 255 * /
			sum = abs(sum);
			result[i][j] = sum;
			if (sum > 255) result[i][j] = 255;
		}
	}
	return result;
}
 
 /* Convertir imagen a matriz */
var **mat2array(Mat &src) {
	int rows = src.rows;
	int cols = src.cols;
	var **result = (var**)malloc(rows * sizeof(var*));
 
	//double sum = 0;
	for (int i = 0; i < rows; i++) {
		uchar *data = src.ptr<uchar>(i);
		result[i] = (var*)malloc(cols * sizeof(var*));
		for (int j = 0; j < cols ; j++) {
			int temp = data[j];
			result[i][j] = temp;
		}
	}
	return result;
}
 
 /* Convertir matriz a imagen */
Mat array2mat(var **result,int rows,int cols) {
	 /* Mantenga el mismo tamaño que la imagen original */
	Mat src = Mat(rows-2, cols-2, 0);
	for (int i = 2; i < rows; i++) {
		uchar *data = src.ptr<uchar>(i-2);
		for (int j =2; j < cols; j++) {
			data[j-2]=result[i][j];
		}
	}
	return src;
}
 
Mat addxy(Mat &src1, Mat &src2) {
	int rows = src1.rows > src2.rows ? src1.rows : src2.rows;
	int cols = src1.cols > src2.cols ? src1.cols : src2.cols;
	Mat dst = Mat(rows,cols,0);
	for (int i = 0; i < rows; i++) {
		uchar *data = dst.ptr<uchar>(i);
		uchar *d1 = src1.ptr<uchar>(i);
		uchar *d2 = src2.ptr<uchar>(i);
		for (int j = 0; j < cols; j++) {
			int sum = sqrt(d1[j] * d1[j] + d2[j] * d2[j]);
			if (sum > 255) data[j] = 255;
			else data[j] = sum;
		}
	}
	return dst;
}
 
int main() {
	 /* Operador Sobel */
	int sx[3][3] = {
		{ -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 },
	};
	int sy[3][3] = {
		{ 1, 2, 1 },
	{ 0, 0, 0 },
	{ -1, -2, -1 },
	};
	 / * Para un cálculo fácil, lea en imagen en escala de grises * /
	Mat img = imread("D:/Personal/Desktop/ip.png",0);
	 / * Determinar si la imagen existe * /
	if (img.rows*img.cols==0) return 1;
	 imshow ("imagen original", img);
	var **array = mat2array(img);
	 / * Calcular gradiente utilizando el operador sobel * /
	var **gradx = conv2(array, sx, img.rows, img.cols);
	var **grady = conv2(array, sy, img.rows, img.cols);
	Mat src1 = array2mat(gradx, img.rows, img.cols);
	Mat src2 = array2mat(grady, img.rows, img.cols);
	 imshow ("Imagen de entidad horizontal", src1);
	 imshow ("Imagen de entidad vertical", src2);
	Mat dst = addxy(src1, src2);
	 / * Superposición de imagen * /
	 imshow ("Imagen de detección de bordes", dst);
	for (int i = 0; i < img.rows; i++) {
		free(array[i]);
		free(gradx[i]);
		free(grady[i]);
	}
	free(array);
	free(gradx);
	free(grady);
	waitKey();
}