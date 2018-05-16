#pragma once

#include <vector>
#include <deque>
#include <string>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <cmath>

/*
��������B��ͼƬ�Ĵ�С��Ϊ720*576���Ѳ��������޶������µķ�Χ��
*/
#define START_ROW 70
#define END_ROW 510
#define START_COL 70
#define END_COL 640
#define MAXN 5000
#define LENGTH 10
#define BEGIN 50
#define END 220
#define BLUR Size(11, 11)

using namespace std;
using namespace cv;

bool labeling(Mat grayImg, Mat &labelImg, int bin_th);
bool contour(Mat binaryImg, vector<vector<cv::Point> > &contours);
bool _3dtrace(string filename, vector<cv::Point3f> &trace);

/*
����������������ʵ��ʹ��ʱֻ��Ҫ����_3dtrace�������ɣ����в���&trace����ά����������
Ŀǰֻʵ���˶�ά����Ĳ��ң�trace[i].z��ȫ����Ϊ0

ʹ��ǰҪȷ���Ѱ���opencv��
*/