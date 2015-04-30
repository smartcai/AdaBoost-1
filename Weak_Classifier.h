//
//  Weak_Classifier.h
//  基于AdaBoost算法的人脸检测
//
//  Created by 唧唧歪歪 on 15/2/27.
//  Copyright (c) 2015年 唧唧歪歪. All rights reserved.
//

#ifndef W_C
#define W_C

#include"Haar_Feature.h"
#include"Coordinate.h"

class Weak_Classifier
{
public:
    Haar_Feature haar;//haar特征；
    Coordinate point1;
    Coordinate point2;
    
    int threshold;//当前弱分类器的阈值；
    int p;//弱分类器方向；
    double rate;//弱分类器错误率；
    double weight;//弱分类器权重；
};
#endif
