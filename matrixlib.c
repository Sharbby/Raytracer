#include<math.h>
#include<stdio.h>

struct point{
    double x;
    double y;
    double z;
};//点由空间三个坐标表示

struct color{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};//颜色为32位

struct vector{
    double x;
    double y;
    double z;
};//自由向量由三个方向的分向量表示

struct half_line{
    struct vector direct;
    struct point start;
};//射线由起点以及一个自由向量表示

struct surfaces{
    struct point edge_point[3];
    struct color RGB;
    unsigned char alpha;
    unsigned char reflect;
    double n;
};//一个表面由三个点确定和规制，采用三角面，有颜色，alpha透明度，reflect反射度 


struct pointLight{
    struct point sourcePoint;
    struct color RGB;
};

struct surfaceLight{
    struct surfaces sourceSurface;
    struct color RGB;
};

struct camara{
    struct point position;
    struct vector directAndDepth;
    struct vector Yvect;
    struct vector Xvect;
}defaultCamara;

struct matrix3{
    double a,b,c,d,e,f,g,h,i;
};

struct geoSurface{
    struct point position;
    struct vector normalVector;
};

struct bufferArea{
    double d1;
    char flag;
    struct point p1;
    struct vector v1;
    struct surfaces s1;
    struct half_line h1;
    struct color c1;
};


//测试用
void printVect(struct vector a){
    printf("-->%.14lf,%.14lf,%.14lf\n",a.x,a.y,a.z);
}
void printPoint(struct point a){
    printf("==>%.14lf,%.14lf,%.14lf\n",a.x,a.y,a.z);
}
void printMatrix3(struct matrix3 a){
    printf("--<%.14lf,%.14lf,%.14lf>\n",a.a,a.b,a.c);
    printf("--<%.14lf,%.14lf,%.14lf>\n",a.d,a.e,a.f);
    printf("--<%.14lf,%.14lf,%.14lf>\n",a.g,a.h,a.i);
}


//正常代码


struct vector getVectorFromPoint(struct point *a,struct point *b){
    struct vector result;
    result.x=b->x-a->x;
    result.y=b->y-a->y;
    result.z=b->z-a->z;
    return result;
}

struct color colorAdjust(struct color *a,double alpha,char baseColor){
    struct color result;
    if (baseColor==1){
        result.R=255-(char)(alpha*(255-a->R));
        result.G=255-(char)(alpha*(255-a->G));
        result.B=255-(char)(alpha*(255-a->B));
        
    }
    else{
        result.R=(char)(alpha*a->R);
        result.G=(char)(alpha*a->G);
        result.B=(char)(alpha*a->B);
    }
}//basecolor变化方式，0偏暗，1偏亮

struct color colorPlus(struct color *a,struct color *b){
    struct color result;
    result.R=a->R+b->R;
    result.G=a->G+b->G;
    result.B=a->B+b->B;
    return result;
}

double dotProduct(struct vector *a,struct vector *b){
    double result;
    result=a->x*b->x+a->y*b->y+a->z*b->z;
    return result;
}

struct vector crossProduct(struct vector *a,struct vector *b){
    struct vector result;
    result.x=a->y*b->z-a->z*b->y;
    result.y=a->z*b->x-a->x*b->z;
    result.z=a->x*b->y-a->y*b->x;
    return result;
}
double getVectorLenth(struct vector *a){
    int lenth;
    lenth = sqrt(a->x*a->x+a->y*a->y+a->z*a->z);
    return lenth;
}
void unitization(struct vector *a){
    struct vector b;
    a->x=a->x/getVectorLenth(a);
    a->y=a->y/getVectorLenth(a);
    a->z=a->z/getVectorLenth(a);
}

void setPoint(double x,double y,double z,struct point *a){
    a->x=x;
    a->y=y;
    a->z=z;
}
void setVector(double x,double y,double z,struct vector *a){
    a->x=x;
    a->y=y;
    a->z=z;
}

void copyVector(struct vector b,struct vector *a){
    a->x=b.x;
    a->y=b.y;
    a->z=b.z;
}

void setRay(double px,double py,double pz,double vx,double vy,double vz,struct half_line *a){
    setPoint(px,py,pz,&(a->start));
    setVector(vx,vy,vz,&(a->direct));
}

struct vector vectorAdjust(struct vector *a,struct vector *b,double f,double k,struct vector *c){
    struct vector result;
    result.x=f*(a->x)+k*(b->x)+c->x;
    result.y=f*(a->y)+k*(b->y)+c->y;
    result.z=f*(a->z)+k*(b->z)+c->z;
    return result;
}

struct point vector2point(struct vector *a){
    struct point c;
    c.x=a->x;
    c.y=a->y;
    c.z=a->z;
    return c;
}

void writePointIntoMatrix(struct matrix3 *a,struct point *b,char typeAndPosition){
    if (typeAndPosition==11){
        a->a=b->x;
        a->b=b->y;
        a->c=b->z;
    }
    if (typeAndPosition==12){
        a->d=b->x;
        a->e=b->y;
        a->f=b->z;
    }
    if (typeAndPosition==13){
        a->g=b->x;
        a->h=b->y;
        a->i=b->z;
    }
    if (typeAndPosition==21){
        a->a=b->x;
        a->d=b->y;
        a->g=b->z;
    }
    if (typeAndPosition==22){
        a->b=b->x;
        a->e=b->y;
        a->h=b->z;
    }
    if (typeAndPosition==23){
        a->c=b->x;
        a->f=b->y;
        a->i=b->z;
    }
}//写入位置分两部分，13代表写入第三行，23代表写入第三列，21代表写入第一列

//行列式
double determinant(struct matrix3 *a){
    double a1,a2,a3,a4,a5,a6,result;
    a1=(a->a)*(a->e)*(a->i);
    a2=(a->b)*(a->f)*(a->g);
    a3=(a->c)*(a->d)*(a->h);

    a4=(a->c)*(a->e)*(a->g);
    a5=(a->a)*(a->f)*(a->h);
    a6=(a->b)*(a->d)*(a->i);

    result=(a1+a2+a3)-(a4+a5+a6);
    return result;
}

//线面求交
struct point getPoint(struct half_line *a,struct surfaces *b){
    struct matrix3 s1,s2,s3,s4;
    struct vector f3,f2,f1 = {0,0,1};//待测试特殊情况
    struct point p1,p2,p3,p4,result;

    f3=getVectorFromPoint(&(b->edge_point[0]),&(b->edge_point[1]));
    f2=getVectorFromPoint(&(b->edge_point[1]),&(b->edge_point[2]));
    f3=crossProduct(&f3,&f2);
    f1=crossProduct(&(a->direct),&f1);
    if (getVectorLenth(&f1)==0) {
        f1.x=1;
        f1=crossProduct(&(a->direct),&f1);
    }
    f2=crossProduct(&(a->direct),&f1);


    p1=vector2point(&f1);
    p2=vector2point(&f2);
    p3=vector2point(&f3);
    writePointIntoMatrix(&s1,&p1,11);
    writePointIntoMatrix(&s1,&p2,12);
    writePointIntoMatrix(&s1,&p3,13);

    p4.x=f1.x*(a->start.x)+f1.y*(a->start.y)+f1.z*(a->start.z);
    p4.y=f2.x*(a->start.x)+f2.y*(a->start.y)+f2.z*(a->start.z);
    p4.z=f3.x*(b->edge_point[0].x)+f3.y*(b->edge_point[0].y)+f3.z*(b->edge_point[0].z);

    s2=s1;
    s3=s1;
    s4=s1;

    writePointIntoMatrix(&s2,&p4,21);
    writePointIntoMatrix(&s3,&p4,22);
    writePointIntoMatrix(&s4,&p4,23);

    result.x=determinant(&s2)/determinant(&s1);
    result.y=determinant(&s3)/determinant(&s1);
    result.z=determinant(&s4)/determinant(&s1);
    return result;
}

int isPointInSurface(struct surfaces *a,struct point *b){
    struct vector v1,v2,v3,v4,v5;
    double s1;
    v1=getVectorFromPoint(&(a->edge_point[1]),&(a->edge_point[0]));
    v2=getVectorFromPoint(&(a->edge_point[2]),&(a->edge_point[0]));
    v3=getVectorFromPoint(b,&(a->edge_point[0]));
    v4=crossProduct(&v1,&v3);
    v5=crossProduct(&v3,&v2);
    s1=dotProduct(&v4,&v5);
    if (s1>=0){
        v1=getVectorFromPoint(&(a->edge_point[2]),&(a->edge_point[1]));
        v2=getVectorFromPoint(&(a->edge_point[0]),&(a->edge_point[1]));
        v3=getVectorFromPoint(b,&(a->edge_point[1]));
        v4=crossProduct(&v1,&v3);
        v5=crossProduct(&v3,&v2);
        s1=dotProduct(&v4,&v5);
        if (s1>=0) return 1;
        else return 0;
    }
    else return 0;
}//在三角形内返回1，否则返回0

struct vector matrixMultiVector(struct matrix3 *m,struct vector *v){
    struct vector result;
    result.x=m->a*v->x+m->b*v->y+m->c*v->z;
    result.y=m->d*v->x+m->e*v->y+m->f*v->z;
    result.z=m->g*v->x+m->h*v->y+m->i*v->z;
    return result;
}

struct vector spinAround(struct vector *a,struct vector *b,double angle){
    double c,s,c1;
    struct matrix3 m;
    struct vector result;
    c=cos(angle);
    s=sin(angle);
    c1=1-c;
    m.a=c+c1*a->x*a->x;
    m.b=c1*a->x*a->y-s*a->z;
    m.c=c1*a->x*a->z+s*a->y;
    m.d=c1*a->x*a->y+s*a->z;
    m.e=c+c1*a->y*a->y;
    m.f=c1*a->y*a->z-s*a->x;
    m.g=c1*a->x*a->z-s*a->y;
    m.h=c1*a->y*a->z-s*a->x;
    m.i=c+c1*a->z*a->z;
    result = matrixMultiVector(&m,&result);
    return result;
}

struct vector FLIP(struct vector *a,struct vector *b){
    struct matrix3 m;
    struct vector result;
    result.x = -b->x;
    result.y = -b->y;
    result.z = -b->z;
    m.a=2*a->x*a->x-1;
    m.b=2*a->x*a->y;
    m.c=2*a->x*a->z;
    m.d=m.b;
    m.e=2*a->y*a->y-1;
    m.f=2*a->y*a->z;
    m.g=m.c;
    m.h=m.f;
    m.i=2*a->z*a->z-1;
    result = matrixMultiVector(&m,&result);
    return result;
}

struct half_line calReflect(struct vector *a,struct point b,struct vector *c){
    struct half_line result;
    result.start=b;
    result.direct = FLIP(a,c);
    return result;
}

double calSpinAngle(struct vector *a,struct vector *c,double n){
    double a1,a2,result;
    a1 = dotProduct(a,c)/(getVectorLenth(a)*getVectorLenth(c));
    a2 = a1/n;
    a1 = (double)asin(a1);
    a2 = (double)asin(a2);
    //这里计算角度
    return result;
}

struct half_line calRefraction(struct vector *a,struct point b,struct vector *c,double n){
    double angles;
    struct half_line result;
    angles = calSpinAngle(a,c,n);
    result.start = b;
    result.direct = spinAround(a,c,angles);//这里改成叉乘过后的向量
    return result;
}

unsigned int color2int(struct color a){
    unsigned int result=0;
    char r=a.R,g=a.G,b=a.B;
    asm (
        "mov %%bl,%%al;"
        "sal $8,%%eax;"
        "mov %%cl,%%al;"
        "sal $8,%%eax;"
        "mov %%dl,%%al;"
        "sal $8,%%eax;"
        :"=a"(result)
        :"b"(r),"c"(g),"d"(b)
    );
    return result;
}
