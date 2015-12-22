#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>

int min(int a, int b, int c);
int Rgb2Hsi(const IplImage* src, IplImage* dataH, IplImage* dataS, IplImage* dataI);
int main(int argc, char ** argv)
{
    IplImage * Img = cvLoadImage(argv[1], 1);
    IplImage * HImg = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);
    IplImage * SImg = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);
    IplImage * IImg = cvCreateImage(cvGetSize(Img), IPL_DEPTH_8U, 1);    

    if( Rgb2Hsi(Img, HImg, SImg, IImg) == 0)
    {
        printf("Convert Error!\n");
        exit(-1);
    }
    
    //高斯滤波先，以平滑图像

    cvSmooth(SImg, SImg, CV_GAUSSIAN, 3, 0, 0, 0);
//    cvThreshold(SImg, SImg, 100, 255, CV_THRESH_BINARY);

    
    IplImage *pImgCanny = cvCreateImage(cvGetSize(SImg), IPL_DEPTH_8U, 1);
    cvCanny(SImg, pImgCanny, 50, 150, 3);
    
    cvNamedWindow("Img", 1);    
    cvNamedWindow("HImg", 1);
    cvNamedWindow("SImg", 1);
    cvNamedWindow("IImg", 1);

    cvShowImage("Img", Img);
    cvShowImage("HImg", HImg);
    cvShowImage("SImg", SImg);
    cvShowImage("IImg", IImg);

    cvWaitKey(0);
    return 0;
}


int Rgb2Hsi(const IplImage* src, IplImage* dataH, IplImage* dataS, IplImage* dataI)
{
    //返回0：成功，1：失败

    //输入为彩色图像src, 输出dataH,dataS,dataI,分别为HSI模型的H、S、I分量

    int y,x,offsetSrc,offsetHsi;
    int step,channels,step1;
    int r,g,b,minRgb;
    double cosThita,num,den,S,H;
    uchar *dataImg;
    uchar *dataS1;
    uchar *dataI1;
    uchar *dataH1;


    channels = src->nChannels;
    dataImg = (uchar *)src->imageData;

    dataI1 = (uchar *)dataI->imageData;
    dataH1=(uchar *)dataH->imageData;
    dataS1=(uchar *)dataS->imageData;


    if (channels!=3)
    {
        printf("The input image is not 3-channels!\n");
        return 0;
    }

    step=src->widthStep;
    step1=dataI->widthStep;

    for (y=0; y<src->height; y++)
     for (x=0; x<src->width; x++)
        {
            offsetSrc = y*step + x*channels;
            offsetHsi=y*step1+x;

            b = dataImg[offsetSrc];
            g=dataImg[offsetSrc+1];
            r=dataImg[offsetSrc+2];

            dataI1[offsetHsi] = (int)((r+g+b)/3+0.5);
            num=(2*r-g-b)/2;
            den=sqrt((r-g)*(r-g)+(r-b)*(g-b));
            if (den==0)
                den=0.01;
            cosThita=acos(num/den);
            minRgb = min(r, g, b);

            den=r+g+b;
            if (den==0)
                den=0.01;
            S=1-3*minRgb/den;

            dataS1[offsetHsi]=(int) (S*255+0.5);//将S分量和H分量都扩充到[0,255]区间以便于显示

                                     //一般H分量在[0,2pi]之间，S在[0,1]之间

    
            if (b<=g)
                H=cosThita/(2*3.14);
            else
                H=(2*3.14-cosThita)/(2*3.14);
            dataH1[offsetHsi] = (int)(H*255+0.5);
            if (S==0)
                dataH1[offsetHsi]=0;

        }


    return 1;

}

int min(int a, int b, int c)
{
    int m = a;
    if(m > b)
        m = b;
    if( m > c)
        m = c;

    return m;
}
