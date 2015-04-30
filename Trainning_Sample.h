//
//  Trainning_Sample.h
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/2/27.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#ifndef T_S
#define T_S

#include<iostream>
#include<cv.h>
#include<highgui.h>

using namespace std;
using namespace cv;

class Trainning_Sample
{
public:
    Mat image;//样本图像；
    int integral_image[20][20];//积分图像；
    int type;//样本类型；
    double weight;//样本权重；
};
#endif