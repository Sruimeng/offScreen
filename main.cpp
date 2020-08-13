// MIT License

// Copyright (c) 2020 苏瑞梦

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "Shader.h"
#include "toojpeg.h"

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

//如果使用osmesa的话 还需要调用glfw3native
#if USE_NATIVE_OSMESA
#define GLFW_EXPOSE_NATIVE_OSMESA
#include "../include/GLFW/glfw3native.h"
#endif

#include <iostream>
#include <fstream>
#if defined(_WIN32)
#pragma comment(lib, "../lib64/glfw3.lib")
#endif

#define GET_ARRAY_SIZE(array, length)                \
	{                                                \
		length = (sizeof(array) / sizeof(array[0])); \
	}

namespace
{
	//输出文件的名称
	std::string OUT_FILE = "example.jpg";
	//输出图片的大小
	int IMG_SIZE = 2048;
	auto imgBuffer = (unsigned char *)malloc(IMG_SIZE * IMG_SIZE * 3); //颜色输出流
	//输出文件
	std::ofstream outFile;
	void myOutput(unsigned char byte)
	{
		outFile << byte;
	}
	//opengl初始化方法
	void openglInit(int major, int minor)
	{
		//glfw初始化
		glfwInit();
		//设置主版本和小版本
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		//窗口的初始化设置
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	}
	//报错输出
	static void error_callback(int error, const char *description)
	{
		fprintf(stderr, "Error: %s\n", description);
	}
} // namespace

int main()
{
	outFile.open(OUT_FILE);
	//新建窗口
	GLFWwindow *window;
	//错误输出
	glfwSetErrorCallback(error_callback);
	//macOS的特殊初始化
	glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	//opengl 初始化
	openglInit(3, 3);
	//不显示窗口
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	//新建窗口 需要注意的是这里窗口的大小代表了输出图片的大小
	window = glfwCreateWindow(IMG_SIZE, IMG_SIZE, "offscreen example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	//创建上下文
	glfwMakeContextCurrent(window);
	//glad初始化
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int indices[] = {// 通过index创建三角形
							  0, 1, 2};
	float vertices[] = {
		//创建点的坐标
		0.0f,
		0.7f,
		0.0f,
		-0.7f,
		-0.7f,
		0.0f,
		0.7f,
		-0.7f,
		0.0f,
	};
	float colors[] = {
		//修改颜色
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,
	};

	int verticesLength;
	int indicesLength;
	GET_ARRAY_SIZE(vertices, verticesLength);
	GET_ARRAY_SIZE(indices, indicesLength);

	//设置vbo
	unsigned int VBO[2];
	glGenBuffers(2, VBO);
	//设置vao
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	//绑定VAO
	glBindVertexArray(VAO);
	//绑定VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);

	{
		//设置EBO
		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}
	//设置fbo的窗口
	float quadVertices[] = {// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
							// positions   // texCoords
							-1.0f, 1.0f, 0.0f, 1.0f,
							-1.0f, -1.0f, 0.0f, 0.0f,
							1.0f, -1.0f, 1.0f, 0.0f,

							-1.0f, 1.0f, 0.0f, 1.0f,
							1.0f, -1.0f, 1.0f, 0.0f,
							1.0f, 1.0f, 1.0f, 1.0f};
	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

	//绑定基础的shader
	Shader fsShader("../../offscreen/shader.fs");
	Shader vsShader("../../offscreen/shader.vs");
	ShaderProgram shaderProgram(vsShader.code, fsShader.code);
	shaderProgram.use();
	//绑定屏幕的shader
	Shader ffsShader("../../offscreen/screen.fs");
	Shader fvsShader("../../offscreen/screen.vs");
	ShaderProgram fshaderProgram(fvsShader.code, ffsShader.code);
	
	fshaderProgram.use();
	fshaderProgram.setInt("screenTexture", 0);
	// FBO的设置
	// -------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// 颜色纹理的设置
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMG_SIZE, IMG_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// 设置rbo
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, IMG_SIZE, IMG_SIZE);			  // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
																								  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//设置图片相关参数
	const bool isrgb = true;			   // true = rgb image, else false = grayscale
	const int quality = 100;			   // compression quality: 0 = worst, 100 = best, 80 to 90 are most often used
	const bool downsample = false;		   // false = save as ycbcr444 jpeg (better quality), true = ycbcr420 (smaller file)
	const char *comment = "example image"; // arbitrary jpeg comment

	// {
	// 	glfwGetFramebufferSize(window, &IMG_SIZE, &IMG_SIZE);

	// 	glViewport(0, 0, IMG_SIZE, IMG_SIZE);

	// 	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// 	glClear(GL_COLOR_BUFFER_BIT);

	// 	shaderProgram.use();

	// 	glDrawArrays(GL_TRIANGLES, 0, 3);

	// #if USE_NATIVE_OSMESA
	// 	glfwGetOSMesaColorBuffer(window, &IMG_SIZE, &IMG_SIZE, NULL, (void**) &imgBuffer);
	// #else
	// 	glReadPixels(0, 0, IMG_SIZE, IMG_SIZE, GL_RGB, GL_UNSIGNED_BYTE, imgBuffer);
	// #endif

	// 	auto ok = TooJpeg::writeJpeg(myOutput, imgBuffer, IMG_SIZE, IMG_SIZE, isrgb, quality, downsample, comment);

	// #if USE_NATIVE_OSMESA
	// // Here is where there's nothing
	// #else
	// 	free(imgBuffer);
	// #endif

	// 	glfwDestroyWindow(window);

	// 	glfwTerminate();
	// 	exit(EXIT_SUCCESS);
	// }

	{
		// render
		// ------
		// bind to framebuffer and draw scene as we normally would to color texture
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

		// make sure we clear the framebuffer's content
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due
		glViewport(0, 0, IMG_SIZE, IMG_SIZE);
		fshaderProgram.use();
		// clear all relevant buffers
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer); // use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);
#if USE_NATIVE_OSMESA
		glfwGetOSMesaColorBuffer(window, &IMG_SIZE, &IMG_SIZE, NULL, (void **)&imgBuffer);
#else
		glReadPixels(0, 0, IMG_SIZE, IMG_SIZE, GL_RGB, GL_UNSIGNED_BYTE, imgBuffer);
#endif
		auto ok = TooJpeg::writeJpeg(myOutput, imgBuffer, IMG_SIZE, IMG_SIZE, isrgb, quality, downsample, comment);
		glBindVertexArray(0); // no need to unbind it every time static_cast<GLsizei>(vertices.size())
		//glfwGetFramebufferSize(window, &IMG_SIZE, &IMG_SIZE);
		// #if USE_NATIVE_OSMESA
		// 		glfwGetOSMesaColorBuffer(window, &IMG_SIZE, &IMG_SIZE, NULL, (void **)&imgBuffer);
		// #else
		// 		glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, imgBuffer);
		// #endif

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

#if USE_NATIVE_OSMESA
// Here is where there's nothing
#else
		free(imgBuffer);
#endif

		glfwDestroyWindow(window);

		glfwTerminate();
		exit(EXIT_SUCCESS);
	}
}