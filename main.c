//#include<SDL2\SDL.h>
#include<stdio.h>
#include"matrixlib.c"
#include"filelib.c"
#include<math.h>


#define windowWidth 640
#define windowHeight 480

#define sx 0
#define sy 0
#define sz 0

#define modelSurfaceNumber 100
#define pointLightNumber 10

#define maxTraceTimes 5
#define maxRange 10000000

#define bufferAreaStack defaultBufferArea[0]

#define modelPath "D:\\CODES\\C\\graphic\\model.txt"
#define lightPath "D:\\CODES\\C\\graphic\\light.txt"



//SDL_Window* window = NULL;//窗口的指针
//SDL_Surface* surface = NULL;//窗口表面的指针
//Uint32* pixels = NULL;
struct half_line camaraRay[windowHeight*windowWidth];
struct surfaces modelsurface[modelSurfaceNumber];
struct pointLight pLight[pointLightNumber];
struct bufferArea defaultBufferArea[maxTraceTimes+1];
struct color backgroundColor={100,100,100};
unsigned int traceTimes=0;
FILE *fp = NULL;

/*

//底层库部分
void initVideo(){
    if (SDL_Init(SDL_INIT_VIDEO)!=0){
        printf("图像初始化错误！%s",SDL_GetError());
    }
    window = SDL_CreateWindow("我的第一个窗口",
        SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
        windowWidth,windowHeight,
        SDL_WINDOW_SHOWN);
    if (!window){
        printf("窗口创建错误！%s",SDL_GetError());
    }
}

void initSurface(){
    surface = SDL_GetWindowSurface(window);
    if (!surface){
        printf("获取平面失败！%s",SDL_GetError());
    }
}

void refresh(){
    SDL_UpdateWindowSurface(window);
}

void release(){
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
}

unsigned int calculateposition(int x,int y){
    int position;
    position=windowWidth*y+x;
    if (x<windowWidth&&y<windowHeight){
        return position;
    }
    else return 0;
}

*/

//渲染部分

void pushBufferArea(){
    int i;
    traceTimes++;
    for (i=maxTraceTimes;i>0;i--){
        defaultBufferArea[i]=defaultBufferArea[i-1];
    }
}
void popBufferArea(){
    int i;
    traceTimes--;
    for (i=0;i<maxTraceTimes;i++){
        defaultBufferArea[i]=defaultBufferArea[i+1];
    }
}

void setSurfaceOranting(struct camara *a){
    struct vector c,b={0,0,1};
    a->Yvect=crossProduct(&(a->directAndDepth),&b);
    if (getVectorLenth(&(a->Yvect))==0) a->Yvect.y=1;
    unitization(&(a->Yvect));
    a->Xvect=crossProduct(&(a->directAndDepth),&(a->Yvect));
    unitization(&(a->Xvect));
}

//相机初始化
void InitCamera(struct camara *a){
    setPoint(sx,sy,sz,&(a->position));
    setVector(1,0,0,&(a->directAndDepth));
    setSurfaceOranting(a);
}
//平面射线设置
void setCamaraRay(struct camara *m){
    long long int i,j,k;
    struct vector middle;
    printf("设置相机逆光路\n");
    for (i=0;i<windowHeight;i++){
        for (j=0;j<windowWidth;j++){
            k=i*windowWidth+j;
            setPoint(sx,sy,sz,&(camaraRay[k].start));
            middle=vectorAdjust(&(m->Xvect),&(m->Yvect),(double)(i-windowHeight/2),(double)(j-windowWidth/2),&(m->directAndDepth));
            copyVector(middle,&(camaraRay[k].direct));
            printf("\r%d/%d",windowHeight*windowWidth,k+1);
        }
    }
    printf("--DONE!\n");
}
//逐面检测


struct bufferArea surfaceDetect(){
    long long int i;
    double bufferRange=bufferAreaStack.d1;
    struct point p;
    struct vector k;
    struct bufferArea b;
    for (i=0;i<modelSurfaceNumber;i++){
        p=getPoint(&(bufferAreaStack.h1),&(modelsurface[i]));//取点
        k=getVectorFromPoint(&(defaultCamara.position),&p);
        if (dotProduct(&k,&(bufferAreaStack.v1))>=0&&bufferRange>=getVectorLenth(&k)&&isPointInSurface(&(bufferAreaStack.s1),&p)){//正向？先于某平面？在平面内？
            bufferAreaStack.s1=modelsurface[i];
            bufferAreaStack.p1=p;
            bufferAreaStack.d1=bufferRange;
            bufferAreaStack.flag=1;
        } 
        else bufferAreaStack.flag=0;
    }
    return b;
}//待测试

struct color getColor(struct half_line a);
//三种光路
//漫反射
struct color diffusionReflect(){
    pushBufferArea();
}
//全反射
struct color totalReflect(){
    struct half_line line = bufferAreaStack.h1;
    struct point sur = bufferAreaStack.p1;
    struct color result = backgroundColor; 
    struct vector v = bufferAreaStack.v1;
    pushBufferArea();
    if (traceTimes<maxTraceTimes) {
        result = getColor(calReflect(&(line.direct),sur,&v));
    }
    popBufferArea();
    return result;
}
//折射
struct color refraction(){
    struct half_line line = bufferAreaStack.h1;
    struct point pt = bufferAreaStack.p1;
    struct color result = backgroundColor; 
    struct vector v = bufferAreaStack.v1;
    struct surfaces sur = bufferAreaStack.s1;
    pushBufferArea();
    if (traceTimes<maxTraceTimes) {
         result = getColor(calRefraction(&(line.direct),pt,&v,sur.n));
    }
    popBufferArea();
    return result;
}
//递归主函数
struct color getColor(struct half_line a){
    struct color result,ref1,ref2,dif;
    bufferAreaStack.h1=a;
    bufferAreaStack.d1=maxRange;
    bufferAreaStack.v1=a.direct;
    surfaceDetect();
    if (bufferAreaStack.flag==1){
        ref1 = totalReflect();
        ref1 = colorAdjust(&ref1,bufferAreaStack.s1.reflect,0);
        ref2 = refraction();
        ref2 = colorAdjust(&ref2,bufferAreaStack.s1.alpha,0);
        dif = diffusionReflect();
        result = colorPlus(&ref1,&ref2);
        result = colorPlus(&result,&dif);
    }
    else result=backgroundColor;
    return result;
}


//填入像素的函数
void putPixels(){
    long long int i,j,k;
    int color;
    for (i=0;i<windowHeight;i++){
        for (j=0;j<windowWidth;j++){
            k=i*windowWidth+j;
            color=color2int(getColor(camaraRay[k]));
            printf("\r%d/%d--color=%d",windowHeight*windowWidth,k+1,color);
        }
    }
    printf("--DONE!\n");
}

void PRODUCT(){
    InitCamera(&defaultCamara);
    printf("设置相机朝向\n");
    printVect(defaultCamara.directAndDepth);
    printVect(defaultCamara.Yvect);
    printVect(defaultCamara.Xvect);
    setCamaraRay(&defaultCamara);
    putPixels();
}

void loadModel(){
    int i;
    printf("载入模型\n");
    fp = fopen(modelPath,"r");
    if (fp == NULL){
        printf("\n模型文件错误！\n");
    }
    for (i=0;i<modelSurfaceNumber;i++){
        fscanf(fp,"%lf %lf %lf",&(modelsurface[i].edge_point[0].x),&(modelsurface[i].edge_point[0].y),&(modelsurface[i].edge_point[0].z));
        fscanf(fp,"%lf %lf %lf",&(modelsurface[i].edge_point[1].x),&(modelsurface[i].edge_point[1].y),&(modelsurface[i].edge_point[1].z));
        fscanf(fp,"%lf %lf %lf",&(modelsurface[i].edge_point[2].x),&(modelsurface[i].edge_point[2].y),&(modelsurface[i].edge_point[2].z));
        fscanf(fp,"%d",&modelsurface[i].alpha);
        fscanf(fp,"%d",&modelsurface[i].reflect);
        fscanf(fp,"%lf",&modelsurface[i].n);
        fseek(fp,3,1);
        printf("\r%d/%d",i,modelSurfaceNumber);
    }
    printf("\r---DONE!\n");
    fclose(fp);
}

void loadLight(){
    int i;
    printf("载入灯光\n");
    fp = fopen(lightPath,"r");
    if (fp == NULL){
        printf("\n灯光文件错误！\n");
    }
    for (i=0;i<pointLightNumber;i++){
        fscanf(fp,"%lf %lf %lf",&(pLight[i].sourcePoint.x),&(pLight[i].sourcePoint.y),&(pLight[i].sourcePoint.z));
        fscanf(fp,"%d",&pLight[i].RGB.R);
        fscanf(fp,"%d",&pLight[i].RGB.G);
        fscanf(fp,"%d",&pLight[i].RGB.B);
        fseek(fp,3,1);
        printf("\r%d/%d",i,pointLightNumber);
    }
    printf("\r---DONE!\n");
    fclose(fp);
}

void LOAD(){
    loadModel();
    loadLight();
}

int main(int argc, char *argv[]){
    LOAD();
    PRODUCT();
}





/*
int main(int argc, char *argv[]){
    initVideo();
    initSurface();
    pixels = surface->pixels;
    PRODUCT();
    refresh();
    SDL_Delay(5000);
    release();
    return 0;
}
*/
