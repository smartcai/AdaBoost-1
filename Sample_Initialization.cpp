//
//  Sample_Initialization.cpp
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/3/9.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#include "Sample_Initialization.h"

//获取所有样本的名称；
void Sample_Initialization::GetFile()
{
    string filename="/Users/Project/pos.txt";
    ifstream P_infile(filename.c_str());
    string temp;
    while(getline(P_infile,temp))
    {
        temp="/Users/Project/Faces/"+temp;
        temp=temp.substr(0,temp.length()-1);
        P_File.push_back(temp);
    }
    
    filename="/Users/Project/neg.txt";
    ifstream M_infile(filename.c_str());
    while(getline(M_infile,temp))
    {
        temp="/Users/Project/NonFaces/"+temp;
        temp=temp.substr(0,temp.length()-1);
        M_File.push_back(temp);
    }
}
//从数据库中取出正负样本的数据存入两个容器中；
/*
//提取所有正类样本名称；
void Sample_Initialization::GetFile()
{
    int i;//表示临时变量；
    int rowcount=0;//表示受影响的行数；
    
    string sql;//表示SQL语句；
    string file_name;//表示获取的数据库内的名称；
    
    MYSQL mycon;
    mysql_init(&mycon);
    mysql_real_connect(&mycon, "localhost", "root", "", "adaboost", 3306, NULL, 0);//连接数据库；
    
    MYSQL_RES * result=NULL;
    MYSQL_ROW row=NULL;
    
    //提取所有人脸的样本的名称；
    sql="select * from face_list";
    mysql_real_query(&mycon, sql.c_str(), sql.length());
    result=mysql_store_result(&mycon);
    rowcount=(int)mysql_num_rows(result);//显示转换，将long转换为int;
    
    for(i=0;i<rowcount;i++)
    {
        row=mysql_fetch_row(result);//获取当前行的数据；
        
        file_name=row[0];
        P_File.push_back(file_name);
    }
    
    //提取所有非人脸样本的名称；
    sql="select * from nonface_list";
    mysql_real_query(&mycon, sql.c_str(), sql.length());
    result=mysql_store_result(&mycon);
    rowcount=(int)mysql_num_rows(result);
    
    for(i=0;i<rowcount;i++)
    {
        row=mysql_fetch_row(result);
        
        file_name=row[0];
        M_File.push_back((file_name));
    }
    mysql_close(&mycon);
}
*/

//求解积分图像；
Trainning_Sample Sample_Initialization::Calculate(Trainning_Sample t_s)
{
    int m,n;//临时变量；
    //数组初始化；
    for(m=0;m<t_s.image.rows;m++)
    {
        for(n=0;n<t_s.image.cols;n++)
        {
            t_s.integral_image[m][n]=0;
        }
    }
    int i,j;//临时变量；
    
    for(i=0;i<t_s.image.rows;i++)
    {
        for(j=0;j<t_s.image.cols;j++)
        {
            int temp=0;
            for(m=0;m<=i;m++)
            {
                for(n=0;n<=j;n++)
                {
                    int t=t_s.image.at<uchar>(m,n);
                    temp+=t;
                }
            }
            t_s.integral_image[i][j]=temp;
        }
    }
    return t_s;
}

//加载所有样本的初始信息；
void Sample_Initialization::Sample_Load()
{
    int i;//临时变量；
    double w1=1.0000/(P_T*2.0000);//初始化样本权重；
    
    //加载正类样本的所有信息；
    for(i=0;i<P_T;i++)
    {
        Mat img=imread(P_File[i]);
        
        Trainning_Sample t_s;
        t_s.image=img;
        t_s.type=1;
        t_s.weight=w1;
        t_s=Calculate(t_s);
        
        P_Sample.push_back(t_s);
    }
    
    double w2=1.0000/(M_T*2.0000);
    //加载负类样本的所有信息；
    for(i=0;i<M_T;i++)
    {
        Mat img=imread(M_File[i]);
        
        Trainning_Sample t_s;
        t_s.image=img;
        t_s.type=0;
        t_s.weight=w2;
        t_s=Calculate(t_s);
        
        M_Sample.push_back(t_s);
    }
}

