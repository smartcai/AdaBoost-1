//
//  Sample_Initialization.h
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/3/9.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#ifndef S_I
#define S_I

#include<iostream>
#include<vector>
#include<mysql.h>
#include<cv.h>
#include<fstream>
#include<string>
#include<highgui.h>
#include"Trainning_Sample.h"

using namespace std;
using namespace cv;

class Sample_Initialization
{
public:
    vector<string> P_File;//正样本图片名称；
    vector<string> M_File;//负样本图片名称；
    vector<Trainning_Sample> P_Sample;//正类训练样本信息；
    int P_T;//使用正类样本的数量；
    vector<Trainning_Sample> M_Sample;//负类训练样本信息；
    int M_T;//使用负类样本的数量；

    //获取所有样本名称与样本信息；
    void GetFile();//获取样本名称；
    Trainning_Sample Calculate(Trainning_Sample t_s);//获取样本所有信息；
    void Sample_Load();//加载所有样本；
};
#endif
