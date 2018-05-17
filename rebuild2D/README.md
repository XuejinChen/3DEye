# 基于给定曲线的三维重建（适用于二维曲线） #
  
## 1、OpenGL中的坐标系统 ##
  
OpenGL中的三维坐标系统如下：

![coordinate](https://learnopengl-cn.github.io/img/01/08/coordinate_systems_right_handed.png )
需要注意的是：
x、y、z轴的顺序和正常顺序有所不同，但都是 **右手系**；
OpenGL的有效顶点是x、y、z轴坐标都在 __[-1.0,1.0]__ 之间的数据。
  
## 2、OpenGL中的顶点输入 ##
  
OpenGL中通过 **顶点缓冲数组**（Vertex Buffer Objects, VBO）一次性将大批顶点数据发送到显卡上。这种方式要求我们在存储顶点数据时就要使用 **地址连续** 的数组。顶点缓冲数据会被解析为下面这样子：

![vertices](https://learnopengl-cn.github.io/img/01/04/vertex_attribute_pointer.png )
  
## 3、目前绘制的实现方式 ##
  
绘制流程如下：
```flow
st=>start: 开始
e=>end: 结束
op1=>operation: 输入曲线
op2=>operation: 等距采样
op3=>operation: 设置顶点数组
op4=>operation: 渲染
  
st->op1->op2->op3->op4->e
  
```
目前绘制的思路主要参考绘制球体的流程：

![](https://raw.githubusercontent.com/lsa1997/rebuild2D/master/sphere.bmp)  
![](https://raw.githubusercontent.com/lsa1997/rebuild2D/master/sphere_line.bmp)  
在眼球彩超重建中，初步认为不同彩超图片类似于球体在经度方向切片,绘制时类似于在纬度方向绘制三角形带。
三角形剖分时选择最临近的三个顶点组成三角形：

![](https://raw.githubusercontent.com/lsa1997/rebuild2D/master/triangulation.png)  
  
## 4、尚未完成的工作 ##
  
(1)、对给定的曲线数据进行标准化，使之符合OpenGL的绘制要求；
(2)、使用模型导入库Assimp，将模型的数据改为通用的网格数据；
(3)、三维曲线数据的计算、顶点法向量的计算和三维图形的绘制；
(4)、眼球彩超图片边缘提取的数据与需要的曲线数据之间的坐标转换。
  
