/**
* CS4185/CS5185 Multimedia Technologies and Applications
* Course Assignment
* Image Retrieval Project
*/

#include <iostream>
#include<math.h>
#include <stdio.h>
#include <algorithm>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/legacy/legacy.hpp"

#include "similar.h"



using namespace std;
using namespace cv;

#define IMAGE_LIST_FILE "inputimage.txt"  // NOTE: this is relative to current file

//Compute pixel-by-pixel difference

double pixelDis(Mat img1, Mat img2)
{
	int w = img1.cols, h = img1.rows;
	Mat new_img2;
	resize(img2, new_img2, img1.size());
	Mat gray1,gray2;
	cvtColor(img1, gray1, COLOR_BGR2GRAY);
	cvtColor(new_img2, gray2, COLOR_BGR2GRAY);
	medianBlur(gray1, gray1, 5);
	medianBlur(gray2, gray2, 5);
	double sum = 0;
	for (int i = 0; i < w; i++)for (int j = 0; j < h; j++)
	{
		sum += abs(gray1.at<uchar>(j, i) - gray2.at<uchar>(j, i));
	}
	sum=sum / (w*h);
	return sum;
}

int lineDetect(Mat img) {
	Mat dst;//, cdst, cdstP;
	Canny(img, dst, 50, 200, 3);
//	cvtColor(dst, cdst, COLOR_GRAY2BGR);
	//cdstP = cdst.clone();
	// Probabilistic Line Transform
	vector<Vec4i> linesP; 
	HoughLinesP(dst, linesP, 1, CV_PI / 180, 50, 150, 10); 
	int count = linesP.size();
	return count;

}
//line diff
double getHist(Mat img1, Mat img2) {
	Mat hsv_img1, hsv_img2;
	cvtColor(img1, hsv_img1, COLOR_BGR2HSV);
	cvtColor(img2, hsv_img2, COLOR_BGR2HSV);
	Mat new_img1, new_img2;
	int x1, y1, x2, y2;
	x1 = hsv_img1.cols / 8;
	y1 = hsv_img1.rows / 8;
	new_img1 = hsv_img1(Rect(x1, y1, hsv_img1.cols - x1, hsv_img1.rows - y1));
	x2 = hsv_img2.cols / 8;
	y2 = hsv_img2.rows / 8;
	new_img2 = hsv_img2(Rect(x2, y2, hsv_img2.cols - x2, hsv_img2.rows - y2));
	int h_bins = 8, s_bins = 2, v_bins = 2;
	int histSize[] = { h_bins, s_bins,v_bins };
	int channels[] = { 0, 1, 2 };
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	float v_ranges[] = { 0,256 };
	const float* ranges[] = { h_ranges, s_ranges,v_ranges };
	Mat hist_img1, hist_img2;
	calcHist(&new_img1, 1, channels, Mat(), hist_img1, 2, histSize, ranges, true, false);
	calcHist(&new_img2, 1, channels, Mat(), hist_img2, 2, histSize, ranges, true, false);
	double result = compareHist(hist_img2, hist_img1, 0);
	return result;
}


int main(int argc, const char** argv)
{
	double h_distance[1000], s_distance[1000], v_distance[1000];
	double h_sort[1000], s_sort[1000], v_sort[1000],sort_total[1000];

	Mat src_input;
	Mat db_img;
	bool has_line[1000],src_has_line,is_similar[1000];
	double his_score[1000], mat_score[1000], line_count[1000];
	int db_id = 0;
	double min_dis = 9999;
	int max_score_num;
	int number;

	printf("1: beach\n");
	printf("2: building\n");
	printf("3: bus\n");
	printf("4: dinosaur\n");
	printf("5: flower\n");
	printf("6: horse\n");
	printf("7: man\n");
	printf("Type in the number to choose a category and type enter to confirm\n");
	scanf_s("%d", &number);

	switch (number) {
	case 1:
		src_input = imread("beach.jpg");  // read input image
		printf("You choose: %d - beach\n", number);
		break;
	case 2:
		src_input = imread("building.jpg");
		printf("You choose: %d - building\n", number);
		break;
	case 3:
		src_input = imread("bus.jpg");
		printf("You choose: %d - bus\n", number);
		break;
	case 4:
		src_input = imread("dinosaur.jpg");
		printf("You choose: %d - dinosaur\n", number);
		break;
	case 5:
		src_input = imread("flower.jpg");
		printf("You choose: %d - flower\n", number);
		break;
	case 6:
		src_input = imread("horse.jpg");
		printf("You choose: %d - horse\n", number);
		break;
	case 7:
		src_input = imread("man.jpg");
		printf("You choose: %d - man\n", number);
		break;
	}

	if (!src_input.data)
	{
		printf("Cannot find the input image!\n");
		system("pause");
		return -1;
	}
	imshow("Input", src_input);

	int src_line = lineDetect(src_input);
	if (src_line== 0)
		src_has_line = false;
	else
		src_has_line = true;
	///Read Database
	FILE* fp;
	char imagepath[200];
	//imwrite("ans/src.jpg", src_input);
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting features from input images...\n");
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			printf("%s\n", imagepath);
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);

			db_img = imread(tempname); // read database image
			if (!db_img.data)
			{
				printf("Cannot find the database image number %d!\n", db_id + 1);
				system("pause");
				return -1;
			}
			Mat new_img2;
			resize(db_img,new_img2, src_input.size(), 5);
			//Apply histogram comparison
			his_score[db_id]=getHist(src_input,db_img);
			//cout << "Hist Result =" << his_score[db_id]<< endl;
			//compare distance between pixels
			mat_score[db_id] = pixelDis(src_input, db_img);
			//cout << "Distance: " << mat_score[db_id] << endl;
			//Detect number of lines in the image 
			line_count[db_id] = lineDetect(db_img);
			//cout << "Line detected= " << line_count[db_id] << endl;
			if (line_count[db_id] == 0) {
				has_line[db_id] = false;
			}
			else
				has_line[db_id] = true;
			db_id++;
		}
	}
	fclose(fp);
	


	int target;
	switch (number) {
	case 1:
		target = 1;
		break;
	case 2:
		target = 2;
		break;
	case 3:
		target = 3;
		break;
	case 4:
		target = 4;
		break;
	case 5:
		target = 6;
		break;
	case 6:
		target = 7;
		break;
	case 7:
		target = 0;
		break;
	}

	int hist_count = 0;
	double mean = 0, sum = 0, dis_mean = 0;//dis_sum = 0;
	vector <int> result;
	double min_hist = 0.3;
	//calculate the mean of histogram , median_distance of images with good histogram scoree
	for (int i = 0; i < 100; i++) {
		//cout << "x= " << i << " :";
		for (int j = 0; j < 10; j++) {
		//	printf( "%0.3f",his_score[i*10+j]);
			if (his_score[i * 10 + j] > min_hist) {
				hist_count++;
				result.push_back(mat_score[i * 10 + j]);
				sum += his_score[i * 10 + j];
				//dis_sum += mat_score[i * 10 + j];
			}
			//cout << " ";
		}
		//cout << endl;
	}
	int median = 0;
	median = result.at(hist_count / 2);
	mean = sum / hist_count;
	//cout << "mean= " << mean<<endl;
	//dis_mean = dis_sum / hist_count;



	int img_line_count = lineDetect(src_input);
	//cout << "Base Line detected= " << img_line_count << endl;
	//imshow("Best Match Image", max_mat_img);

	//calculate the mean of histogram and 



	//check for best match
	double max_score = 0;
	max_score_num = 0;
	for (int i = 0; i < 1000; i++) {	
			is_similar[i] = false;
			if (his_score[i]> max_score&&mat_score[i] <= median-1) {
				max_score = his_score[i];
				max_score_num = i;
			}
	}
	int corr_count = 0,mat_count=0;
	//check for similarity
	for (int i = 0; i < 1000; i++) {
		if (!src_has_line) {
			if (line_count[i] < 1) { 	
				mat_count++; 
				is_similar[i] = true;
				if (i >= target * 100 && i < (target + 1) * 100) {
					corr_count++;
				}
			}
		
		} else	if (his_score[i] > mean&&mat_score[i] <= 78) {
					mat_count++;
					is_similar[i] = true;
					if (i >= target * 100 && i < (target + 1) * 100) {
						corr_count++;
					}
		}

	}
	cout << "Total Pass = " << mat_count << endl;
	cout << "Target Pass = " <<corr_count << endl;
	cout << "pricision = " << (double)corr_count/ (double)mat_count;
	cout << "recall = " << (double)corr_count / 100;
	char max_name[200];
	sprintf_s(max_name, 200, "../image.orig/%d.jpg", max_score_num+1);
	//cout << "Max name  = " << max_name<<endl;
	cout << " Most similar image is " << max_score_num+1<<endl;
	Mat max_img = imread(max_name);
	imshow("Best Match Image", max_img);
	
	saveImg(is_similar);
	printf("Done \n");
	// Wait for the user to press a key in the GUI window.
	//Press ESC to quit
	int keyValue = 0;
	while (keyValue >= 0)
	{
		keyValue = cvWaitKey(0);

		switch (keyValue)
		{
		case 27:keyValue = -1;
			break;
		}
	}

	return 0;
}


