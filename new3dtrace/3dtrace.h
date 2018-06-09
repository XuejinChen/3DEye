#pragma once

#include <vector>
#include <deque>
#include <string>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <deque>

/*
所有眼球B超图片的大小均为720*576，把查找区域限定在以下的范围内
*/
#define START_ROW 60
#define END_ROW 510
#define START_COL 70
#define END_COL 640
#define MAXN 5000

#define BEGIN 50 //代表轮廓前端的检测范围
#define END 240 //代表轮廓后端的检测范围，若末端检测太短了可以适当向上调整
#define MAX_LENGTH 400 //代表轮廓长度的上限
#define BLUR Size(13, 13) //均值滤波的参数，两个整数最好保持一致，如果轮廓查找有问题可以将此参数适当地向上调整
#define CUTOFF 10 //曲线截去头尾两端的长度

using namespace std;
using namespace cv;

bool labeling(Mat grayImg, Mat &labelImg, int bin_th);
bool contour(Mat binaryImg, vector<vector<cv::Point> > &contours);
vector<cv::Point> _3dtrace(string filename, vector<cv::Point3f> &trace, float &r, float theta = 0.0);

/*
这里有三个函数，实际使用时只需要调用_3dtrace函数即可，其中参数&trace即三维的轮廓坐标
目前已实现了三维坐标的映射，theta为trace绕Y轴旋转的角度（弧度），r为经线的半径
返回值可以不用管，仅作为调试使用
使用前要确认已包含opencv库
*/