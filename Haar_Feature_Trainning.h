//
//  Haar_Feature_Trainning.h
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/2/27.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#ifndef H_F_T
#define H_F_T

#include<stdlib.h>
#include<stdio.h>
#include<cv.h>
#include<highgui.h>
#include<mysql.h>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include"Coordinate.h"
#include"Trainning_Sample.h"
#include"Haar_Feature.h"
#include"Weak_Classifier.h"

using namespace std;
using namespace cv;

class Haar_Feature_Trainning
{
public:
    vector<Trainning_Sample> P_Sample;//人脸样本集合；
    vector<Trainning_Sample> M_Sample;//非人脸样本集合；
    Weak_Classifier w;//求出的最优弱分类器；
    
    //获取当项Haar特征值；
    int get_haar1(Haar_Feature haar,Coordinate A_Point,Coordinate D_Point,int num);//求正类；
    int get_haar2(Haar_Feature haar,Coordinate A_Point,Coordinate D_Point,int num);//求负类；
    //弱预测；
    int Weak_judge(Weak_Classifier w_c,int p,int num,int type);//用弱分类器判断当前样本的正负类；
    void Get_All(Haar_Feature haar,string tabel_name);//初次获取所有特征值函数；
    void Trainning(Haar_Feature haar,string table_name,int t);//训练特征；
    //辅助函数；
    string Convert(int temp);//将整型转换为字符串类型；
    string Convert(double temp);//将浮点型转换为字符串类型；
    int Low_Integral_Function(int x1,int x2);//取下整函数；
};
#endif
