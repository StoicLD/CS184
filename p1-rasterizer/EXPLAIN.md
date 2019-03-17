# Project 1: Rasterizer

<img src="https://cs184.eecs.berkeley.edu/cs184_sp16_content/article_images/3_1.jpg" width="800px" align="middle"/>

## 前言
第一个project的内容覆盖CS184从spring2019第一课到第五课的内容，包含了三角形光栅化
绘制，基本的矩阵变换和采样。总共是两个Section，七个part，由于第七个part属于个人自由
发挥选做内容。。本人由于懒没有做，因此只有前六个part。Project1采用了CS184教学团队
自己开发的GUI和图形库CGL，貌似是在OpenGL上封装了一层。详细的初始化说明请参考[README.md](./README.md)

## Section I: Rasterization

### Part 1: Rasterizing single-color triangles
第一部分是光栅化三角形。这貌似是每个图形学入门课程开始必做的一个练习。
在开始之前，建议先看一下这篇文章，讲述了[图片如何以数据形式存在于计算机中](https://cs184.eecs.berkeley.edu/sp19/article/7/images-as-data)
本题主要是做两件事：
（1）