//
//  Cascade_Classifier_Trainning.h
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/3/2.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#ifndef C_C_T
#define C_C_T

#include<stdio.h>
#include<iostream>
#include<cv.h>
#include<highgui.h>
#include<vector>
#include<string>
#include<mysql.h>
#include"Trainning_Sample.h"
#include"AdaBoost_Trainning.h"
#include"Sample_Initialization.h"

using namespace std;
using namespace cv;

class Cascade_Classifier_Trainning
{
public:    
    AdaBoost_Trainning a_t;//定义AdaBoost对象；
    Sample_Initialization s_i;//定义Sample_Initialization对象；
    
    //表示训练的当层强分类器的误识率与监测率；
    double f_t;
    double d_t;
    double F;
    double D;
    
    //级联分类器的实现；
    void Cascade_Trainning();//实现级联分类器的训练；
    double Strong_judge1(vector<Weak_Classifier> w_c);//强预测；
    double Strong_judge2(vector<Weak_Classifier> w_c);//强预测；
    int Strong_judge2(vector<Weak_Classifier> w_c,int num);//强预测，判断第num个样本的样本类型；
    int get_haar1(Haar_Feature haar, Coordinate A_Point, Coordinate D_Point, int num);
    int get_haar2(Haar_Feature haar, Coordinate A_Point, Coordinate D_Point, int num);
};
#endif
