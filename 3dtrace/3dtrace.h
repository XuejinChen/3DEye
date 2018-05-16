#pragma once

#include <vector>
#include <deque>
#include <string>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <cmath>

/*
所有眼球B超图片的大小均为720*576，把查找区域限定在以下的范围内
*/
#define START_ROW 70
#define END_ROW 510
#define START_COL 70
#define END_COL 640
#define MAXN 5000
#define LENGTH 10
#define BEGIN 50
#define END 230
#define MAX_LENGTH 350
#define BLUR Size(13, 13)

using namespace std;
using namespace cv;

bool labeling(Mat grayImg, Mat &labelImg, int bin_th);
bool contour(Mat binaryImg, vector<vector<cv::Point> > &contours);
bool _3dtrace(string filename, vector<cv::Point3f> &trace);

/*
这里有三个函数，实际使用时只需要调用_3dtrace函数即可，其中参数&trace即三维的轮廓坐标
目前只实现了二维坐标的查找，trace[i].z已全部设为0

使用前要确认已包含opencv库
*/
