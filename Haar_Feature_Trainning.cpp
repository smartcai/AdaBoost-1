//
//  Haar_Feature_Trainning.cpp
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/2/27.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//
#include "Haar_Feature_Trainning.h"

//训练弱分类器函数；
void Haar_Feature_Trainning::Trainning(Haar_Feature haar,string table_name,int t)
{
    //所有特征值已经成功载入；
    /*
    //载入所有Haar特征下的Haar值；
    if(t==0)
    {
        Get_All(haar, table_name);
    }
    */
    
    //创建连接；
    MYSQL mycon;
    mysql_init(&mycon);
    mysql_real_connect(&mycon, "localhost", "root", "", "adaboost", 3306, NULL, 0);
    string sql;//SQL语句；
    MYSQL_RES *result=NULL;
    
    sql="select * from "+table_name;
    mysql_real_query(&mycon, sql.c_str(), sql.length());
    result=mysql_store_result(&mycon);
    int rowcount=(int)mysql_num_rows(result);
    
    int i;//临时变量；
    double min_wrong_rate=50000.000;//作为最小的误差率；
    for(i=0;i<rowcount;i++)
    {
        int H;//弱预测的值；
        MYSQL_ROW row=mysql_fetch_row(result);
        
        Weak_Classifier w_c;//临时弱分类器变量；
        w_c.threshold=atoi(row[1]);
        w_c.haar=haar;
        
        w_c.point1.x_axis=atoi(row[3]);
        w_c.point1.y_axis=atoi(row[4]);
        w_c.point2.x_axis=atoi(row[5]);
        w_c.point2.y_axis=atoi(row[6]);
        
        int num;
        //使用p=1作为判断不等式方向的值；
        double wrong_rate1=0.0000;//P＝1时，所有特征对应错误率；
        for(num=0;num<P_Sample.size();num++)
        {
            H=Weak_judge(w_c, 1, num, 1);
            wrong_rate1+=P_Sample[num].weight*abs(H-1);
        }
        for(num=0;num<M_Sample.size();num++)
        {
            H=Weak_judge(w_c, 1, num, 0);
            wrong_rate1+=M_Sample[num].weight*H;
        }
        
        //使用p=-1作为判断不等式方向的值；
        double wrong_rate2=0.0000;//P=0时，所有特征对应错误率；
        for(num=0;num<P_Sample.size();num++)
        {
            H=Weak_judge(w_c,-1, num, 1);
            wrong_rate2+=P_Sample[num].weight*abs(H-1);
        }
        for(num=0;num<M_Sample.size();num++)
        {
            H=Weak_judge(w_c, -1, num, 0);
            wrong_rate2+=M_Sample[num].weight*H;
        }
        //求解出最小的弱分类器；
        if(wrong_rate1<=wrong_rate2)
        {
            w_c.p=1;//弱分类器不等式方向；
            w_c.rate=wrong_rate1;//弱分类器误差率；
        }
        else
        {
            w_c.p=-1;
            w_c.rate=wrong_rate2;
        }
        //初始权重；
        w_c.weight=0.0000;//弱分类器的权重；
        if(w_c.rate<min_wrong_rate)
        {
            min_wrong_rate=w_c.rate;
            w=w_c;
        }
    }
    mysql_close(&mycon);
}

//获取当前特征对应的所有Haar特征值；
void Haar_Feature_Trainning::Get_All(Haar_Feature haar,string table_name)
{
    MYSQL mycon;
    mysql_init(&mycon);
    mysql_real_connect(&mycon, "localhost", "root", "", "adaboost", 3306, NULL, 0);
    
    int num;//临时变量；
    int id=1;//纪录特征值的ID；
    
    for(num=0;num<P_Sample.size();num++)
    {
        int width=P_Sample[num].image.rows-1;
        int length=P_Sample[num].image.cols-1;
        
        int i,j;//临时变量；
        for(i=1;i<width-haar.s;i++)
        {
            for(j=1;j<length-haar.t;j++)
            {
                int w=P_Sample[num].image.rows-i;
                w=Low_Integral_Function(w,haar.s);
                
                int l=P_Sample[num].image.cols-j;
                l=Low_Integral_Function(l,haar.t);
                
                int x1,y1;
                x1=i;
                y1=j;
                
                Coordinate A_Point;
                A_Point.x_axis=x1;
                A_Point.y_axis=y1;
                
                int m,n;//临时变量；
                
                for(m=2;m<=w;m++)
                {
                    int x2,y2;
                    x2=x1+m*haar.s;
                    for(n=2;n<=l;n++)
                    {
                        y2=y1+n*haar.t;
                        
                        Coordinate D_Point;
                        D_Point.x_axis=x2;
                        D_Point.y_axis=y2;
                        
                        int integral=get_haar1(haar, A_Point, D_Point, num);
                        
                        string sql;
                        sql="insert into "+table_name+" values (";
                        sql+=Convert(id)+",";
                        sql+=Convert(integral)+",";
                        sql+=Convert(1)+",";
                        sql+=Convert(A_Point.x_axis)+",";
                        sql+=Convert(A_Point.y_axis)+",";
                        sql+=Convert(D_Point.x_axis)+",";
                        sql+=Convert(D_Point.y_axis)+")";
                        id++;
                        
                        mysql_real_query(&mycon,sql.c_str(),sql.length());
                    }
                }
            }
        }
    }
    
    for(num=0;num<M_Sample.size();num++)
    {
        int width=M_Sample[num].image.rows-1;
        int length=M_Sample[num].image.cols-1;
        
        int i,j;//临时变量；
        
        for(i=1;i<width-haar.s;i++)
        {
            for(j=1;j<length-haar.t;j++)
            {
                int w=M_Sample[num].image.rows-i;
                w=Low_Integral_Function(w,haar.s);
                
                int l=M_Sample[num].image.cols-j;
                l=Low_Integral_Function(l,haar.t);
                
                int x1=i;
                int y1=j;
                
                Coordinate A_Point;
                A_Point.x_axis=x1;
                A_Point.y_axis=y1;
                
                int m,n;//临时变量；
                
                for(m=2;m<=w;m++)
                {
                    int x2=x1+m*haar.s;
                    
                    for(n=2;n<=l;n++)
                    {
                        int y2=y1+n*haar.t;
                        
                        Coordinate D_Point;
                        D_Point.x_axis=x2;
                        D_Point.y_axis=y2;
                        
                        int integral=get_haar2(haar,A_Point,D_Point,num);
                        
                        string sql;
                        sql="insert into "+table_name+" values (";
                        sql+=Convert(id)+",";
                        sql+=Convert(integral)+",";
                        sql+=Convert(0)+",";
                        sql+=Convert(A_Point.x_axis)+",";
                        sql+=Convert(A_Point.y_axis)+",";
                        sql+=Convert(D_Point.x_axis)+",";
                        sql+=Convert(D_Point.y_axis)+")";
                        id++;
                        mysql_real_query(&mycon,sql.c_str(),sql.length());
                    }
                }
            }
        }
    }
    mysql_close(&mycon);
    //mysql_query(&mycon,"COMMIT");//执行事务;
}

//使用弱分类器判断当前样本的正负类;
int Haar_Feature_Trainning::Weak_judge(Weak_Classifier w_c,int p,int num,int type)
{
    int integral;//当前样本的特征值；
    if(type==1)
    {
        integral=get_haar1(w_c.haar, w_c.point1, w_c.point2, num);
    }
    else
    {
        integral=get_haar2(w_c.haar, w_c.point1, w_c.point2, num);
    }
    
    int h;//当前样本的种类；
    if(p*integral<p*w_c.threshold)
    {
        h=1;
    }
    else
    {
        h=0;
    }
    return h;
}

//计算正类样本特征值；
int Haar_Feature_Trainning::get_haar1(Haar_Feature haar, Coordinate A_Point, Coordinate D_Point, int num)
{
    Coordinate B_Point,C_Point;
    int W_Z=0,B_Z=0;
    int integral=0;
    
    B_Point.x_axis=A_Point.x_axis;
    B_Point.y_axis=D_Point.y_axis;
    
    C_Point.x_axis=D_Point.x_axis;
    C_Point.y_axis=A_Point.y_axis;
    
    switch(haar.kind)
    {
        case 1:
        {
            Coordinate C1,C2;
            C1.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C1.y_axis=A_Point.y_axis;
            
            C2.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C2.y_axis=D_Point.y_axis;
            
            W_Z=P_Sample[num].integral_image[C2.x_axis][C2.y_axis]+P_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-P_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-P_Sample[num].integral_image[C1.x_axis][C1.y_axis];
            B_Z=P_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
        };break;
        case 2:
        {
            Coordinate C1,C2;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=C_Point.x_axis;
            C2.y_axis=(D_Point.y_axis-C_Point.y_axis)/haar.t+C_Point.y_axis;
            
            W_Z=P_Sample[num].integral_image[C2.x_axis][C2.y_axis]+P_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
            B_Z=P_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-P_Sample[num].integral_image[C2.x_axis][C2.y_axis];
        };break;
        case 3:
        {
            Coordinate C1,C2,C3,C4;
            C1.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C1.y_axis=A_Point.y_axis;
            
            C2.x_axis=(D_Point.x_axis-B_Point.x_axis)/haar.s+B_Point.x_axis;
            C2.y_axis=B_Point.y_axis;
            
            C3.x_axis=(C_Point.x_axis-A_Point.x_axis)*(haar.s-1)/haar.s+A_Point.x_axis;
            C3.y_axis=A_Point.y_axis;
            
            C4.x_axis=(D_Point.x_axis-B_Point.x_axis)*(haar.s-1)/haar.s+B_Point.x_axis;
            C4.y_axis=B_Point.y_axis;
            
            W_Z=P_Sample[num].integral_image[C2.x_axis][C2.y_axis]+P_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]+P_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+P_Sample[num].integral_image[C3.x_axis][C3.y_axis]-P_Sample[num].integral_image[C4.x_axis][C4.y_axis]-P_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
            B_Z=P_Sample[num].integral_image[C4.x_axis][C4.y_axis]+P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[C4.x_axis][C4.y_axis];
            B_Z=B_Z*2;
        };break;
        case 4:
        {
            Coordinate C1,C2,C3,C4;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=A_Point.x_axis;
            C2.y_axis=(B_Point.y_axis-A_Point.y_axis)*(haar.t-1)/haar.t+A_Point.y_axis;
            
            C3.x_axis=C_Point.x_axis;
            C3.y_axis=(D_Point.x_axis-C_Point.x_axis)/haar.t+C_Point.x_axis;
            
            C4.x_axis=C_Point.x_axis;
            C4.y_axis=(D_Point.y_axis-C_Point.y_axis)*(haar.t-1)/haar.t+C_Point.y_axis;
            
            W_Z=P_Sample[num].integral_image[C3.x_axis][C3.y_axis]+P_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis]+P_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-P_Sample[num].integral_image[C4.x_axis][C4.y_axis];
            B_Z=P_Sample[num].integral_image[C4.x_axis][C4.y_axis]+P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[C3.x_axis][C3.y_axis];
            B_Z=B_Z*2;
        };break;
        case 5:
        {
            Coordinate C1,C2,C3,C4,C5;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C2.y_axis=A_Point.y_axis;
            
            C3.x_axis=C2.x_axis;
            C3.y_axis=C1.y_axis;
            
            C4.x_axis=C2.x_axis;
            C4.y_axis=B_Point.y_axis;
            
            C5.x_axis=C_Point.x_axis;
            C5.y_axis=C1.y_axis;
            
            W_Z=P_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+P_Sample[num].integral_image[C3.x_axis][C3.y_axis]-P_Sample[num].integral_image[C4.x_axis][C4.y_axis]-P_Sample[num].integral_image[C5.x_axis][C5.y_axis]+P_Sample[num].integral_image[C3.x_axis][C3.y_axis]+P_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C2.x_axis][C2.y_axis];
            B_Z=P_Sample[num].integral_image[C5.x_axis][C5.y_axis]+P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[C3.x_axis][C3.y_axis]-P_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis]+P_Sample[num].integral_image[C4.x_axis][C4.y_axis]+P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-P_Sample[num].integral_image[C3.x_axis][C3.y_axis];
        };break;
        case 6:
        {
            Coordinate C1,C2,C3,C4;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=A_Point.x_axis;
            C2.y_axis=(B_Point.y_axis-A_Point.y_axis)*(haar.t-1)/haar.t+A_Point.y_axis;
            
            C3.x_axis=C_Point.x_axis;
            C3.y_axis=(D_Point.x_axis-C_Point.x_axis)/haar.t+C_Point.x_axis;
            
            C4.x_axis=C_Point.x_axis;
            C4.y_axis=(D_Point.y_axis-C_Point.y_axis)*(haar.t-1)/haar.t+C_Point.y_axis;
            
            W_Z=P_Sample[num].integral_image[C4.x_axis][C4.y_axis]+P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[C3.x_axis][C3.y_axis];
            B_Z=P_Sample[num].integral_image[C3.x_axis][C3.y_axis]+P_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis]+P_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-P_Sample[num].integral_image[C4.x_axis][C4.y_axis];
            
            W_Z=W_Z*2;
        };break;
        case 7:
        {
            Coordinate C1,C2;
            C1.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C1.y_axis=A_Point.y_axis;
            
            C2.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C2.y_axis=D_Point.y_axis;
            
            W_Z=P_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+P_Sample[num].integral_image[C1.x_axis][C1.y_axis]-P_Sample[num].integral_image[C2.x_axis][C2.y_axis]-P_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
            B_Z=P_Sample[num].integral_image[C2.x_axis][C2.y_axis]+P_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-P_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-P_Sample[num].integral_image[C1.x_axis][C1.y_axis];
        };break;
    }
    integral=W_Z-B_Z;
    return integral;
}

//计算负类样本特征值；
int Haar_Feature_Trainning::get_haar2(Haar_Feature haar,Coordinate A_Point,Coordinate D_Point,int num)
{
    Coordinate B_Point,C_Point;
    int W_Z=0,B_Z=0;
    int integral=0;
    
    B_Point.x_axis=A_Point.x_axis;
    B_Point.y_axis=D_Point.y_axis;
    
    C_Point.x_axis=D_Point.x_axis;
    C_Point.y_axis=A_Point.y_axis;
    
    switch(haar.kind)
    {
        case 1:
        {
            Coordinate C1,C2;
            C1.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C1.y_axis=A_Point.y_axis;
            
            C2.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C2.y_axis=D_Point.y_axis;
            
            W_Z=M_Sample[num].integral_image[C2.x_axis][C2.y_axis]+M_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-M_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-M_Sample[num].integral_image[C1.x_axis][C1.y_axis];
            B_Z=M_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
        };break;
        case 2:
        {
            Coordinate C1,C2;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=C_Point.x_axis;
            C2.y_axis=(D_Point.y_axis-C_Point.y_axis)/haar.t+C_Point.y_axis;
            
            W_Z=M_Sample[num].integral_image[C2.x_axis][C2.y_axis]+M_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
            B_Z=M_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-M_Sample[num].integral_image[C2.x_axis][C2.y_axis];
        };break;
        case 3:
        {
            Coordinate C1,C2,C3,C4;
            C1.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C1.y_axis=A_Point.y_axis;
            
            C2.x_axis=(D_Point.x_axis-B_Point.x_axis)/haar.s+B_Point.x_axis;
            C2.y_axis=B_Point.y_axis;
            
            C3.x_axis=(C_Point.x_axis-A_Point.x_axis)*(haar.s-1)/haar.s+A_Point.x_axis;
            C3.y_axis=A_Point.y_axis;
            
            C4.x_axis=(D_Point.x_axis-B_Point.x_axis)*(haar.s-1)/haar.s+B_Point.x_axis;
            C4.y_axis=B_Point.y_axis;
            
            W_Z=M_Sample[num].integral_image[C2.x_axis][C2.y_axis]+M_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]+M_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+M_Sample[num].integral_image[C3.x_axis][C3.y_axis]-M_Sample[num].integral_image[C4.x_axis][C4.y_axis]-M_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
            B_Z=M_Sample[num].integral_image[C4.x_axis][C4.y_axis]+M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[C4.x_axis][C4.y_axis];
            B_Z=B_Z*2;
        };break;
        case 4:
        {
            Coordinate C1,C2,C3,C4;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=A_Point.x_axis;
            C2.y_axis=(B_Point.y_axis-A_Point.y_axis)*(haar.t-1)/haar.t+A_Point.y_axis;
            
            C3.x_axis=C_Point.x_axis;
            C3.y_axis=(D_Point.x_axis-C_Point.x_axis)/haar.t+C_Point.x_axis;
            
            C4.x_axis=C_Point.x_axis;
            C4.y_axis=(D_Point.y_axis-C_Point.y_axis)*(haar.t-1)/haar.t+C_Point.y_axis;
            
            W_Z=M_Sample[num].integral_image[C3.x_axis][C3.y_axis]+M_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis]+M_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-M_Sample[num].integral_image[C4.x_axis][C4.y_axis];
            B_Z=M_Sample[num].integral_image[C4.x_axis][C4.y_axis]+M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[C3.x_axis][C3.y_axis];
            B_Z=B_Z*2;
        };break;
        case 5:
        {
            Coordinate C1,C2,C3,C4,C5;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C2.y_axis=A_Point.y_axis;
            
            C3.x_axis=C2.x_axis;
            C3.y_axis=C1.y_axis;
            
            C4.x_axis=C2.x_axis;
            C4.y_axis=B_Point.y_axis;
            
            C5.x_axis=C_Point.x_axis;
            C5.y_axis=C1.y_axis;
            
            W_Z=M_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+M_Sample[num].integral_image[C3.x_axis][C3.y_axis]-M_Sample[num].integral_image[C4.x_axis][C4.y_axis]-M_Sample[num].integral_image[C5.x_axis][C5.y_axis]+M_Sample[num].integral_image[C3.x_axis][C3.y_axis]+M_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C2.x_axis][C2.y_axis];
            B_Z=M_Sample[num].integral_image[C5.x_axis][C5.y_axis]+M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[C3.x_axis][C3.y_axis]-M_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis]+M_Sample[num].integral_image[C4.x_axis][C4.y_axis]+M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-M_Sample[num].integral_image[C3.x_axis][C3.y_axis];
        };break;
        case 6:
        {
            Coordinate C1,C2,C3,C4;
            C1.x_axis=A_Point.x_axis;
            C1.y_axis=(B_Point.y_axis-A_Point.y_axis)/haar.t+A_Point.y_axis;
            
            C2.x_axis=A_Point.x_axis;
            C2.y_axis=(B_Point.y_axis-A_Point.y_axis)*(haar.t-1)/haar.t+A_Point.y_axis;
            
            C3.x_axis=C_Point.x_axis;
            C3.y_axis=(D_Point.x_axis-C_Point.x_axis)/haar.t+C_Point.x_axis;
            
            C4.x_axis=C_Point.x_axis;
            C4.y_axis=(D_Point.y_axis-C_Point.y_axis)*(haar.t-1)/haar.t+C_Point.y_axis;
            
            W_Z=M_Sample[num].integral_image[C4.x_axis][C4.y_axis]+M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[C3.x_axis][C3.y_axis];
            B_Z=M_Sample[num].integral_image[C3.x_axis][C3.y_axis]+M_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis]+M_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-M_Sample[num].integral_image[C4.x_axis][C4.y_axis];
            
            W_Z=W_Z*2;
        };break;
        case 7:
        {
            Coordinate C1,C2;
            C1.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C1.y_axis=A_Point.y_axis;
            
            C2.x_axis=(C_Point.x_axis-A_Point.x_axis)/haar.s+A_Point.x_axis;
            C2.y_axis=D_Point.y_axis;
            
            W_Z=M_Sample[num].integral_image[D_Point.x_axis][D_Point.y_axis]+M_Sample[num].integral_image[C1.x_axis][C1.y_axis]-M_Sample[num].integral_image[C2.x_axis][C2.y_axis]-M_Sample[num].integral_image[C_Point.x_axis][C_Point.y_axis];
            B_Z=M_Sample[num].integral_image[C2.x_axis][C2.y_axis]+M_Sample[num].integral_image[A_Point.x_axis][A_Point.y_axis]-M_Sample[num].integral_image[B_Point.x_axis][B_Point.y_axis]-M_Sample[num].integral_image[C1.x_axis][C1.y_axis];
        };break;
    }
    integral=W_Z-B_Z;
    return integral;
}

//将整形类型转换为字符串类型；
string Haar_Feature_Trainning::Convert(int temp)
{
    stringstream ss;
    string s;
    ss<<temp;
    ss>>s;
    return s;
}

//将浮点型转换为字符串类型；
string Haar_Feature_Trainning::Convert(double temp)
{
    stringstream ss;
    string s;
    ss<<temp;
    ss>>s;
    return s;
}

//取下整函数；
int Haar_Feature_Trainning::Low_Integral_Function(int x1,int x2)
{
    if(x1%x2==0)
    {
        return x1/x2-1;
    }
    else
    {
        return x1/x2;
    }
}




