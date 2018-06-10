# 基于给定曲线的三维重建
    2018.06.10
        1、增加了绘制流程图（暂定）
	    2、采样过程改写，增加多项式拟合
    2018.05.25
        1、addCurve()输入类型统一为vector<cv::Point3f>。
        2、删除rebuild_3Dpoint类，统一使用glm::vec3。
        3、将原先的二维重建改为三维重建，增加了法向量的计算。
        4、为了便于导出obj文件，采用了顶点索引进行渲染。
        5、增加了对输入曲线坐标的标准化。
    2018.05.17
        完成了二维曲线重建
## 1、坐标系统 ##
OpenGL中的三维坐标系统如下：
![coordinate](https://learnopengl-cn.github.io/img/01/08/coordinate_systems_right_handed.png)
需要注意的是：
x、y、z轴的顺序和正常顺序有所不同，但都是 **右手系**；
OpenGL的有效顶点是x、y、z轴坐标都在 __[-1.0,1.0]__ 之间的数据。

##2、顶点输入 ##
OpenGL中通过 **顶点缓冲数组**（Vertex Buffer Objects, VBO）一次性将大批顶点数据发送到显卡上。这种方式要求我们在存储顶点数据时就要使用 **地址连续** 的数组（在C++中，vector也是地址连续的）。顶点缓冲数据会被解析为下面这样子：
![vertices](https://learnopengl-cn.github.io/img/01/04/vertex_attribute_pointer.png)
在rubuild.h中定义的顶点格式如下：

    struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
    };

后续还将按需增加纹理坐标、法向贴图等数据。
在绘制三角形面元时，存在很多重复顶点。为了避免造成数据冗余，我们通过索引来告知OpenGL选取哪些顶点组成三角形，这通过**顶点索引数组**（EBO）实现。另一方面，这也与obj文件的格式相符。
##3、.obj网格文件格式
为了增强通用性，最终我们希望能导出网格数据，以便用其他3D建模软件进行编辑。网格文件格式有很多种，在这里使用的是.obj格式。它的主要内容包括：

    v x y z //顶点坐标
    vt u v //纹理坐标
    vn x y z //法向量坐标
    f a1/b1/c1 a2/b2/c2 a3/b3/c3 //面顶点索引 顺序为顶点索引/纹理索引/法线索引

由于目前尚未增加纹理贴图，所以暂时还没有导出网格文件。
## 4、目前绘制的实现方式 ##  
```flow 
st=>start: 开始
e=>end: 结束 
io1=>inputoutput: 给定y方向采样数M和x方向采样数N
io2=>inputoutput: 添加曲线addCurve() 
op1=>operation: y方向等距采样
op2=>operation: 对同y分量的顶点，进行多项式拟合
op3=>operation: 对拟合曲线，在x方向等距采样
op4=>operation: 构建网格
op5=>operation: 渲染
st->io1->io2->op1->op2->op3->op4->op5->e

``` 
##待商榷： 
* **y方向等距采样，如何选定起止点？**  
  采样步长：所有曲线是否同起止点？如果不是，查找max(min(y))和min(max(y))作为起止点  
  采样方向：输入的曲线，y方向是单调变化的。能否确定y的变化方向？  
* **多项式拟合**
  https://blog.csdn.net/guduruyu/article/details/72866144
  最小二乘法多项式拟合，使用opencv求解方程组  
  方程阶数是否也要可调？  
  阶数影响极值点的数目，如果曲线形状太扭曲，可能要提高阶数，带来采样问题  
  对于一条曲线而言，x必须单一映射到z  
* **拟合曲线的采样**
  最理想的情况是按角度变化采样  
  但涉及到高阶方程的求解，计算困难  
  目前设想在x方向等距采样，对于不同纬度的切片，按球体计算横切圆的半径r，采样间隔2r/N,从+r采样到-r  
  输入曲线的点只用作拟合曲线，而不用作绘制三维模型  

##初步结果：
以球体计算，给定7条曲线。y轴采样点数M=50，x轴采样点数N=50，多项式阶数order=5  
![rebuild](https://raw.githubusercontent.com/lsa1997/images/master/%E9%87%8D%E5%BB%BA.bmp)   

效果比较不错。  
与之前的效果对比：  
![rebuild](https://raw.githubusercontent.com/lsa1997/images/master/rebuild3d.bmp)

## 5、尚未完成的工作 ##
(1)、了解院方眼球彩超拍摄原理，将彩超2维数据转为三维坐标；  
(2)、通过按键改变采样点数；  
(3)、渲染模型的纹理信息；  
