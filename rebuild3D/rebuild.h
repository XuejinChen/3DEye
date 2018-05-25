#ifndef REBUILD
#define REBUILD
#include<opencv2/opencv.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <vector>
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

class rebuild_curve {
public:
	rebuild_curve();
	void setLength(int length);
	void addCurve(vector<cv::Point3f> curve, int length);
	glm::vec3* curve;
	int length;
};

rebuild_curve::rebuild_curve() {
	this->length = 0;
	this->curve = NULL;
}

void rebuild_curve::setLength(int length) {
	this->length = length;
	this->curve = new glm::vec3[length];
}

void rebuild_curve::addCurve(vector<cv::Point3f> curve, int length) {
	if (this->length == 0) {
		this->length = length;
		this->curve = new glm::vec3[length];
	}
	for (int i = 0; i<length; i++) {
		this->curve[i].x = curve[i].x;
		this->curve[i].y = curve[i].y;
		this->curve[i].z = curve[i].z;
	}
};

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	//// texCoords
	//glm::vec2 TexCoords;
	//// tangent
	//glm::vec3 Tangent;
	//// bitangent
	//glm::vec3 Bitangent;
};

class rebuild {
public:
	rebuild(int curveNum, int sampleNum);
	void rebuild_sample(); //等距采样
	void rebuild_pointPrint(); //打印采样点
	void rebuild_addCurve(vector<cv::Point3f> curve,int length); //添加曲线
	int rebuild_draw(); //绘制图形

	vector<Vertex> vertices; //顶点
	vector<unsigned int> indices; //索引
	//vector<glm::vec3> normal; //法向量
	//占位：纹理uv坐标
private:
	rebuild_curve* curve;
	rebuild_curve* curve_sampled;
	int curveNum; //曲线数目
	int sampleNum; //采样点数
	int currentIndex; //当前已添加曲线数

	float rebuild_calcDist(glm::vec3 a, glm::vec3 b); //计算两点间距离
	void rebuild_calcVertices(); //将曲线转化为顶点数组
	glm::vec3 getSample(glm::vec3 startPoint, glm::vec3 endPoint, float distance); //获取采样点
	float max(float x, float y);
	float min(float x, float y);
	void rebuild_normalize();
};

rebuild::rebuild(int curveNum, int sampleNum) {
	this->curveNum = curveNum;
	this->sampleNum = sampleNum;
	this->currentIndex = 0;
	this->curve = new rebuild_curve[curveNum];
	this->curve_sampled = new rebuild_curve[curveNum];
	cout << "curveNum:" << curveNum << "\tsampleNum:" << this->sampleNum << "\n" << endl;

};
//添加曲线
void rebuild::rebuild_addCurve(vector<cv::Point3f> curve, int length) {
	if (currentIndex >= curveNum) {
		printf("cannot add more curves!\n");
		return;
	}
	else {
		this->curve[currentIndex].addCurve(curve, length);
		currentIndex++;
	}
}
//等距采样
void rebuild::rebuild_sample() {
	rebuild_normalize();

	for (int p = 0; p < currentIndex; p++) {

		int length = this->curve[p].length;
		int sampleNum = this->sampleNum;
		float totalDist = 0;
		float* dist = new float[length]; //记录每个点的长度
		this->curve_sampled[p].setLength(sampleNum);
		dist[0] = 0;

		//采样的首尾点为给定曲线的首尾点
		this->curve_sampled[p].curve[0] = this->curve[p].curve[0];
		this->curve_sampled[p].curve[sampleNum - 1] = this->curve[p].curve[length - 1];
		//计算点到第一个点的距离
		for (int i = 1; i < length; i++) {
			float dist2points = rebuild_calcDist(this->curve[p].curve[i], this->curve[p].curve[i - 1]);
			totalDist += dist2points;
			dist[i] = totalDist;
			cout << "dist" << i << ":" << dist[i] << "\n" << endl;
		}

		float avgDist = totalDist / (sampleNum - 1); //采样间距
		cout << "totalDist:" << totalDist << "\tavgDist:" << avgDist << "\n" << endl;
		float currentDist = avgDist;
		int currentIndex = 0;
		for (int i = 1; i < sampleNum - 1; i++) {
			while (dist[currentIndex] < currentDist) {
				currentIndex++;
			}
			float distance = currentDist - dist[currentIndex - 1];
			cout << "currentIndex:" << currentIndex << "\n" << endl;
			this->curve_sampled[p].curve[i] = getSample(this->curve[p].curve[currentIndex - 1], this->curve[p].curve[currentIndex], distance);
			currentDist += avgDist;
		}


	}

}

//计算两点间距离
float rebuild::rebuild_calcDist(glm::vec3 p1, glm::vec3 p2) {
	float distance = 0;
	distance = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
	return distance;
}

//计算采样点
glm::vec3 rebuild::getSample(glm::vec3 startPoint, glm::vec3 endPoint, float distance) {
	glm::vec3 samplePoint;
	float d = rebuild_calcDist(startPoint, endPoint);
	float sin_theta = (endPoint.y - startPoint.y) / d;
	float cos_theta = sqrt(1 - pow(sin_theta, 2));

	float cos_phi = (endPoint.x - startPoint.x) / (d*cos_theta);
	float sin_phi = (endPoint.z - startPoint.z) / (d*cos_theta);

	samplePoint.x = startPoint.x + distance*cos_theta*sin_phi;
	samplePoint.y = startPoint.y + distance*sin_theta;
	samplePoint.z = startPoint.z + distance*cos_theta*cos_phi;

	return samplePoint;
}

void rebuild::rebuild_pointPrint() {
	for (int j = 0; j < currentIndex; j++) {
		for (int i = 0; i < sampleNum; i++) {
			cout << "point" << i << "\tx:" << this->curve_sampled[j].curve[i].x << "\ty:" << this->curve_sampled[j].curve[i].y << "\tz:" << this->curve_sampled[j].curve[i].z << "\n" << endl;
		}
	}
}

float rebuild::max(float x, float y) {
	if (x >= y)
		return x;
	else
		return y;
}

float rebuild::min(float x, float y) {
	if (x <= y)
		return x;
	else
		return y;
}

int rebuild::rebuild_draw() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("3.1.materials.vs", "3.1.materials.fs");

	rebuild_calcVertices();

	//for (int i = 0; i < currentIndex*sampleNum; i++) {
	//	cout << "x:" << vertices[i * 6] << "\ty:" << vertices[i * 6 + 1] << "\tz:" << vertices[i * 6 + 2] << "\n" << endl;
	//}


	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, VAO,EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	// normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	
	glBindVertexArray(0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		glm::vec3 lightColor;
		lightColor.x = 1.0f;
		lightColor.y = 0.7f;
		lightColor.z = 0.3f;
		glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		lightingShader.setVec3("light.ambient", ambientColor);
		lightingShader.setVec3("light.diffuse", diffuseColor);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		lightingShader.setFloat("material.shininess", 32.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model;
		lightingShader.setMat4("model", model);

		// render the cube
		glBindVertexArray(VAO);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
//将曲线坐标转化为顶点数据
void rebuild::rebuild_calcVertices() {
	int index = 0;

	for (int i = 0; i < currentIndex; i++) {
		for (int j = 0; j < sampleNum; j++) {
			Vertex vert;
			vert.Position = this->curve_sampled[i].curve[j];
			vert.Normal = glm::vec3(0.0, 0.0, 0.0);
			this->vertices.push_back(vert);
		}
	}

	for (int i = 0; i < currentIndex - 1; i++) {
		for (int j = 0; j < sampleNum - 1; j++) {
			//设置顶点索引
			this->indices.push_back(i*sampleNum + j);
			this->indices.push_back((i + 1)*sampleNum + j);
			this->indices.push_back(i*sampleNum + j + 1);
			this->indices.push_back(i*sampleNum + j + 1);
			this->indices.push_back((i + 1)*sampleNum + j);
			this->indices.push_back((i + 1)*sampleNum + j + 1);

			//计算面的法向量
			glm::vec3 a, b;
			a = this->curve_sampled[i].curve[j] - this->curve_sampled[i].curve[j + 1];
			b = this->curve_sampled[i + 1].curve[j] - this->curve_sampled[i].curve[j + 1];
			glm::vec3 norm = glm::cross(a,b);
			this->vertices[i * sampleNum + j].Normal += norm;
			this->vertices[(i + 1) * sampleNum + j].Normal += norm;
			this->vertices[i * sampleNum + j + 1].Normal += norm;

			a = b;
			b = curve_sampled[i + 1].curve[j + 1] - curve_sampled[i].curve[j + 1];
			norm = glm::cross(a, b);
			this->vertices[i * sampleNum + j + 1].Normal += norm;
			this->vertices[(i + 1) * sampleNum + j].Normal += norm;
			this->vertices[(i + 1) * sampleNum + j + 1].Normal += norm;
		}
	}
	int count = 0;
	for (int i = 0; i < currentIndex; i++) {
		for (int j = 0; j < sampleNum; j++) {
			cout << "position[" << count << "]=" << vertices[count].Position.x << "," << vertices[count].Position.y << "," << vertices[count].Position.z << "\n";
			vertices[count].Normal = glm::normalize(vertices[count].Normal);
			cout << "normal[" << count << "]=" << vertices[count].Normal.x << "," << vertices[count].Normal.y << "," << vertices[count].Normal.z << "\n";
			count++;
		}
	}
		for (int j = 0; j < indices.size(); j++) {
			cout << "indice[" << j << "]=" << indices[j] << "\n";
	}
}

//将坐标归一化到-1.0~1.0之间
void rebuild::rebuild_normalize() {
	float maxCord , minCord;
	float normCord;
	maxCord = curve[0].curve[0].x;
	minCord = maxCord;
	for (int i = 0; i < currentIndex; i++) {
		for (int j = 0; j < curve[i].length; j++) {
			if (maxCord < curve[i].curve[j].x) maxCord = this->curve[i].curve[j].x;
			if (maxCord < curve[i].curve[j].y) maxCord = this->curve[i].curve[j].y;
			if (maxCord < curve[i].curve[j].z) maxCord = this->curve[i].curve[j].z;

			if (minCord > curve[i].curve[j].x) minCord = this->curve[i].curve[j].x;
			if (minCord > curve[i].curve[j].y) minCord = this->curve[i].curve[j].y;
			if (minCord > curve[i].curve[j].z) minCord = this->curve[i].curve[j].z;
		}
	}

	normCord = max(abs(maxCord), abs(minCord));
	cout << "normCord:" << normCord << "\n";
	for (int i = 0; i < currentIndex; i++) {
		for (int j = 0; j < curve[i].length; j++) {
			this->curve[i].curve[j].x /= normCord;
			this->curve[i].curve[j].y /= normCord;
			this->curve[i].curve[j].z /= normCord;
		}
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#endif // !REBUILD

