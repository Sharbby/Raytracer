//#include<SDL2\SDL.h>
#include<stdio.h>
#include"MatrixLibrary.c"
#include<math.h>


#define windowWidth 640
#define windowHeight 480

#define sx 0
#define sy 0
#define sz 0

#define ox 5
#define oy 0
#define oz 0

#define modelSurfaceNumber 100
#define pointLightNumber 10

#define maxTraceTimes 5
#define maxRange 10000000

#define bufferAreaStack defaultBufferArea[0]

#define modelPath "D:\\CODES\\C\\graphic\\models.txt"
#define lightPath "D:\\CODES\\C\\graphic\\light.txt"



//SDL_Window* window = NULL;//窗口的指针
//SDL_Surface* surface = NULL;//窗口表面的指针
//Uint32* pixels = NULL;

Ray camaraRay[windowHeight*windowWidth];
ModelSurface modelsurface[modelSurfaceNumber];
LightSource pLight[pointLightNumber];
bufferArea defaultBufferArea[maxTraceTimes+1];
Colorchar backgroundColor={0,200,0};
Camara defaultCamara;
unsigned int traceTimes=0;
FILE *fp = NULL;

unsigned int calculateposition(int x,int y){
    int position;
    position=windowWidth*y+x;
    if (x<windowWidth&&y<windowHeight){
        return position;
    }
    else return 0;
}



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
void clearBufferArea(){
    unsigned int i;
    double d1=0;
    char flag=0;
    Point p1={0,0,0};
    Vector v1={0,0,0};
    Colorchar c1={0,0,0};
    ModelSurface s1={p1,p1,p1,c1,0,0,0};
    Ray h1 = {p1,v1};
    for (i=0;i<maxTraceTimes;i++){
        defaultBufferArea[i].d1=0;
        defaultBufferArea[i].flag = 0;
        defaultBufferArea[i].p1 = p1;
        defaultBufferArea[i].v1 = v1;
        defaultBufferArea[i].s1 = s1;
        defaultBufferArea[i].h1 = h1;
        defaultBufferArea[i].c1 = c1;
    }
}

void setSurfaceOranting(struct camara *a){
    Vector c,b={0,0,1};
    a->Yvect=crossProduct(a->directAndDepth,b);
    if (getVectorLenth(a->Yvect)==0) a->Yvect.Y=1;
    a->Yvect = unitization(a->Yvect);
    a->Xvect = crossProduct(a->directAndDepth,a->Yvect);
    a->Xvect = unitization(a->Xvect);
}

//相机初始化
void InitCamera(struct camara *a){
    a->position = setPoint(sx,sy,sz);
    a->directAndDepth = setVector(ox,oy,oz);
    setSurfaceOranting(a);
}
//平面射线设置
void setCamaraRay(struct camara *m){
    long long int i,j,k;
    Vector middle;
    printf("设置相机逆光路\n");
    for (i=0;i<windowHeight;i++){
        for (j=0;j<windowWidth;j++){
            k=i*windowWidth+j;
            camaraRay[k].start = setPoint(sx,sy,sz);
            middle=vectorAdjust(m->Xvect,m->Yvect,(double)(i-windowHeight/2),(double)(j-windowWidth/2),m->directAndDepth);
            camaraRay[k].direct=middle;
            //printf("\r%d/%d",windowHeight*windowWidth,k+1);
        }
    }
    printf("--DONE!\n");
}
//逐面检测


void surfaceDetect(){
    long long int i;
    Point p;
    Vector k;
    bufferAreaStack.flag = 0;
    bufferAreaStack.d1 = maxRange;
    for (i=0;i<modelSurfaceNumber;i++){
        if (dotProduct(getNormalVector(modelsurface[i]),bufferAreaStack.h1.direct)!=0)//平行？
        {
            p=getPoint(bufferAreaStack.h1,modelsurface[i]);//取点
            k=getVectorFromPoint(defaultCamara.position,p);
            if (dotProduct(k,bufferAreaStack.v1)>=0)//正向？
                if (bufferAreaStack.d1>=getVectorLenth(k))//最近？
                    if (isPointInSurface(modelsurface[i],p))//平面内？
                    {
                        bufferAreaStack.s1=modelsurface[i];
                        bufferAreaStack.p1=p;
                        bufferAreaStack.d1=getVectorLenth(k);
                        bufferAreaStack.flag=1;
                    } 
        }
    }
}//待测试

Colorchar getColor(Ray a);
//三种光路
//漫反射
Colorchar diffusionReflect(){
    int i;
    double plightLenth;
    Point Origin={0,0,0};
    Vector lightVector;
    Colorchar colorBuffer,result = backgroundColor;
    Point locatePoint = bufferAreaStack.p1;
    if (traceTimes < maxTraceTimes) {
        pushBufferArea();
        for (i=0;i<pointLightNumber;i++){
            lightVector = getVectorFromPoint(Origin,pLight[i].start);
            bufferAreaStack.h1.start = locatePoint;
            bufferAreaStack.h1.direct = getVectorFromPoint(locatePoint,pLight[i].start);
            surfaceDetect();
            if (bufferAreaStack.flag = 0){
                colorBuffer=pLight[i].color;
                colorBuffer = colorAdjust(colorBuffer,getVectorLenth(lightVector),backgroundColor);
                result = colorPlus(result,colorBuffer);
            }
            else if (getVectorLenth(lightVector)<=bufferAreaStack.d1){
                colorBuffer=pLight[i].color;
                colorBuffer = colorAdjust(colorBuffer,getVectorLenth(lightVector),backgroundColor);
                result = colorPlus(result,colorBuffer);
            }
        }
        popBufferArea();
        result = colorFilt(bufferAreaStack.s1.color,result);
    }
    return result;
}
//全反射
Colorchar totalReflect(){
    Ray line = bufferAreaStack.h1;
    Point sur = bufferAreaStack.p1;
    Colorchar result = backgroundColor; 
    Vector v = bufferAreaStack.v1;
    if (traceTimes < maxTraceTimes) {
        pushBufferArea();
        if (traceTimes<maxTraceTimes) {
            result = getColor(calReflect(line.direct,sur,v));
        }
        popBufferArea();
    }
    return result;
}
//折射
Colorchar refraction(){
    Ray line = bufferAreaStack.h1;
    Point pt = bufferAreaStack.p1;
    Colorchar result = backgroundColor; 
    Vector v = bufferAreaStack.v1;
    ModelSurface sur = bufferAreaStack.s1;
    if (traceTimes < maxTraceTimes) {
        pushBufferArea();
        if (traceTimes<maxTraceTimes) {
            result = getColor(calRefraction(line.direct,pt,v,sur.n));
        }
        popBufferArea();
    }
    return result;
}
//递归主函数
Colorchar getColor(Ray a){
    Colorchar result,ref1,ref2;
    clearBufferArea();
    bufferAreaStack.h1=a;
    bufferAreaStack.d1=maxRange;
    bufferAreaStack.v1=a.direct;
    surfaceDetect();
    if (bufferAreaStack.flag==1){
        result = diffusionReflect();
        if (bufferAreaStack.s1.reflect!=0){
            ref1 = totalReflect();
            result = colorAdjust(ref1,bufferAreaStack.s1.reflect,result);
        }
        if (bufferAreaStack.s1.alpha!=0){
            ref2 = refraction();
            result = colorAdjust(ref2,bufferAreaStack.s1.alpha,result);
        }
    }
    else result=backgroundColor;
    return result;
}


//填入像素的函数

void putPixels(){
    long long int i,j,k;
    Colorchar color;
        //Uint32 buffer;
    for (i=0;i<windowHeight;i++){
        for (j=0;j<windowWidth;j++){
            k=i*windowWidth+j;
            color=getColor(camaraRay[k]);
        //buffer = SDL_MapRGBA(surface->format,color.R,color.G,color.B,255);
        //pixels[k] = buffer;
            //if (color.R != backgroundColor.R&&color.G != backgroundColor.G&&color.B != backgroundColor.B) 
                printf("%d/%d--color=%d,%d,%d\n",windowHeight*windowWidth,k+1,color.R,color.G,color.B);
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
    
}

void loadModel(){
    int i;
    double bx,by,bz;
    unsigned char r,g,b;
    printf("载入模型\n");
    fp = fopen(modelPath,"r");
    if (fp == NULL){
        printf("\n模型文件错误！\n");
    }
    for (i=0;i<modelSurfaceNumber;i++){
        fscanf(fp,"%lf %lf %lf",&bx,&by,&bz);
        modelsurface[i].edge[0].x = bx;
        modelsurface[i].edge[0].y = by;
        modelsurface[i].edge[0].z = bz;
        fscanf(fp,"%lf %lf %lf",&bx,&by,&bz);
        modelsurface[i].edge[1].x = bx;
        modelsurface[i].edge[1].y = by;
        modelsurface[i].edge[1].z = bz;
        fscanf(fp,"%lf %lf %lf",&bx,&by,&bz);
        modelsurface[i].edge[2].x = bx;
        modelsurface[i].edge[2].y = by;
        modelsurface[i].edge[2].z = bz;
        fscanf(fp,"%lf %lf %lf",&bx,&by,&bz);
        modelsurface[i].color.R = (char)bx;
        modelsurface[i].color.G = (char)by;
        modelsurface[i].color.B = (char)bz;
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
        fscanf(fp,"%lf %lf %lf",&(pLight[i].start.x),&(pLight[i].start.y),&(pLight[i].start.z));
        fscanf(fp,"%d",&pLight[i].color.R);
        fscanf(fp,"%d",&pLight[i].color.G);
        fscanf(fp,"%d",&pLight[i].color.B);
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
    putPixels();
}




/*
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

int main(int argc, char *argv[]){
    LOAD();
    PRODUCT();
    initVideo();
    initSurface();
    pixels = surface->pixels;
    putPixels();
    SDL_UpdateWindowSurface(window);
    SDL_Delay(5000);
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    return 0;
}
*/