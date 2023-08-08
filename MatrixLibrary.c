#include<math.h>
#include<stdio.h>
#include<stdlib.h>

typedef struct point{
    double x;
    double y;
    double z;
}Point;

typedef struct color {
    unsigned char R;
    unsigned char G;
    unsigned char B;
}Colorchar;

typedef struct vector{
    double X;
    double Y;
    double Z;
}Vector;

typedef struct halfLine{
    Point start;
    Vector direct;
}Ray;

typedef struct modelSurfaces{
    Point edge[3];
    Colorchar color;
    unsigned char alpha;
    unsigned char reflect;
    double n;
}ModelSurface;

typedef struct pointLight{
    Point start;
    Colorchar color;
}LightSource;

typedef struct camara{
    Point position;
    Vector directAndDepth;
    Vector Yvect;
    Vector Xvect;
}Camara;

typedef struct matrix3{
    double a,b,c,d,e,f,g,h,i;
}Matrix3;

typedef struct bufferArea{
    double d1;
    char flag;
    Point p1;
    Vector v1;
    ModelSurface s1;
    Ray h1;
    Colorchar c1;
}bufferArea;

//测试用
void printVect(Vector a){
    printf("-->%.14lf,%.14lf,%.14lf\n",a.X,a.Y,a.Z);
}
void printPoint(Point a){
    printf("==>%.14lf,%.14lf,%.14lf\n",a.x,a.y,a.z);
}
void printMatrix3(Matrix3 a){
    printf("--<%.14lf,%.14lf,%.14lf>\n",a.a,a.b,a.c);
    printf("--<%.14lf,%.14lf,%.14lf>\n",a.d,a.e,a.f);
    printf("--<%.14lf,%.14lf,%.14lf>\n",a.g,a.h,a.i);
}

//写入
Point setPoint(double x,double y,double z){
    Point result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}
Vector setVector(double x,double y,double z){
    Vector result;
    result.X = x;
    result.Y = y;
    result.Z = z;
    return result;
}


//点操作

Point vector2point(Vector a){
    Point c;
    c.x=a.X;
    c.y=a.Y;
    c.z=a.Z;
    return c;
}

//向量操作

Vector getVectorFromPoint(Point start,Point direct){
    Vector result;
    result.X =direct.x - start.x; 
    result.Y =direct.y - start.y;
    result.Z =direct.z - start.z;
    return result;
}

double dotProduct(Vector a,Vector b){
    double result;
    result = a.X*b.X+a.Y*b.Y+a.Z*b.Z;
    return result;
}

Vector crossProduct(Vector a,Vector b){
    Vector result;
    result.X=a.Y*b.Z-a.Z*b.Y;
    result.Y=a.Z*b.X-a.X*b.Z;
    result.Z=a.X*b.Y-a.Y*b.X;
    return result;
}

double getVectorLenth(Vector a){
    int lenth;
    lenth = sqrt(a.X*a.X+a.Y*a.Y+a.Z*a.Z);
    return lenth;
}

Vector unitization(Vector a){
    Vector b;
    b.X=a.X/getVectorLenth(a);
    b.Y=a.Y/getVectorLenth(a);
    b.Z=a.Z/getVectorLenth(a);
    return b;
}

Vector vectorAdjust(Vector a,Vector b,double f,double k,Vector c){
    Vector result;
    result.X=f*(a.X)+k*(b.X)+c.X;
    result.Y=f*(a.Y)+k*(b.Y)+c.Y;
    result.Z=f*(a.Z)+k*(b.Z)+c.Z;
    return result;
}

Vector getNormalVector(ModelSurface a){
    Vector a1,a2;
    a1=getVectorFromPoint(a.edge[0],a.edge[1]);
    a2=getVectorFromPoint(a.edge[1],a.edge[2]);
    a1=crossProduct(a1,a2);
    return a1;
}

//矩阵操作
void writePointIntoMatrix(struct matrix3 *a,Point b,char typeAndPosition){
    if (typeAndPosition==11){
        a->a=b.x;
        a->b=b.y;
        a->c=b.z;
    }
    if (typeAndPosition==12){
        a->d=b.x;
        a->e=b.y;
        a->f=b.z;
    }
    if (typeAndPosition==13){
        a->g=b.x;
        a->h=b.y;
        a->i=b.z;
    }
    if (typeAndPosition==21){
        a->a=b.x;
        a->d=b.y;
        a->g=b.z;
    }
    if (typeAndPosition==22){
        a->b=b.x;
        a->e=b.y;
        a->h=b.z;
    }
    if (typeAndPosition==23){
        a->c=b.x;
        a->f=b.y;
        a->i=b.z;
    }
}//写入位置分两部分，13代表写入第三行，23代表写入第三列，21代表写入第一列


double determinant(struct matrix3 a){
    double a1,a2,a3,a4,a5,a6,result;
    a1=(a.a)*(a.e)*(a.i);
    a2=(a.b)*(a.f)*(a.g);
    a3=(a.c)*(a.d)*(a.h);

    a4=(a.c)*(a.e)*(a.g);
    a5=(a.a)*(a.f)*(a.h);
    a6=(a.b)*(a.d)*(a.i);

    result=(a1+a2+a3)-(a4+a5+a6);
    return result;
}
//线面求交
Point getPoint(Ray a,ModelSurface b){
    struct matrix3 s1,s2,s3,s4;
    Vector f3,f2,f1 = {0,0,1};//待测试特殊情况
    Point p1,p2,p3,p4,result;

    f3=getVectorFromPoint(b.edge[0],b.edge[1]);
    f2=getVectorFromPoint(b.edge[1],b.edge[2]);
    f3=crossProduct(f3,f2);
    f1=crossProduct(a.direct,f1);
    if (getVectorLenth(f1)==0) {
        f1.X=1;
        f1=crossProduct(a.direct,f1);
    }
    f2=crossProduct(a.direct,f1);


    p1=vector2point(f1);
    p2=vector2point(f2);
    p3=vector2point(f3);
    writePointIntoMatrix(&s1,p1,11);
    writePointIntoMatrix(&s1,p2,12);
    writePointIntoMatrix(&s1,p3,13);

    p4.x=f1.X*(a.start.x)+f1.Y*(a.start.y)+f1.Z*(a.start.z);
    p4.y=f2.X*(a.start.x)+f2.Y*(a.start.y)+f2.Z*(a.start.z);
    p4.z=f3.X*(b.edge[0].x)+f3.Y*(b.edge[0].y)+f3.Z*(b.edge[0].z);

    s2=s1;
    s3=s1;
    s4=s1;

    writePointIntoMatrix(&s2,p4,21);
    writePointIntoMatrix(&s3,p4,22);
    writePointIntoMatrix(&s4,p4,23);

    result.x=determinant(s2)/determinant(s1);
    result.y=determinant(s3)/determinant(s1);
    result.z=determinant(s4)/determinant(s1);
    return result;
}



int isPointInSurface(ModelSurface a,Point b){
    Vector v1,v2,v3,v4,v5;
    double s1;
    v1=getVectorFromPoint(a.edge[1],a.edge[0]);
    v2=getVectorFromPoint(a.edge[2],a.edge[0]);
    v3=getVectorFromPoint(b,a.edge[0]);
    v4=crossProduct(v1,v3);
    v5=crossProduct(v3,v2);
    s1=dotProduct(v4,v5);
    if (s1>0){
        v1=getVectorFromPoint(a.edge[2],a.edge[1]);
        v2=getVectorFromPoint(a.edge[0],a.edge[1]);
        v3=getVectorFromPoint(b,a.edge[1]);
        v4=crossProduct(v1,v3);
        v5=crossProduct(v3,v2);
        s1=dotProduct(v4,v5);
        if (s1>0) return 1;
        else return 0;
    }
    else return 0;
}//在三角形内返回1，否则返回0


Vector matrixMultiVector(struct matrix3 m,Vector v){
    Vector result;
    result.X=m.a*v.X+m.b*v.Y+m.c*v.Z;
    result.Y=m.d*v.X+m.e*v.Y+m.f*v.Z;
    result.Z=m.g*v.X+m.h*v.Y+m.i*v.Z;
    return result;
}

Vector spinAround(Vector a,Vector b,double angle){
    double c,s,c1;
    struct matrix3 m;
    Vector result;
    c=cos(angle);
    s=sin(angle);
    c1=1-c;
    m.a=c+c1*a.X*a.X;
    m.b=c1*a.X*a.Y-s*a.Z;
    m.c=c1*a.X*a.Z+s*a.Y;
    m.d=c1*a.X*a.Y+s*a.Z;
    m.e=c+c1*a.Y*a.Y;
    m.f=c1*a.Y*a.Z-s*a.X;
    m.g=c1*a.X*a.Z-s*a.Y;
    m.h=c1*a.Y*a.Z-s*a.X;
    m.i=c+c1*a.Z*a.Z;
    result = matrixMultiVector(m,result);
    return result;
}

Vector FLIP(Vector a,Vector b){
    struct matrix3 m;
    Vector result;
    result.X = -b.X;
    result.Y = -b.Y;
    result.Z = -b.Z;
    m.a=2*a.X*a.X-1;
    m.b=2*a.X*a.Y;
    m.c=2*a.X*a.Z;
    m.d=m.b;
    m.e=2*a.Y*a.Y-1;
    m.f=2*a.Y*a.Z;
    m.g=m.c;
    m.h=m.f;
    m.i=2*a.Z*a.Z-1;
    result = matrixMultiVector(m,result);
    return result;
}

Ray calReflect(Vector a,Point b,Vector c){
    Ray result;
    result.start=b;
    result.direct = FLIP(a,c);
    return result;
}

double calSpinAngle(Vector a,Vector c,double n){
    double a1,a2,result;
    a1 = dotProduct(a,c)/(getVectorLenth(a)*getVectorLenth(c));
    a2 = a1/n;
    a1 = (double)asin(a1);
    a2 = (double)asin(a2);
    //这里计算角度
    return result;
}

Ray calRefraction(Vector a,Point b,Vector c,double n){
    double angles;
    Ray result;
    angles = calSpinAngle(a,c,n);
    result.start = b;
    result.direct = spinAround(a,c,angles);//这里改成叉乘过后的向量
    return result;
}

Colorchar colorFilt(Colorchar filter,Colorchar basecolor){
    Colorchar result;
    result.R = basecolor.R*(filter.R/255);
    result.G = basecolor.G*(filter.G/255);
    result.B = basecolor.B*(filter.B/255);
    return result;
}

Colorchar colorAdjust(Colorchar a,double alpha,Colorchar background){
    Colorchar result;
    result.R=(char)(alpha/255*background.R+(255-alpha)/255*a.R);
    result.G=(char)(alpha/255*background.G+(255-alpha)/255*a.G);
    result.B=(char)(alpha/255*background.B+(255-alpha)/255*a.B);
    return result;
}

Colorchar colorPlus(Colorchar a,Colorchar b){
    Colorchar result;
    result.R=a.R+b.R;
    result.G=a.G+b.G;
    result.B=a.B+b.B;
    return result;
}