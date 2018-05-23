#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

int R,C=9,WrongInput=0,FillType=7;
double fps=0.02,M=1.0; //fps=SetRefreshRate>=0.015, M=(2.5>TimeMultiplier>0.0)

void CalcBounce(double *S1,double *S2)
{
    //*S1=FirstPositionOfARow(Xcoord),SoWeCanUsePointersArithmetic:
    //X=(*(S1+0));Y=(*(S1+1));Color(*(S1+2));Mass=(*(S1+3));Radius=(*(S1+4));Xspeed=(*(S1+5));Yspeed=(*(S1+6));XspeedTemp=(*(S1+7));YspeedTemp=(*(S1+8));
    if ((*(S1+4)+*(S2+4))>=(sqrt(pow((*S1-*S2),2)+pow((*(S1+1)-*(S2+1)),2))))   //SpheresTouchingIf R1+R2>=sqrt((x1-x2)^2+(y1-y2)^2).IfSumOfRadiuses>=VectorBetweenCirclesCenters
        {
        //SchoolPhysicsFormula
        *(S1+7)=((((2*(*(S2+3)))/((*(S1+3))+(*(S2+3))))*(*(S2+5)))+((((*(S1+3))-(*(S2+3)))/((*(S1+3))+(*(S2+3))))*(*(S1+5))));  //ChangingXSpeed
        *(S1+8)=((((2*(*(S2+3)))/((*(S1+3))+(*(S2+3))))*(*(S2+6)))+((((*(S1+3))-(*(S2+3)))/((*(S1+3))+(*(S2+3))))*(*(S1+6))));  //ChangingYSpeed
        }
}

void CalcBound(double *S1)
{
        if ((((*(S1+0))+(*(S1+4)))>=getmaxx())||((*(S1+0))<=(*(S1+4)))) //if (x+rad>=getmaxx() || x-rad<=0).If Xcoordinate>=RightBorderCoordinate Or Xcoordinate<=LeftBorderCoordinate
        *(S1+7)*=-1;    //sX*=-1;
        if ((((*(S1+1))+(*(S1+4)))>=getmaxy())||((*(S1+1))<=(*(S1+4))))
        *(S1+8)*=-1;
}

int main ()
{
   //Input
    FILE *File=fopen("input.txt","r");
    if (File == NULL)
    {
        fprintf(stderr,"Can't open input file\n");
        exit(1);
    }
    int i,j;
    fscanf(File,"%d",&R);       //GettingNumberOfBallsSeparatelyToIdentifyArraysAndCycles
    double *p=(double*)calloc(R*C,sizeof(double));       //CallocSuitsBetterBecauseWeDontWantToReadToAllCells
    for (i=0;i<R;i++)
    {
        for (j=0;j<C-2;j++)     //(C-2)BecauseInLastTwoPositionsWeHaveTemporaryValuesOfSpeeds
        {
            fscanf(File,"%lf",p+i*C+j);
        }
    }
    fclose(File);

    //CheckingInputValues
    for (i=0;i<R;i++)
    {
        if ((*(p+i*C+0)<(0+(*(p+i*C+4))))||(*(p+i*C+0)>600-(*(p+i*C+4))))    //CheckingX+WeCantSpawnSpheresOnBordersSoIChangedBasicParameter(OrTheyJustStuck),0+R<X<600-R
        {
            printf("Wrong input Xposition of the sphere #%d\n",i+1);     //(i+1)BecauseArraysStartsAt0 ;)
            WrongInput=1;
        }
        if ((*(p+i*C+1)<(0+(*(p+i*C+4))))||(*(p+i*C+1)>600-(*(p+i*C+4))))    //CheckingY+WeCantSpawnSpheresOnBordersSoIChangedBasicParameter(OrTheyJustStuck),0+R<=Y<=400-R
        {
            printf("Wrong input Yposition of the sphere #%d\n",i+1);
            WrongInput=1;
        }
        if ((*(p+i*C+2)<0)||(*(p+i*C+2)>15))     //CheckingColor
        {
            printf("Wrong input color of the sphere #%d\n",i+1);
            WrongInput=1;
        }
        if ((*(p+i*C+3)<0)||(*(p+i*C+3)>10000))  //CheckingMass
        {
            printf("Wrong input mass of the sphere #%d\n",i+1);
            WrongInput=1;
        }
        if ((*(p+i*C+4)<0)||(*(p+i*C+4)>100))    //CheckingRadius
        {
            printf("Wrong input radius of the sphere #%d\n",i+1);
            WrongInput=1;
        }
        if ((*(p+i*C+5)<-100)||(*(p+i*C+5)>100)) //CheckingSpeedX
        {
            printf("Wrong input Xspeed of the sphere #%d\n",i+1);
            WrongInput=1;
        }
        if ((*(p+i*C+6)<-100)&&(*(p+i*C+6)>100)) //CheckingSpeedY
        {
            printf("Wrong input Yspeed of the sphere #%d\n",i+1);
            WrongInput=1;
        }
    }

    //CheckingIfSpheresCollideOnSpawn(SpawnOneInsideOther)
    for (i=0;i<R;i++)
    {
        for (j=0;j<R;j++)
        {
            if ((i!=j)&&((*(p+i*C+4)+(*(p+j*C+4)))>=(sqrt(pow(((*(p+i*C+0))-(*(p+j*C+0))),2)+pow(((*(p+i*C+1))-(*(p+j*C+1))),2)))))      ////GivingPointersToFirstValuesOfTwoSpheresRows(Xcoordinates)
            {
                printf("Wrong starting position of the spheres #%d and #%d, they collide on spawn\n",i+1,j+1);
                WrongInput=1;
            }
        }
    }

    //CheckingInputs
    if (WrongInput) exit(1);
    if (FillType<0||FillType>11)    //ThereAreOnly12Types
    {
        printf("You gave wrong fill pattern");
        exit(1);
    }

    //GraphicInitialisation
    int gdriver = DETECT, gmode, errorcode;
    initgraph(&gdriver, &gmode, "");
    errorcode = graphresult();
    if (errorcode != grOk)
    {
      printf("Graphics error: %s\n", grapherrormsg(errorcode));
      system ("pause");
      exit(1);
    }

    //SpheresLogic
    clock_t finish,previous;
    float step;
    previous=clock();

    do
    {
        finish = clock();
        step = (finish-previous)*1.0/CLOCKS_PER_SEC;
        if (step >= fps)   //CheckingTimeChange
        {
            previous=finish;
            //Drawing
            for (i=0;i<R;i++)
            {
                //CoveringPreviousSphere
                setfillstyle(FillType,0);     //0=Black
                setcolor(0);    //WeMustHaveCircleBorderWithSBlackColor
                fillellipse(*(p+i*C+0),*(p+i*C+1),*(p+i*C+4),*(p+i*C+4));   //fillellipse (x,y,r,r)
                //PositionChange
                (*(p+i*C+0))+=((*(p+i*C+5))*M*step);     //(x)+=(speedX)*step
                (*(p+i*C+1))+=((*(p+i*C+6))*M*step);     //(y)+=(speedY)*step
            }
            //Boundery/Bounce_Calculations
            for (i=0;i<R;i++)
            {
                *(p+i*C+7)=*(p+i*C+5);    //SettingTempSpeedX_SavingOriginalValue
                *(p+i*C+8)=*(p+i*C+6);    //SettingTempSpeedY_SavingOriginalValue
                for (j=0;j<R;j++)
                    {
                    if (i!=j) CalcBounce((p+i*C),(p+j*C));      //GivingPointersToFirstValuesOfTwoSpheresRows(Xcoordinates)/(i!=j)SoThatIWillNotCheckBallWithItself
                    }
                CalcBound((p+i*C));     //GivingPointerToFirstValueOfSpheresRow(Xcoordinate)
            }
            //SettingSpeed
            for (i=0;i<R;i++)
            {
                *(p+i*C+5)=*(p+i*C+7);    //SettingSpeedX
                *(p+i*C+6)=*(p+i*C+8);    //SettingSpeedY
            }
            //Drawing
            for (i=0;i<R;i++)
            {
                //DrawingNew
                setfillstyle(FillType,*(p+i*C+2)); //setfillpattern("LINE_FILL",COLOR)
                setcolor(*(p+i*C+2));   //WeMustHaveCirclesBorderWithSameColor
                fillellipse(*(p+i*C+0),*(p+i*C+1),*(p+i*C+4),*(p+i*C+4));   //fillellipse (x,y,r,r)
            }
        }
    }
    while (!kbhit());   //WhileNoInput
    closegraph();
    free(p);
    return 1;
}
