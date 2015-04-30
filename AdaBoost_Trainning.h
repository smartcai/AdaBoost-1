//
//  AdaBoost_Trainning.h
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/3/2.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#ifndef A_T
#define A_T

#include<stdio.h>
#include<iostream>
#include<cv.h>
#include<highgui.h>
#include<mysql.h>
#include<cmath>
#include<vector>
#include"Haar_Feature_Trainning.h"
#include"Trainning_Sample.h"
#include"Weak_Classifier.h"

using namespace std;
using namespace cv;

class AdaBoost_Trainning
{
public:
    int T;//训练次数；
    vector<Weak_Classifier> w_c;//弱分类器的集合；
    Haar_Feature_Trainning h_f_t;//创建训练对象；
    
    void Init(int t);
    void Weight_Normalization();//权重归一化；
    void Weight_Renew(Weak_Classifier w,double b);//权重更新；
    void AdaBoostTrainning();//训练函数，训练强分类器；
};
#endif
