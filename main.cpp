//
//  main.cpp
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/2/25.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#include<iostream>
#include<cv.h>
#include<highgui.h>
#include<mysql.h>
#include<stdlib.h>
#include"Haar_Feature_Trainning.h"
#include"AdaBoost_Trainning.h"
#include"Sample_Initialization.h"

using namespace std;
using namespace cv;

int main()
{
    //加载正类样本和负类样本的信息；
    Sample_Initialization si;
    si.GetFile();
    si.P_T=1000;
    si.M_T=1000;
    si.Sample_Load();
    
    AdaBoost_Trainning a_t;
    a_t.Init(200);
    a_t.h_f_t.P_Sample=si.P_Sample;
    a_t.h_f_t.M_Sample=si.M_Sample;
    a_t.AdaBoostTrainning();
    
    //创建连接；
    MYSQL mycon;
    mysql_init(&mycon);
    mysql_real_connect(&mycon, "localhost", "root", "", "adaboost", 3306, NULL, 0);
    string sql;//SQL语句；

    int i;
    for(i=0;i<a_t.w_c.size();i++)
    {
        sql="insert into weak_classifier1 (";
        sql+=a_t.h_f_t.Convert(i+1)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].haar.kind)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].threshold)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].p)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].rate)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].weight)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].point1.x_axis)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].point1.y_axis)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].point2.x_axis)+",";
        sql+=a_t.h_f_t.Convert(a_t.w_c[i].point2.y_axis)+")";
        mysql_real_query(&mycon, sql.c_str(), sql.length());
    }
    
    return 0;
}
