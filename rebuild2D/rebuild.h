#ifndef REBUILD
#define REBUILD

using namespace std;

class rebuild_3Dpoint {
public:
	float x,y,z;
};

class rebuild_2Dcurve {
public:
	rebuild_2Dcurve();
	void setLength(int length);
	void addCurve(float* curve, int length);
	rebuild_3Dpoint* curve;
	int length;
};

class rebuild2D {
public:
	rebuild2D(int curveNum, int sampleNum);
	void rebuild_sample(); //等距采样
	void rebuild_pointPrint(); //打印采样点
	void rebuild_addCurve(float* curve,int length); //添加曲线
	int rebuild_draw(); //绘制图形

private:
	rebuild_2Dcurve* curve;
	rebuild_2Dcurve* curve_sampled;
	int curveNum; //曲线数目
	int sampleNum; //采样点数
	int currentIndex; //当前已添加曲线数

	float rebuild_calcDist(rebuild_3Dpoint a, rebuild_3Dpoint b); //计算两点间距离
	void rebuild_calcVertices(float * vertices); //将曲线转化为顶点数组
	rebuild_3Dpoint getSample(rebuild_3Dpoint startPoint, rebuild_3Dpoint endPoint, float distance); //获取采样点
	float max(float x, float y);
	float min(float x, float y);
};



#endif // !REBUILD

