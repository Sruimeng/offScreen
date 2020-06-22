# linux服务器运行opengl图形程序

## 运行方法

### 安装cmake

`sudo apt install cmake`

### 安装Xvfb

`sudo apt-get install xvfb`

### 编译

```cpp

cmake ./

make

Xvfb :5 -screen 0 800x600x24 &
export DISPLAY=:5

./offscreen

```

### 输出

输出结果为一个名为example.jpg的图片
