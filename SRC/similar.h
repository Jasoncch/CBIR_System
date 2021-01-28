#pragma 
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
#include <windows.h>
using namespace std;
using namespace cv;
#define IMAGE_LIST_FILE "inputimage.txt"  // NOTE: this is relative to current file

void saveImg(bool check[])
{
	Mat db_img;
	LPCSTR folder = "ans";
	CreateDirectoryA(folder, NULL);
	int db_id = 0;
	///Read Database
	FILE* fp;
	char imagepath[200];
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Saving images to folder..\n");
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);
			db_img = imread(tempname);
			//string savingName;// = "ans/" + to_string(db_id) + ".jpg";
			
			char folder_name[200];
			sprintf_s(folder_name,200, "ans/%d.jpg", db_id+1);
			if (check[db_id]) {
				printf("Saving %s \n" ,folder_name);
				imwrite(folder_name, db_img);
			}
			db_id++;
		}
	}
	//imwrite("ans.tiff",output);
	fclose(fp);
}