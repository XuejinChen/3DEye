#include "3dtrace.h"

//连通域标记函数
bool labeling(Mat grayImg, Mat &labelImg, int bin_th = 70)
{
	int i, j, k;
	int label;
	int x, y;
	int row = grayImg.rows, col = grayImg.cols;
	int eight[8][2] = { { 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 },{ 0,1 },{ 1,-1 },{ 1,0 },{ 1,1 } };
	int temp, test;
	int ans[MAXN] = { 0 };
	static bool map[MAXN][MAXN];
	Mat output;
	deque<int> queue;
	queue.clear();
	static bool is[MAXN];
	bool flag = true;
	uchar *pg;
	int *pl;
	labelImg.create(Size(col, row), CV_32SC1);
	if (!(row == 576 && col == 720))
	{
		//图像尺寸不是720*576的话，认定为非眼球B超图片
		return false;
	}
	for (i = START_ROW; i < END_ROW; i++)
	{
		pl = labelImg.ptr<int>(i);
		for (j = START_COL; j < END_COL; j++)
		{
			pl[j] = 0;
		}
	}
	output.create(Size(col, row), CV_8UC3);
	memset(is, false, MAXN);
	memset(map, false, MAXN * MAXN);
	label = 1;
	for (i = START_ROW; i < END_ROW; i++)
	{
		pg = grayImg.ptr<uchar>(i);
		pl = labelImg.ptr<int>(i);
		for (j = START_COL; j < END_COL; j++)
		{
			if (pg[j] < bin_th)
			{
				continue;
			}
			temp = label;
			for (k = 0; k < 4; k++)
			{
				x = i + eight[k][0];
				y = j + eight[k][1];
				if (x >= 0 && y >= 0 && y < col)
				{
					if (grayImg.at<uchar>(x, y) < bin_th)
					{
						continue;
					}
					test = labelImg.at<int>(x, y);
					if (temp != label)
					{
						map[temp][test] = true;
						map[test][temp] = true;
					}
					if (test < temp)
					{
						temp = test;
					}
				}
			}
			if (temp == label)
			{
				label++;
				if (label > MAXN)
				{
					return false;
				}
			}
			pl[j] = temp;
		}
	}
	temp = 1;
	while (flag)
	{
		flag = false;
		for (i = 1; i < label; i++)
		{
			if (!is[i])
			{
				flag = true;
				break;
			}
		}
		queue.push_back(i);
		is[i] = true;
		while (!queue.empty())
		{
			test = queue.front();
			queue.pop_front();
			ans[test] = temp;
			for (i = 1; i < label; i++)
			{
				if (!map[test][i] || is[i])
				{
					continue;
				}
				queue.push_back(i);
				is[i] = true;
			}
		}
		temp++;
	}
	for (i = START_ROW; i < END_ROW; i++)
	{
		pl = labelImg.ptr<int>(i);
		for (j = START_COL; j < END_COL; j++)
		{
			pl[j] = ans[pl[j]];
		}
	}
	return true;
}
//利用连通域查找边界
bool contour(Mat binaryImg, vector<vector<cv::Point> > &contours)
{
	int i, j;
	int num = 0;
	int temp;
	int row = binaryImg.rows, col = binaryImg.cols;
	Mat label;
	int *ql;
	bool flag, end;
	if (!labeling(binaryImg, label))
	{
		return false;
	}
	contours.clear();
	contours.resize(MAXN);
	Point work, test;
	Point start[MAXN];
	bool is[MAXN];
	Point search[30] = { Point(0,1),Point(-1,1),Point(-1,0),Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0),Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1), Point(0,1),Point(-1,1),Point(-1,0),Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1) };
	memset(is, false, MAXN);
	//标记各标签的起点
	for (i = 0; i < row; i++)
	{
		ql = label.ptr<int>(i);
		for (j = 0; j < col; j++)
		{
			if (ql[j] && !is[ql[j]])
			{
				start[ql[j]] = Point(i, j);
				is[ql[j]] = true;
				num++;
			}
		}
	}
	for (i = 1; i <= num; i++)
	{
		temp = 0;
		flag = true;
		if (is[i])
		{
			work = start[i];
			contours[i].push_back(work);
			end = false;
			while (flag)
			{
				flag = false;
				temp += 11;
				for (j = temp - 5; j < temp; j++)
				{
					test = work + search[j];
					if (test.x >= 0 && test.x < row && test.y >= 0 && test.y < col)
					{
						if (label.at<int>(test.x, test.y) == i)
						{
							if (test == start[i])
							{
								end = true;
								break;
							}
							flag = true;
							work = test;
							temp = j % 8;
							contours[i].push_back(test);
							break;
						}
					}
				}
				if (!flag && !end)
				{
					test = work + search[temp];
					if (test.x >= 0 && test.x < row&&test.y >= 0 && test.y < col)
					{
						if (label.at<int>(test.x, test.y) == i)
						{
							if (test == start[i])
							{
								end = true;
								continue;
							}
							flag = true;
							work = test;
							temp = j % 8;
							contours[i].push_back(test);
							continue;
						}
					}
					test = work + search[temp + 2];
					if (test.x >= 0 && test.x < row&&test.y >= 0 && test.y < col)
					{
						if (label.at<int>(test.x, test.y) == i)
						{
							if (test == start[i])
							{
								end = true;
								continue;
							}
							flag = true;
							work = test;
							temp = j % 8;
							contours[i].push_back(test);
							continue;
						}
					}
					test = work + search[temp + 1];
					if (test.x >= 0 && test.x < row&&test.y >= 0 && test.y < col)
					{
						if (label.at<int>(test.x, test.y) == i)
						{
							if (test == start[i])
							{
								end = true;
								continue;
							}
							flag = true;
							work = test;
							temp = j % 8;
							contours[i].push_back(test);
							continue;
						}
					}
				}
			}
		}
	}
	return true;
}
int _dist(Point x, Point y)
{
	const Point eight[8] = { Point(-1,-1),Point(-1,0),Point(-1,1),Point(0,1),Point(1,1),Point(1,0),Point(1,-1),Point(0,-1) };
	int i, j;
	int ans;
	for (i = 0; i < 8; i++)
	{
		if (x == eight[i])
		{
			break;
		}
	}
	for (j = 0; j < 8; j++)
	{
		if (y == eight[j])
		{
			break;
		}
	}
	ans = abs(i - j);
	return ans < 8 - ans ? ans : 8 - ans;
}
bool check(Point x, Point y)
{
	if (x == Point(-1, -1))
	{
		if (y == Point(1, 0) || y == Point(1, 1))
		{
			return true;
		}
	}
	if (x == Point(-1, 0) && y == Point(1, 1))
	{
		return true;
	}
	return false;
}
bool check0(Point x, Point y)
{
	if (x == Point(-1, 1))
	{
		if (y == Point(1, 0) || y == Point(1, -1))
		{
			return true;
		}
	}
	if (x == Point(-1, 0) && y == Point(1, -1))
	{
		return true;
	}
	return false;
}
bool check2(Point x, Point y)
{
	if (x == Point(-1, -1))
	{
		return true;
	}
	if (x == Point(-1, 0) && y == Point(1, 1))
	{
		return true;
	}
	return false;
}

float radius(deque<Point3f> &temp)
{
	float a = temp.back().x / 2, b;
	float min = MAXN;
	float ans, move;
	for (int i = 0; i < temp.size(); i++)
	{
		if (abs(temp[i].x - a) < min)
		{
			min = abs(temp[i].x - a);
			b = temp[i].y;
		}
	}
	ans = (a * a + b * b) / (2 * b);
	move = ans - b > 0 ? ans - b : 0;
	for (int i = 0; i < temp.size(); i++)
	{
		temp[i].y += move;
	}
	return ans;
//	return (pow(temp.back().x-temp.front().x,2)/4+pow(;
}
deque<cv::Point3f> align(deque<cv::Point3f> temp)
{
	deque<cv::Point3f> trace;
	trace.clear();
	float tx, ty, tm;
	tx = temp.back().x - temp.front().x;
	ty = temp.back().y - temp.front().y;
	tm = sqrt(tx*tx + ty * ty);
	trace.resize(temp.size());
	trace[0] = Point3f(0, 0, 0);
	for (int i = 1; i < temp.size(); i++)//曲线平移到原点并且旋转对齐
	{
		temp[i].x -= temp[0].x;
		temp[i].y -= temp[0].y;
		trace[i].x = (temp[i].x*tx + temp[i].y*ty) / tm;
		trace[i].y = (temp[i].y*tx - temp[i].x*ty) / tm;
	}
	return trace;
}
//从边界中提取出眼球表面的三维坐标
//参数filename即需要处理的文件名（包含完整路径），trace即三维路径（z坐标默认为0）
//r为经线的半径，如果传入的r==0那么半径大小将由这条曲线自身决定，如果是一个有意义的正数，那么直接用这个数作为半径
//角度theta定义为曲线切片与X-Y平面的夹角，范围为[-pi/2, pi/2]，默认值为0
//返回值为原图中的路径，未经平移旋转，可以忽略
vector<cv::Point> _3dtrace(string filename, vector<cv::Point3f> &trace, float &r, float theta)
{
	Mat test = imread(filename, 0);
	bool flag = false;
	int max = 0;
	int tt = 0;
	int start, len=0;
	int startm = 0, lenm = 0;
	float mycos = cos(theta), mysin = sin(theta), move;
	vector<vector<cv::Point> > contours;
	vector<int> dist;
	deque<cv::Point3f> temp;
	vector<cv::Point> _trace;
	blur(test, test, BLUR);
	contour(test, contours);
	trace.clear();
	//找出最长的一条边界，其中必定包含眼球的轮廓
	for (int i = 1; i < contours.size(); i++)
	{
		if (max < contours[i].size())
		{
			max = contours[i].size();
			tt = i;
		}
	}
	dist.resize(max);
	for (int i = 0; i < max; i++)
	{
		dist[i] = _dist(contours[tt][(i - 1 + max) % max] - contours[tt][i], contours[tt][(i + 1) % max] - contours[tt][i]);
	}
	for (int i = 1; i < max; i++)
	{
		if (dist[i] >= 2 && len < MAX_LENGTH && contours[tt][i].x >= contours[tt][i-1].x)
		{
			if (flag)
			{
				len++;
			}
			else
			{
				if (check(contours[tt][(i - 1 + max) % max] - contours[tt][i], contours[tt][(i + 1) % max] - contours[tt][i]))
				{
					start = i;
					len = 1;
					flag = true;
				}
			}
		}
		else
		{
			if (flag)
			{
				if (len > lenm)
				{
					lenm = len;
					startm = start;
				}
				flag = false;
			}
		}
	}
	//处理始端不符合规则的线段
	flag = false;
	start = startm;
	for (int i = 0; i < BEGIN; i++)
	{
		if (check0(contours[tt][i + startm - 1] - contours[tt][i + startm], contours[tt][i + startm + 1] - contours[tt][i + startm]))
		{
			flag = true;
		}
		if (flag && check(contours[tt][i + startm - 1] - contours[tt][i + startm], contours[tt][i + startm + 1] - contours[tt][i + startm]))
		{
			start = i + startm;
			flag = false;
		}
	}
	lenm -= (start - startm);
	startm = start;
	//处理末端不符合规则的线段
	int i;
	flag = false;
	for (i = END; i < lenm; i++)
	{
		if (check2(contours[tt][i + startm - 1] - contours[tt][i + startm], contours[tt][i + startm + 1] - contours[tt][i + startm]))
		{
			break;
		}
		if (contours[tt][i + startm - 1].x == contours[tt][i + startm + 1].x)
		{
			if (flag)
			{
				break;
			}
			flag = true;
		}
		else
		{
			flag = false;
		}
	}
	lenm = i - 1;
	temp.resize(lenm);
	_trace.resize(lenm);
	for (int i = 0; i < lenm; i++)
	{
		_trace[i].y = temp[i].y = contours[tt][i + startm].y;
		_trace[i].x = temp[i].x = contours[tt][i + startm].x;
		temp[i].z = 0;
	}
	temp = align(temp);
	for (i = CUTOFF; i >=0; i--)
	{
		if (temp[i].x > temp[i + 1].x || temp[i].y >= temp[i + 1].y)
		{
			flag = true;
			break;
		}
	}
	for (; i >= 0; i--)
	{
		temp.pop_front();
	}
	for (i = temp.size() - CUTOFF - 1; i < temp.size(); i++)
	{
		if (temp[i].x < temp[i - 1].x || temp[i].y < 0)
		{
			flag = true;
			break;
		}
	}
	for (; i < temp.size();)
	{
		temp.pop_back();
	}
	temp = align(temp);
	trace.resize(temp.size());
	if (r < temp[temp.size() / 2].y || r > 300)//判断r是否有效
	{
		cout << (r = radius(temp)) << endl;
	}
	else
	{
		cout << r << endl;
	}
	for (int i = 0; i < temp.size(); i++)//曲线切片绕Y轴旋转
	{
		trace[i].y = temp[i].x - temp.back().x/2;
		trace[i].z = temp[i].y*mysin;
		trace[i].x = temp[i].y*mycos;
	}
	return _trace;
}