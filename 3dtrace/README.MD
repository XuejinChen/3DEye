# 眼球B超图片的边缘检测

---

### 一、 观察B超图片
把图片的边缘细节放大观察，发现边缘处的过度带比较宽，有些地方甚至有二十几个像素的过渡带，而且边缘凹凸不平，这并不方便提取平滑的边缘。
![ps.jpg](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/ps.png)

## 二、 处理步骤
首先对图像进行平滑处理，使用11×11像素的均值滤波，使得眼球边缘的过渡带趋于均匀，如下图所示

![blur.jpg](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/blur.jpg)

可以看到边缘过渡带的比较平滑，这时候选定一个阈值进行边缘查找即可。关于边缘查找的算法，我使用的是首先对二值化图像进行连通域标记[[1]](README.MD#references)，然后在已有的连通域的基础上进行轮廓的查找[[2]](README.MD#references)。
设置阈值为70二值化（只对眼球B超图像的区域进行处理，相当于裁剪了图像），得到的轮廓如下图中的绿线所示：

![edges.jpg](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/edges.jpg)

可以看到眼球的轮廓已经被查找出来了，但是要将其单独提取出来还需要进一步的处理。首先图中包含数个轮廓，显而易见眼球的轮廓是属于最长的那条轮廓的，那么找出这条最长的轮廓，眼球段的部分相对部其他部分平滑，可以利用这一特征将其提取出来。
定义连续的轮廓上某一个像素点的"平滑度" :
>轮廓上的各个像素点是连续的，那么其中任何一个像素点有且仅有两个相邻的像素点，那么这两个相邻像素点最少需要移动多少单位长度（只能水平或垂直）才能重合，定义为这个轮廓在这个点的"平滑度"。比如下图两种情况下，"平滑度"分别为2和3。

![smooth2.png](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/smooth2.png)   ![smooth3.png](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/smooth3.png)

接下来就是把平滑度大于等于3的最长连续子轮廓找出来，对起点和终点再进行特殊处理即可找到比较满意的眼球轮廓。实验结果如下图所示，黄线部分即是在最大轮廓的子段

![out.png](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/out.jpg)

把原来边缘查找的绿线隐藏

![trace.png](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/trace.jpg)

从结果上看还是比较成功的。代码的源文件3dtrace.h与3dtrace.cpp已上传。

## 三、 未解决的问题
本项目的目标是找出眼球的三维坐标，但是目前只实现了二维坐标的查找，B超图片中的信息无法准确判断其之间的三维关系。对于数据集，同一个眼球的B超图的数量从4张到8张不等，要从这些图像中构建出三维关系仍缺少数据，我在源代码中默认三维坐标Z轴的值为0。
### 1.  GN值
每张B超图片上方都会出现一个GB值，单位为分贝，不同的图片的值不一样。而且B超图片上给出的数据只有这一个GN值，经过网络资料搜索推测为超声波辐射的功率，应该跟三维位置无关系。但是不是很确定，待确认。
### 2. 其他数据
某些文件夹中包含一张或多张如下图所示的数据图片，其中数据的意义不明，如果其中包含了三维位置的信息，那么对于不包含这种数据图片的某些文件夹又应该如何处理？

![data.jpg](https://raw.githubusercontent.com/taotie144/ImgStore/master/temp1/data.jpg)

## **REFERENCES:**

[1]: Lifeng He, Yuyan Chao, Kenji Suzuki, Kesheng Wu, "Fast connected-component labeling", *Pattern Recognition*, vol. 42, pp. 1977-1987, 2009.

[2]: WANG Fu-sheng, QI Guo-qing, "Boundary tracking algorithm of objects in binary image", *Journal of Dalian Maritime University*, vol. 32, no. 1, pp. 62-67,  Feb. 2006.
