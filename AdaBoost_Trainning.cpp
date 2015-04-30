//
//  AdaBoost_Trainning.cpp
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/3/2.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//
#include "AdaBoost_Trainning.h"

void AdaBoost_Trainning::Init(int t)
{
    T=t;
}
//训练函数；
void AdaBoost_Trainning::AdaBoostTrainning()
{
    //构造Haar特征；
    Haar_Feature haar1;
    haar1.kind=1;
    haar1.s=2;
    haar1.t=1;
    
    /*
    Haar_Feature haar2;
    haar2.kind=2;
    haar2.s=1;
    haar2.t=2;
    */
    /*
    Haar_Feature haar3;
    haar3.kind=3;
    haar3.s=3;
    haar3.t=1;
    
    Haar_Feature haar4;
    haar4.kind=4;
    haar4.s=1;
    haar4.t=3;
    
    Haar_Feature haar5;
    haar5.kind=5;
    haar5.s=2;
    haar5.t=2;
    */
    
    Haar_Feature haar6;
    haar6.kind=6;
    haar6.s=1;
    haar6.t=3;
    
    /*
    Haar_Feature haar7;
    haar7.kind=7;
    haar7.s=2;
    haar7.t=1;
    */
    //所有的特征值已经计算完成；
    /*
    //创建连接；
    MYSQL mycon;
    mysql_init(&mycon);
    mysql_real_connect(&mycon, "localhost", "root", "", "adaboost", 3306, NULL, 0);
    string sql;//SQL语句；
    
    sql="delete from haar_feature1";
    mysql_real_query(&mycon, sql.c_str(), sql.length());
    
    sql="delete from haar_feature2";
    mysql_real_query(&mycon, sql.c_str(), sql.length());
    
    sql="delete from haar_feature6";
    mysql_real_query(&mycon, sql.c_str(), sql.length());
    
    sql="delete from haar_feature7";
    mysql_real_query(&mycon, sql.c_str(), sql.length());
    mysql_close(&mycon);
    */
    w_c.clear();//清空弱分类器集合；
    
    int i;//临时变量；
    //训练；
    for(i=0;i<T;i++)
    {
        cout<<"第"<<i<<"次训练:"<<endl;
        //1.归一化样本权重；
        Weight_Normalization();
        
        vector<Weak_Classifier> w;//创建存放弱分类器的容器；
        h_f_t.Trainning(haar1, "haar_feature1", i);//使用第1类haar特征；
        cout<<"第一个特征对应的弱分类器的错误率："<<h_f_t.w.rate<<endl;
        w.push_back(h_f_t.w);//读取第一个特征生成的最佳弱分类器；
       
        //h_f_t.Trainning(haar2, "haar_feature2", i);//使用第2类haar特征；
        //cout<<"第二个特征对应的弱分类器的错误率："<<h_f_t.w.rate<<endl;
        //w.push_back(h_f_t.w);//读取第二个特征生成的最佳弱分类器；
        
        h_f_t.Trainning(haar6, "haar_feature6", i);//使用第6类haar特征；
        cout<<"第三个特征对应的弱分类器的错误率："<<h_f_t.w.rate<<endl;
        w.push_back(h_f_t.w);//读取第三个特征生成的最佳弱分类器；
        
        //h_f_t.Trainning(haar7, "haar_feature7", i);//使用第7类haar特征；
        //cout<<"第四个特征对应的弱分类器的错误率："<<h_f_t.w.rate<<endl;
        //w.push_back(h_f_t.w);//读取第四个特征生成的最佳弱分类器;
        
        //改！！！！！！！！！！！！！！！！！！！！！！！！！！
        //对比获取四个特征的最优弱分类器中的最佳弱分类器；
        /*
        double min_rate=w[0].rate;
        int m;//临时变量；
        int temp=0;//临时变量；
        for(m=1;m<2;m++)
        {
            if(w[m].rate<min_rate)
            {
                temp=m;
                min_rate=w[m].rate;
            }
        }
         */
        
        int temp=0;//临时变量；
        if(w[0].rate<w[1].rate)
        {
            temp=0;
        }
        else
        {
            temp=1;
        }
        //计算所有弱分类器的权重；
        double b=(w[temp].rate*1.0000)/(1.0000-w[temp].rate);
        double alph=log(1.0000/b);
        w[temp].weight=alph;
        
        w_c.push_back(w[temp]);
        Weight_Renew(w_c[i],b);//样本权重更新；
        
        cout<<"阈值为:"<<w_c[i].threshold<<endl;
        cout<<"弱分类器特征的类别为:"<<w_c[i].haar.kind<<endl;
        cout<<"弱分类器的权重为:"<<w_c[i].weight<<endl;
        cout<<"弱分类器不等式的方向为:"<<w_c[i].p<<endl;
        cout<<"弱分类器的错误率为:"<<w_c[i].rate<<endl;
        cout<<"坐标为:"<<endl;
        cout<<w_c[i].point1.x_axis<<","<<w_c[i].point1.y_axis<<endl;
        cout<<w_c[i].point2.x_axis<<","<<w_c[i].point2.y_axis<<endl;
    }
}

//权重更新；
void AdaBoost_Trainning::Weight_Renew(Weak_Classifier w,double b)
{
    int i;//临时变量；
    int P_num=0;//正类判断正确的数量；
    
    //更新所有正类样本的权重；
    for(i=0;i<h_f_t.P_Sample.size();i++)
    {
        int h=h_f_t.Weak_judge(w, w.p, i, 1);
        int ei;
        
        if(h==1)
        {
            ei=0;//当前正类样本的类别判断正确；
            P_num++;
        }
        else
        {
            ei=1;//当前负类样本的类别判断错误；
        }
        double weight_temp=h_f_t.P_Sample[i].weight*pow(b,1-ei);
        h_f_t.P_Sample[i].weight=weight_temp;
    }
    
    int M_num=0;//负类判断正确的数量；
    //更新所有负类样本的权重；
    for(i=0;i<h_f_t.M_Sample.size();i++)
    {
        int h=h_f_t.Weak_judge(w, w.p, i, 0);
        int ei;
        
        if(h==1)
        {
            ei=1;//当前负类样本的类别判断错误；
        }
        else
        {
            ei=0;//当前负类样本的类别判断正确；
            M_num++;
        }
        double weight_temp=h_f_t.M_Sample[i].weight*pow(b,1-ei);
        h_f_t.M_Sample[i].weight=weight_temp;
    }
    cout<<"正类判断正确的一共为:"<<P_num<<endl;
    cout<<"负类判断正确的一共为:"<<M_num<<endl;
}

//权重归一化；
void AdaBoost_Trainning::Weight_Normalization()
{
    int i;//临时变量；
    
    double weight_temp=0.0000;//临时变量；
    double weight_sum=0.0000;//临时变量；
    
    for(i=0;i<h_f_t.P_Sample.size();i++)
    {
        weight_sum+=h_f_t.P_Sample[i].weight;
    }
    for(i=0;i<h_f_t.M_Sample.size();i++)
    {
        weight_sum+=h_f_t.M_Sample[i].weight;;
    }
    for(i=0;i<h_f_t.P_Sample.size();i++)
    {
        weight_temp=h_f_t.P_Sample[i].weight;
        h_f_t.P_Sample[i].weight=weight_temp/(weight_sum*1.000);
    }
    for(i=0;i<h_f_t.M_Sample.size();i++)
    {
        weight_temp=h_f_t.M_Sample[i].weight;
        h_f_t.M_Sample[i].weight=weight_temp/(weight_sum*1.000);
    }
}
