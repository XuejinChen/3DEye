#pragma once

#include <vector>
#include <deque>
#include <string>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <deque>

/*
��������B��ͼƬ�Ĵ�С��Ϊ720*576���Ѳ��������޶������µķ�Χ��
*/
#define START_ROW 60
#define END_ROW 510
#define START_COL 70
#define END_COL 640
#define MAXN 5000

#define BEGIN 50 //��������ǰ�˵ļ�ⷶΧ
#define END 240 //����������˵ļ�ⷶΧ����ĩ�˼��̫���˿����ʵ����ϵ���
#define MAX_LENGTH 400 //�����������ȵ�����
#define BLUR Size(13, 13) //��ֵ�˲��Ĳ���������������ñ���һ�£��������������������Խ��˲����ʵ������ϵ���
#define CUTOFF 10 //���߽�ȥͷβ���˵ĳ���

using namespace std;
using namespace cv;

bool labeling(Mat grayImg, Mat &labelImg, int bin_th);
bool contour(Mat binaryImg, vector<vector<cv::Point> > &contours);
vector<cv::Point> _3dtrace(string filename, vector<cv::Point3f> &trace, float &r, float theta = 0.0);

/*
����������������ʵ��ʹ��ʱֻ��Ҫ����_3dtrace�������ɣ����в���&trace����ά����������
Ŀǰ��ʵ������ά�����ӳ�䣬thetaΪtrace��Y����ת�ĽǶȣ����ȣ���rΪ���ߵİ뾶
����ֵ���Բ��ùܣ�����Ϊ����ʹ��
ʹ��ǰҪȷ���Ѱ���opencv��
*/