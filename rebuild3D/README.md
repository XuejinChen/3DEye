# 基于给定曲线的三维重建
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
绘制流程如下：
```flow
st=>start: 开始
e=>end: 结束
op1=>operation: 输入曲线
op2=>operation: 等距采样
op3=>operation: 生成索引及顶点数据
op4=>operation: 渲染

st->op1->op2->op3->op4->e

```
目前绘制的思路主要参考绘制球体的流程（曲线数=50，每条曲线采样100点）：
![sphere](https://raw.githubusercontent.com/lsa1997/images/master/sphere.bmp)
![sphere_line](https://raw.githubusercontent.com/lsa1997/images/master/sphere_line.bmp)
在眼球彩超重建中，初步认为不同彩超图片类似于球体在纬度方向切片,绘制时类似于在经度方向绘制三角形带，三角形剖分时选择最临近的三个顶点组成三角形。  
现在的问题在于，对于同一个眼球，彩超图片数目最多二十余张，最少仅四张，如果采用三角形网格，渲染出的模型如下所示（曲线数=7，每条曲线采样50点）：
![rebuild](https://raw.githubusercontent.com/lsa1997/images/master/rebuild3d.bmp)
可以看到，由于曲线数据较少，球体的边缘有明显的棱角，但由于顶点法向量是取均值得到的，所以面片之间的过渡还比较自然。如果要得到更为圆润的模型，还需要进一步的优化。
## 5、尚未完成的工作 ##
(1)、了解院方眼球彩超拍摄原理，将彩超2维数据转为三维坐标；  
(2)、通过按键改变采样点数；  
(3)、渲染模型的纹理信息；  
(4)、对于彩超图像数目过少时的处理。  
