//
//  Cascade_Classifier_Trainning.cpp
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/3/2.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#include "Cascade_Classifier_Trainning.h"

//实现级联训练；
void Cascade_Classifier_Trainning::Cascade_Trainning()
{
    double F_max=0.5;
    double D_min=0.99;
    double F_target=0.09;
    
    F=1.0;
    D=1.0;
    
    int i=0;
    int n=0;
    
    a_t.h_f_t.P_Sample=s_i.P_Sample;//获取所有正类的样本；
    a_t.h_f_t.M_Sample=s_i.M_Sample;//获取所有负类的样本；
    
    do
    {
        int j;
        i=i+1;
        do
        {
            n++;
            a_t.Init(n);//初始化AdaBoost中样本的数量以及强分类器的集合；
            a_t.AdaBoostTrainning();//在AdaBoost训练算法中,自动将AdaBoost中当前的样本信息加载到Haar_Feature_Trainning类的对象中；
            
            d_t=Strong_judge1(a_t.w_c);
            f_t=Strong_judge2(a_t.w_c);
            
            while(d_t<=D_min)
            {
                //改变第i个强分类器的n个弱分类器的阈值；
                for(j=0;j<n;j++)
                {
                    if(a_t.w_c[j].p==1)
                    {
                        a_t.w_c[j].threshold+=1;
                    }
                    else
                    {
                        a_t.w_c[j].threshold-=1;
                    }
                }
                d_t=Strong_judge1(a_t.w_c);
                f_t=Strong_judge2(a_t.w_c);
            }
        }while(f_t>=F_max);
        
        cout<<"第"<<i<<"个强分类器为:"<<endl;
        
        for(j=0;j<n;j++)
        {
            cout<<"Theta="<<a_t.w_c[j].threshold<<" "<<"X1="<<a_t.w_c[j].point1.x_axis<<" Y1="<<a_t.w_c[j].point1.y_axis<<endl;
            cout<<"X2="<<a_t.w_c[j].point2.x_axis<<" Y2="<<a_t.w_c[j].point2.y_axis<<endl;
            cout<<"P="<<a_t.w_c[j].p<<endl;
            cout<<"Weight="<<a_t.w_c[j].weight<<endl;
        }
        
        //重新选择负类样本集内的样本；
        int H;//样本判断类别；
        vector<Trainning_Sample> t;//临时存放弱分类器的容器；
        for(j=0;j<a_t.h_f_t.M_Sample.size();j++)
        {
            H=Strong_judge2(a_t.w_c,j);
            if(H==1)
            {
                t.push_back(a_t.h_f_t.M_Sample[j]);
            }
        }
        a_t.h_f_t.M_Sample.clear();
        for(j=0;j<t.size();j++)
        {
            a_t.h_f_t.M_Sample.push_back(t[j]);
        }
        F=F*f_t;
    }while(F>=F_target);
}

//判断第num副非人脸是否属于非人脸类；
int Cascade_Classifier_Trainning::Strong_judge2(vector<Weak_Classifier> w_c,int num)
{
    int i;
    int H;
    
    double sum1=0.0000;
    double sum2=0.0000;
    
    for(i=0;i<w_c.size();i++)
    {
        int h;
        h=a_t.h_f_t.Weak_judge(w_c[i], w_c[i].p, num, 0);
        sum1+=w_c[i].weight*h;
        sum2+=w_c[i].weight;
    }
    if(sum1>=0.5*sum2)
    {
        H=1;
        cout<<"第"<<num<<"个负类分类错误"<<endl;
    }
    else
    {
        H=0;
        cout<<"第"<<num<<"个负类分类正确"<<endl;
    }
    return H;
}

//利用当前强分类器判断人脸集合中一共有多少人脸；
double Cascade_Classifier_Trainning::Strong_judge1(vector<Weak_Classifier> w_c)
{
    int num;//临时变量；
    int face=0;//人脸的数量；
    
    for(num=0;num<a_t.h_f_t.P_Sample.size();num++)
    {
        double sum1=0.0000;
        double sum2=0.0000;
        
        int H,h;//临时变量；
        int i;//临时变量；
        
        for(i=0;i<w_c.size();i++)
        {
            h=a_t.h_f_t.Weak_judge(w_c[i], w_c[i].p, num, 1);
            sum1+=w_c[i].weight*h;
            sum2+=w_c[i].weight;
        }
        if(sum1>=0.500*sum2)
        {
            H=1;
            face++;
        }
        else
        {
            H=0;
        }
    }
    return (face*1.0000)/a_t.h_f_t.P_Sample.size();
}

//利用当前强分类器判断非人脸集合中误判的人脸数量；
double Cascade_Classifier_Trainning::Strong_judge2(vector<Weak_Classifier> w_c)
{
    int num;
    int nonface=0;
    
    for(num=0;num<a_t.h_f_t.M_Sample.size();num++)
    {
        double sum1=0.0000;
        double sum2=0.0000;
        int i;
        int H;
        
        for(i=0;i<w_c.size();i++)
        {
            int h=a_t.h_f_t.Weak_judge(w_c[i], w_c[i].p, num, 0);//判断一共有多少人脸；
            sum1+=w_c[i].weight*h;
            sum2+=w_c[i].weight;
        }
        if(sum1>=0.5*sum2)
        {
            H=1;
            nonface++;
        }
        else
        {
            H=0;
        }
    }
    return (nonface*1.0000)/a_t.h_f_t.M_Sample.size();
}



