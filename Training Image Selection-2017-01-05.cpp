// Training Image Selection.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include <ctime>
#include "omp.h"
#include <afx.h>
#include <windows.h>
#include <iostream>
#include <afx.h>
#include <io.h>
#include <fstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace std; 


// Grid data structure
struct GridV
{
  int discprop;    // ģ�������е���ɢ����
  double contprop;     //ģ�������е���������
  int iscddata;   // �Ƿ���������
};

//Structure of random path
struct RandomRoutine
{
  int rpx;
  int rpy;
  int rpz;
};

//Structure of geological patterns
struct GeoPattern
{
	int px;
	int py;
	int pz;
	int facies;
	double ContVariable;   //��������
};

//Matched point in the TI
struct MatchedPoints
{
	int MPx;
	int MPy;
	int MPz;
};

//-----------------------------------------------------------------
//                Parameters Definition
//-----------------------------------------------------------------
// �����ò���
   int nED_0total = 0;  //ED=0�������¼�����
// ��������������
   int DistanceType=0;    //ƥ���(�����¼��ľ���)���㷽ʽ��0-��ģʽ��1-�����ͺ���Ȩ[cate]��2-ŷ�Ͼ���[cont]������һ�в������ݱ���ѡ�����
// Grid definition
   int Nx,Ny,Nz;      //number of nodes
   CString Datafl=" ";    //conditional data file path
   int NTI=0;    //ѵ��ͼ�������
   int TIType=0;    //ѵ��ͼ������
// CString *TrainingImageFl;  //Training Image file path
   string *TrainingImageFl;  //Training Image file path   
   string *TemplateFl;  //Training Image file path   
// Training Image size
   int TINx,TINy,TINz;     //��һѵ��ͼ��Ĺ�ģ������ȫ��
   int *TIscalex,*TIscaley,*TIscalez;    //��ȡ�����ж��ѵ��ͼ��Ĺ�ģ�����ڴ洢
   int *NTemplateNode;  //ÿ��tenplate�Ľڵ���
   int ***TemplateNode;   //template�ڵ�����
// Allocatable Arrays
   GridV ***GridValue;      //ģ�������������
   int ****Replication;      //ģ���ظ�������
   double *****MatchingDegree;    //���ڱ���ȶԵõ��Ķ���߶ȵ�ƥ�������
   int ***TIValue;    //��ɢ����άѵ��ͼ��̬����
   double ***ContTIValue;    //�����ͱ���ѵ��ͼ��̬����
// data patterns definition
   int MaxNode=100;   //���MaxNode�������ģʽ���������������������ɵ��������ݵ������
//   int PatternNode;  //Number of nodes within a pattern��assigned by user
// User preference settings
   int CheckPf=1;   //check Parameter file while reading 
   int seed=69069;  // random seed(to read from the parameter file)
//   int searchwindowx=1,searchwindowy=1,searchwindowz=1;  //�������ڵĴ�С,��ʼ����Ϊ10*10*1��
   GeoPattern *GP;  //GeoPattern
   int MinCDNum=0;    //ÿ������ģʽ����С���������ݵ������ﲻ�������Ŀ������������ģʽ��Ѱ�������ķ�ʽ���
   int NCD=0;  //���������������ݵ�ĸ�����
// Random routine structure array
   RandomRoutine *RR; 
   double MinD=0;  //��ѵ��ͼ���н���ģʽ����ʱ���ɽ���ƥ�������Ҫ�����С�������ݵ�������
   double MinMatchingDegree=0.0;  //ģ����Ҫ����Сƥ��ȣ�GP���ȡ�������¼���
   MatchedPoints mp;   //ÿ��ģ���У�ѡ�е������¼���TI�е�λ��
   int Ncut=0;   //��ɢ������������
   int match=0;   //����Ѱ�����¼�ʱ���Ƿ��ҵ�ƥ��������¼�
   int DisplayDetails=0;   //�Ƿ���ʾģ������еľ���ϸ�ڣ�1-��ʾ��0-����ʾ��
   int DisplaySearch_Match = 0;   //�Ƿ���ʾģ������еĵ���ģʽ��������ƥ����Ϣ��1-��ʾ��0-����ʾ��
   int NLevel=1;    //�����¼�ƥ��ּ�������Ĭ��Ϊ1��
   int **searchwindowscale;   //���ڶ�߶��������������ڴ�С
   int iLevel=0;    //ָʾ��ǰ���ڽ��м���ĳ߶ȱ�ţ�ȫ�ֱ�����Ĭ��Ϊ0������С�߶ȡ�
   CString MatchingResultfl;    //ƥ��ȶԽ������ļ�·��
   int GPmatching=0;    //��ǰ�Ƿ����GP��ƥ��-ȡ����GP�Ƿ񱻴�����0-��1-��
   int iTI=0;   //��ѵ��ͼ���ѭ��,ȫ�ֱ���
   int DisplayException=0;    //��ʾƥ������е��쳣GP
   double CalRatio=0.0;   //�������ı���
//  int **ND;  //��ȡ�����������¼�����,���ڼ���ƽ����ȡ�����ݵ���
//  double **NDAVG;  //��ȡ�����������¼�����,���ڼ���ƽ����ȡ�����ݵ���ƽ��ֵ
   int ConsideredCellNum=0;   //���������������
   int ReadPath=0;   //�Ƿ��ȡ���·��
   int NTIData=0;    //����ѵ��ͼ���ȡ
   int **NumInvalidGP;  //ÿ��ѵ��ͼ���У���ͬ�������������£��޷���ȡ��Ч�ĵ���ģʽ�ĵ���
   int **NumNotEnoughNodesinTI;   //��ÿ��ѵ��ͼ���У���ͬ�������������£��޷���ѵ��ͼ���л�ȡ�����㹻�������������¼����������
   int **NumNotEnoughNodesinTIGP;  //��ÿ��ѵ��ͼ���У���ͬ�������������£���ȡ���㹻�������㣬�����ڲ���Ҫ����������(NCD/2)
   int NumCDGP = 0;  //���ڼ���������ݵ㴦�Ƿ��д���GP
   int TemplateType = 0;  //������������ template type

//-----------------------------------------------------------------
//               ������ʾ�����ȴ�5��
//-----------------------------------------------------------------
void PlayEmergencySound(int times)
{
	for (int i = 0; i < times; i++)
   {
	   PlaySound(TEXT("BEEP1.WAV"), NULL, SND_FILENAME);
   }
   Sleep(5000);
}

//-----------------------------------------------------------------
//                   ������ʾ��
//-----------------------------------------------------------------
void PlayTipsSound(int times)
{
	for (int i = 0; i < times; i++)
	{
		PlaySound(TEXT("Alarm01.wav"), NULL, SND_FILENAME);
	}
	//Sleep(1000);
}

//-----------------------------------------------------------------
//               Input Tips Messages
//-----------------------------------------------------------------
void GreenTipsMsg(CString InputTipsMsg)
{
    //Get the color set handle
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);  
	//Set the color
    SetConsoleTextAttribute(hOut,  
                            BACKGROUND_GREEN |
							BACKGROUND_INTENSITY);  
    cout <<InputTipsMsg;//<<endl; 
	//set the color back
	SetConsoleTextAttribute(hOut,  
                            FOREGROUND_GREEN |  
                            FOREGROUND_INTENSITY); 
	cout<<endl;
}

//-----------------------------------------------------------------
//              allocate MatchingDegree arrays
//-----------------------------------------------------------------
void AllocateMatchingDegreeArray()
{
	//ѭ������ 
	int i,j,k,m,n;
	//allocate arrays
	MatchingDegree=new double ****[NTI];
	for(n=0;n<NTI;n++)
	{
		MatchingDegree[n]=new double***[NLevel];    //��Բ�ͬ����ظ�����������E-type Map
		for(i=0;i<NLevel;i++)
		{
			MatchingDegree[n][i]=new double**[Nz];
			for(j=0;j<Nz;j++) 
			{
				MatchingDegree[n][i][j]=new double*[Ny];
				for(m=0;m<Ny;m++)
				{
					MatchingDegree[n][i][j][m]=new double[Nx];
				}
			}
		}
	}
	
	for(n=0;n<NTI;n++)
		for (m=0;m<NLevel;m++)
			for(i=0;i<Nz;i++)
				for(j=0;j<Ny;j++)
					for(k=0;k<Nx;k++)
					{
						MatchingDegree[n][m][i][j][k]=-999.00;
					}

	GreenTipsMsg("MatchingDegree array(4D) was initialed");
	system("pause");
}
//-----------------------------------------------------------------
//              allocate GridValue arrays
//-----------------------------------------------------------------
void AllocateGridValue()
{
    //ѭ������ 
    int i,j,k,m;
	//allocate arrays
	GridValue=new GridV**[Nz];     //����ֵ�������������ݺ�ģ��ֵ
	Replication=new int***[Nz];    //��Բ�ͬ����ظ�����������E-type Map
		for(i=0;i<Nz;i++)
			{
				GridValue[i]=new GridV*[Ny];
				Replication[i]=new int **[Ny];
  				for(j=0;j<Ny;j++) 
					{
						GridValue[i][j]=new GridV[Nx];
						Replication[i][j]=new int*[Nx];
						for (m=0;m<Nx;m++)
						{
							Replication[i][j][m]=new int[Ncut];
						}
					}
			};

		for(i=0;i<Nz;i++)
			for(j=0;j<Ny;j++)
				for(k=0;k<Nx;k++)
				{
					GridValue[i][j][k].discprop=-999;
					GridValue[i][j][k].contprop=-999.00;
					GridValue[i][j][k].iscddata=-1;    //��ʼ��Ϊ����������
					for (m=0;m<Ncut;m++)
					{
						Replication[i][j][k][m]=0;
					}
				}
	GreenTipsMsg("GridValue was initialed");
	system("pause");
}

//-----------------------------------------------------------------
//               OutPut Warning Messages
//-----------------------------------------------------------------
void YellowWarnMsg(CString WarnMsg)
{
    //Get the color set handle
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);  
	//Set the color
    SetConsoleTextAttribute(hOut,  
                            BACKGROUND_RED |
							BACKGROUND_GREEN |
							BACKGROUND_INTENSITY
                            );//  
    cout <<WarnMsg; 
	//set the color back
	SetConsoleTextAttribute(hOut,  
                            FOREGROUND_GREEN |  
                            FOREGROUND_INTENSITY);
	cout<<endl;
}

//-----------------------------------------------------------------
//                Error Tips Messages
//-----------------------------------------------------------------
void OutputErrorTipsMsg(CString ErrorTipsMsg)
{
    //Get the color set handle
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);  
	//Set the color
    SetConsoleTextAttribute(hOut,  
                            BACKGROUND_RED |
							BACKGROUND_INTENSITY);  
    cout <<ErrorTipsMsg; 
	//set the color back
	SetConsoleTextAttribute(hOut,  
                            FOREGROUND_GREEN |  
                            FOREGROUND_INTENSITY); 
	cout<<endl;
}

//-----------------------------------------------------------------
//         allocate Training Image Value arrays
//-----------------------------------------------------------------
void AllocateTrainingImageValue()
{
    //ѭ������ 
    int i,j,k;
	//allocate arrays
	TIValue=new int**[TINz];
		for(i=0;i<TINz;i++)
		{
			TIValue[i]=new int*[TINy];
			for(j=0;j<TINy;j++) 
				TIValue[i][j]=new int[TINx];
		};
	//��ʼ��Ϊ-99
	for(i=0;i<TINz;i++)
		for(j=0;j<TINy;j++)
			for(k=0;k<TINx;k++)
			{
				TIValue[i][j][k]=-99;
			}
	GreenTipsMsg("Training Image Value Array was initialed");
}

//-----------------------------------------------------------------
//         allocate Training Image Value arrays
//-----------------------------------------------------------------
void DeAllocateTrainingImageValue(int TInx, int TIny, int TInz)
{
	//ѭ������ 
	cout<<"START TO DEALLOCATE!!!!!!!!!!!!!!!!!!!!!"<<endl;
	int i,j;
	//deallocate arrays
	if (TIType==0)
	{
		for (i = 0; i<TInz; i++)
		{
			for (j = 0; j<TIny; j++)
				delete[] TIValue[i][j];
		}
		for (i = 0; i<TInz; i++)
			delete[] TIValue[i];
		delete[] TIValue;
		TIValue=NULL;
	}
	if (TIType == 1)
	{
		for (i = 0; i < TInz; i++)
		{
			for (j = 0; j < TIny; j++)
				delete[] ContTIValue[i][j];
		}
		for (i = 0; i < TInz; i++)
			delete[] ContTIValue[i];
		delete[] ContTIValue;
		ContTIValue = NULL;
	}
	
	GreenTipsMsg("Training Image Value Array was deleted");
}

//-----------------------------------------------------------------
//         allocate Continual Training Image Value arrays
//-----------------------------------------------------------------
void AllocateContTrainingImageValue()
{
	//ѭ������ 
	int i,j,k;
	//allocate arrays
	ContTIValue=new double**[TINz];
	for(i=0;i<TINz;i++)
	{
		ContTIValue[i]=new double*[TINy];
		for(j=0;j<TINy;j++) 
			ContTIValue[i][j]=new double[TINx];
	};

	for(i=0;i<TINz;i++)
		for(j=0;j<TINy;j++)
			for(k=0;k<TINx;k++)
			{
				ContTIValue[i][j][k]=-99;
			}
	GreenTipsMsg("Continual Training Image Value Array was initialed");
}

//-----------------------------------------------------------------
//         deallocate continual Training Image Value arrays
//-----------------------------------------------------------------
void DeAllocateContTrainingImageValue(int m, int n, int p)
{
	//ѭ������ 
	int i,j;
	//deallocate arrays
	for(i=0;i<p;i++)
	{
		for(j=0;j<n;j++) 
			delete ContTIValue[i][j];
		delete ContTIValue[i];
	};
	delete ContTIValue;
	GreenTipsMsg("Training Image Value Array was deleted");
}

void AllocateAbnormalSituationArray()
{
	//ѭ������
	int i = 0, j = 0, k = 0;
	//��̬��������
	NumInvalidGP = new int* [NTI];
	NumNotEnoughNodesinTI = new int*[NTI];
	NumNotEnoughNodesinTIGP = new int*[NTI];
		for (i = 0; i < NTI;i++)
		{
			NumInvalidGP[i] = new int[NLevel];
			NumNotEnoughNodesinTI[i] = new int[NLevel];
			NumNotEnoughNodesinTIGP[i] = new int[NLevel];
		}
		for (i = 0; i < NTI; i++)
			for (j = 0; j < NLevel;j++)
			{
				NumInvalidGP[i][j] = 0;
				NumNotEnoughNodesinTI[i][j] = 0;
				NumNotEnoughNodesinTIGP[i][j] = 0;
			}
}

//-----------------------------------------------------------------
//              Read Training Image File
//-----------------------------------------------------------------
int readTI(string TIPath)
{
	errno_t err;
	//tips
	GreenTipsMsg("--->Start to READING Training Image file ");
	//File pointer
	FILE *TIf;
	//Number of data points at every step
	//data
	double v=0.0;
	//temple string
	char top[100];
	//Open and Read para file
	if ((err = fopen_s(&TIf, TIPath.c_str(), "r")) != 0)
	{
		OutputErrorTipsMsg("===TI data file Path Error!!!");
		return 0;
	}
	else
	{
		//Read file top
		fgets(top,99,TIf);
		fgets(top,99,TIf);
		fgets(top,99,TIf);
		NTIData=0;
		//��ȡѵ��ͼ��ע��ѵ��ͼ�������petrel�����ģ���ôZ�����˳�����ɴ�С
		for (int i = 0; i <TINz ; i++)
			for (int j = 0; j < TINy; j++)
				for (int k = 0; k < TINx; k++)
				{
					fscanf_s(TIf, "%lf", &v);
					if (TIType == 0)
					{
						TIValue[TINz-1-i][j][k] = int(v);
					}
					if (TIType == 1)
						ContTIValue[TINz - 1-i][j][k] = v;
					v = -999;
					NTIData++;
				}
	}
	fclose(TIf);
	cout<<"Training Image file reading completed!!!"<<endl;
	cout<<"Number of Training Image points:"<<NTIData<<endl;
	return 1;
}


//-----------------------------------------------------------------
//             Read Conditional data File
//-----------------------------------------------------------------
int readcond()
{
	errno_t err;
	//tips
	GreenTipsMsg("--->Start to READING Conditional data file ");
	//File pointer
	FILE *condf;
	//Number of data points
	int NCondData=0;
	//data
	int x=0,y=0,z=0;double v=0.0;
	//temple string
	char top[100];
	char *dataflstr;
	dataflstr=Datafl.GetBuffer(Datafl.GetLength());
	Datafl.ReleaseBuffer();

	//Open and Read para file
	if ((err = fopen_s(&condf, dataflstr, "r")) != 0)
	{
		cout<<"===Conditional data file Path Error!!!\n"<<endl;
		return 0;
	}
	else
	{
		//Read file top
		fgets(top,99,condf);
		fgets(top,99,condf);
		fgets(top,99,condf);
		fgets(top,99,condf);
		fgets(top,99,condf);
		fgets(top,99,condf);
		//read conditional data
		while(fscanf_s(condf,"%d %d %d %lf",&x,&y,&z,&v)!=EOF)
		{
			//cout<<"x-"<<x<<"  y-"<<y<<"  z-"<<z<<"  v-"<<v<<endl;
			if (TIType==0)   //��ɢ����
			{
				GridValue[z-1][y-1][x-1].discprop=int(v);
			}
			if (TIType==1)  //�����ͱ���
			{
				GridValue[z-1][y-1][x-1].contprop=v;
			}
			//cout<<int(v)<<"/";   //���ڼ����������
			GridValue[z-1][y-1][x-1].iscddata=1;  //����������Ϊ��������
			NCondData++;
			//cout<<"Value:"<<GridValue[Nz-z][y-1][x-1].contprop<<"     order="<<NCondData<<endl;
		}
	}
	fclose(condf);
	cout<<"Conditional data reading completed!!!\n"<<"Number of data points:"<<NCondData<<"     Conditional data Type:"<<TIType<<endl;
	return 1;
}

//-----------------------------------------------------------------
//             Read template Files
//-----------------------------------------------------------------
int Readtemplates()
{
	errno_t err;
	//tips
	GreenTipsMsg("--->Start to READING Template files ");
	//Number of data points
	int NCondData = 0;
	//�����������
	int MaxPointNum=0;
	//data
	int x = 0, y = 0, z = 0;
	int x1 = 0, y1 = 0, z1 = 0;
	int i = 0,j=0;
	
	for (int nn = 0; nn < NLevel;nn++)
	{
		//File pointer
		FILE *tempf;
		//temple string
		char top[100];
		//Open and Read para file
		if ((err = fopen_s(&tempf, TemplateFl[nn].c_str(), "r")) != 0)
		{
			cout << "===template data file Path Error!!!\n" << endl;
			return 0;
		}
		else
		{
			//Read file top
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			//read template data
			while (fscanf_s(tempf, "%d %d %d", &x, &y, &z) != EOF)
			{
				x1 = x; y1 = y; z1 = z;
				NCondData++;
				//cout<<"Value:"<<GridValue[Nz-z][y-1][x-1].contprop<<"     order="<<NCondData<<endl;
			}
		}
		fclose(tempf);
		NTemplateNode[nn] = NCondData;
		if (MaxPointNum<NCondData)
		{
			MaxPointNum = NCondData;
		}
		NCondData = 0;
	}
	//allocate the template
	TemplateNode = new int**[NLevel];
	for (i = 0; i < NLevel; i++)
	{
		TemplateNode[i] = new int*[MaxPointNum];
		for (j = 0; j < MaxPointNum;j++)
		{
			TemplateNode[i][j] = new int[3];
		}
	}
	for (i = 0; i < NLevel; i++)
		for (j = 0; j < MaxPointNum;j++)
		{
			TemplateNode[i][j][0] = 0; TemplateNode[i][j][1] = 0; TemplateNode[i][j][2] = 0;
		}

	for (int nn = 0; nn < NLevel; nn++)
	{
		//File pointer
		FILE *tempf;
		//temple string
		char top[100];
		//Open and Read para file
		if ((err = fopen_s(&tempf, TemplateFl[nn].c_str(), "r")) != 0)
		{
			cout << "===template data file Path Error!!!\n" << endl;
			return 0;
		}
		else
		{
			//Read file top
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			fgets(top, 99, tempf);
			//read template data
			while (fscanf_s(tempf, "%d %d %d", &x, &y, &z) != EOF)
			{
				TemplateNode[nn][NCondData][0] = x; TemplateNode[nn][NCondData][1] = y; TemplateNode[nn][NCondData][2] = z;
				NCondData++;
			}
		}
		fclose(tempf);
		NCondData = 0;
	}

	cout << "Reading Templates completed!!!\n" << endl;
	return 1;
}

//-----------------------------------------------------------------
//                 Read Parameter File
//-----------------------------------------------------------------
int ReadPar(CString ParaFile)
{
	int i=0,j=0;  //��̬����������ʼ����ѭ������
	errno_t err;
	ifstream fin(ParaFile);
	//tips
	GreenTipsMsg("--->Start to READING ParaFl ");
    //File pointer
	FILE *pf;
	//temple cstring
	char temp[100];
	//Open and Read para file
	if((err=fopen_s(&pf,ParaFile,"r"))!=0)
	{
		cout<<"===ParaFile Path Error!!!\n"<<endl;
		return 0;
	}
	else
	{
		//������������Ԫ����
		fin>>CalRatio; fin.ignore(256,'\n');
		if(CheckPf) cout<<"Ratio of grid nodes to be considered:   "<<CalRatio<<endl;
		//ѵ��ͼ������
		fin>>TIType;  fin.ignore(256,'\n');
		if(CheckPf) cout<<"Training Image Type:   "<<TIType<<endl;
		//��ɢ��������������Ncut
		fin>>Ncut;  fin.ignore(256,'\n');
		if(CheckPf) cout<<"Number of Category variables:   "<<Ncut<<endl;
		//ƥ��ȼ���ĳ߶ȷּ���
		fin>>NLevel;  fin.ignore(256,'\n');
		if(CheckPf) cout<<"Number of templates:   "<<NLevel<<endl;
		//template�ļ�·�����鶯̬�������ʼ��
		TemplateFl = new string[NLevel];
		for (int m = 0; m < NLevel; m++)
			TemplateFl[m] = "";
		//template type
		fin >> TemplateType;  fin.ignore(256, '\n');
		if (CheckPf) cout << "Template type:   " << TemplateType << endl;
		//��ͬ�߶ȵ��������ڴ�С����̬�������ȡ��
		searchwindowscale=new int*[NLevel];
		for(i=0;i<NLevel;i++)
			searchwindowscale[i]=new int[3];
		for(i=0;i<NLevel;i++)
			{
				for(j=0;j<3;j++)
				{
					searchwindowscale[i][j]=1;
					fin>>searchwindowscale[i][j];
				}
				if(CheckPf) cout<<"SearchWindow-x:"<<searchwindowscale[i][0]<<"   SearchWindow-y:"<<searchwindowscale[i][1]<<"   SearchwWindow-z:"<<searchwindowscale[i][2]<<endl;
				fin.ignore(256,'\n');
				//read the path of template file
				fin >> temp; TemplateFl[i] = temp;    fin.ignore(256, '\n');
				if (CheckPf) cout << "template file path:   " << temp << endl;
			}
		//ƥ���(�����¼��ľ���)���㷽ʽ��0-��ģʽ��1-�����ͺ���Ȩ[cate]��2-ŷ�Ͼ���[cont]������һ�в������ݱ���ѡ�����
		fin>>DistanceType;   fin.ignore(256,'\n');
		if(CheckPf) cout<<"DistanceType:"<<DistanceType<<endl;
		//���������ļ�
		fin>>temp;Datafl=temp;    fin.ignore(256,'\n');
		if(CheckPf) cout<<"Conditional data file path:   "<<Datafl<<endl;
		//ģ�������С
		fin>>Nx>>Ny>>Nz;   fin.ignore(256,'\n');
		if(CheckPf) cout<<"Nx:"<<Nx<<"   Ny:"<<Ny<<"   Nz:"<<Nz<<endl;
		//GridValue�����ʼ��
		AllocateGridValue();    //Allocate the GridValue array
		//ѵ��ͼ������
		fin>>NTI;  fin.ignore(256,'\n');
		if(CheckPf) cout<<"Number of TI:   "<<NTI<<endl;
		//�ж�ѵ��ͼ�������Ƿ����
		if(DistanceType==2 && TIType!=1)
			OutputErrorTipsMsg("Training Image Type is conflicted with Distance type of Data Events");
		//ѵ��ͼ���ļ�·�����鶯̬�������ʼ��
		TrainingImageFl=new string[NTI];
		for (int m=0;m<NTI;m++)
			TrainingImageFl[m]="";
		TIscalex=new int[NTI]; TIscaley=new int[NTI];  TIscalez=new int[NTI];
		//��ʼ��
		for (i = 0; i < NTI; i++)
		{
			TIscalex[i] = 0; TIscaley[i] = 0; TIscalez[i] = 0;
		}
		for (i=0;i<NTI;i++)
		{
			//��ȡѵ��ͼ���ļ���
			fin>>temp;TrainingImageFl[i]=temp;   fin.ignore(256,'\n');
			//��ȡѵ��ͼ���ģ
			if(CheckPf) cout<<"File path of Training Image NO."<<i+1<<":"<<TrainingImageFl[i]<<endl;
			//��ȡѵ��ͼ��Ĺ�ģ
			fin>>TIscalex[i]>>TIscaley[i]>>TIscalez[i];    fin.ignore(256,'\n');
			if(CheckPf) cout<<"Size of Training Image NO."<<i+1<<":"<<TIscalex[i]<<"*"<<TIscaley[i]<<"*"<<TIscalez[i]<<endl;
		}
		//MatchingDegree�����ʼ��
		AllocateMatchingDegreeArray();
		//�쳣��¼�����ʼ��
		AllocateAbnormalSituationArray();
		//����ģʽ��ѵ��ͼ����Ҫ�����Сƥ���
		fin>>MinMatchingDegree;    fin.ignore(256,'\n');
		if(CheckPf) cout<<"Min Matching Degree:   "<<MinMatchingDegree<<endl;

		//ģʽ�ڵ����Ŀ----��ʱδ����
		//fin>>PatternNode;    fin.ignore(256,'\n');
		//if(CheckPf) cout<<"PatternNode:"<<PatternNode<<endl;
		//If the parameter PatternNode is too large,warning and reset that parameter.
		//if(PatternNode>MaxNode) 
		//{
		//	YellowWarnMsg("Warning: PatternNode is larger than MaxNode !!!");
		//	PatternNode=MaxNode;
		//}
		//����������С���������ݵ�����MinCDNum
		fin>>MinCDNum;   fin.ignore(256,'\n');
		if(CheckPf) cout<<"Min Conditional Data Points Number:   "<<MinCDNum<<endl;
		//��ѵ��ͼ���н���ģʽ����ʱ���ɽ���ƥ�������Ҫ�����С�������ݵ�����MinD
		fin>>MinD;    fin.ignore(256,'\n');
		if(CheckPf) cout<<"Min Conditional Data Points Number:   "<<MinD<<endl;
		
		//�������
		fin>>seed;   fin.ignore(256,'\n');
		if(CheckPf) cout<<"Random seed:   "<<seed<<endl;
		//���ƥ��Ƚ�����ļ�·��
		fin>>temp; MatchingResultfl=temp;    fin.ignore(256,'\n');
		if(CheckPf) cout<<"Matching Results output file path:   "<<MatchingResultfl<<endl;
		//read templates
		if (TemplateType==1)
		{
			Readtemplates();
		}
	}
	fclose(pf);
	cout<<"Parameters reading completed!!!"<<endl;
	system("pause");
	return 1;
}

//-----------------------------------------------------------------
//              Write Version information
//-----------------------------------------------------------------
void WriteVersion()
{
	cout<<"***********************************************************\n";
	cout<<"              Training Image Selection Tools                    \n\n";
	cout<<"    by Wenjie Feng, Shenghe WU,Yanshu Yin, Ke Zhang \n";
	cout<<"                           V1.0\n";
	cout<<"                     Data: 2017-01-02\n";
	cout<<"***********************************************************\n"<<endl;
}

//-----------------------------------------------------------------
//     Read Parameter,Conditional data and TI file
//-----------------------------------------------------------------
int ReadParaAndData()
{
	//Parameter File operations
	string ParaFile="                          ";
	GreenTipsMsg("--->Input Parameter File Path: ");
	cin>>ParaFile;
	//Check the ParaFl
	if(_access(ParaFile.c_str(),0)==-1)
	{
		OutputErrorTipsMsg("Error: The ParaFl doesn't exist.check your parameter file path !!!");
		return 0;
	}
	
	//Switch to display details or not
	GreenTipsMsg("--->Display details?  Yes-1 , No-0");
	cin>>DisplayDetails;

	//ѡ����ʾ���߲���ʾƥ���е��쳣GP
	YellowWarnMsg("--->Display exception of GP?  Yes-1 , No-0");
	cin>>DisplayException;

	//ѡ����ʾ���߲���ʾƥ���е��쳣GP
	YellowWarnMsg("--->Display search and match process of GP?  Yes-1 , No-0");
	cin >> DisplaySearch_Match;

	//call the ParaFl reader and initial programs
	GreenTipsMsg("--->CHECK Parameter File?  Yes-1 , No-0 ");
	cin>>CheckPf; cout<<"CHECK ParaFl:   "<<CheckPf<<endl;
	if(ReadPar(ParaFile.c_str())==0)
	{
	 OutputErrorTipsMsg("===check your parameter file !!!");
	 return 0;
	}

	//Read the conditional data file with function "readcond()"
	if(readcond()==0)
	{
	 OutputErrorTipsMsg("===check your conditional data file path!!!");
	 return 0;
	}

	/*	//���������ж�ȡѵ��ͼ��
	//Read the Training Image file with function "readTI()"
	if(readTI()==0)
	{
	 OutputErrorTipsMsg("===check your Training Image file path!!!");
	 return 0;
	}
	*/
	//complete and return
    return 1;
}

//-----------------------------------------------------------------
//                       �������·��
//-----------------------------------------------------------------
void RandomPathConstruction(string RPfilename)
{
	errno_t err,err1;
	int i=0,j=0,k=0;
	int	***RandomNum;
	int maxnum=0;   //ģ���������ݵ���
	maxnum=Nx*Ny*Nz;
	char top[100];
	int v1=0,v2=0,v3=0,v4=0;
	
	//�����е����ֵ
	int maxrandomnum=0;   
	//����������·�����鲢��ʼ��
	RR=new RandomRoutine[maxnum];
	for(i=0;i<maxnum;i++)
	{
		RR[i].rpx=0; RR[i].rpy=0; RR[i].rpz=0;
	}
	//��̬����ģ��������
	RandomNum=new int**[Nz];
	for(i=0;i<Nz;i++)
	{
		RandomNum[i]=new int*[Ny];
		for (j=0;j<Ny;j++)
		{
			RandomNum[i][j]=new int[Nx];
		}
	}
	//Ϊÿ��ģ��������������
	for (i=0;i<Nz;i++)
		for(j=0;j<Ny;j++)
			for(k=0;k<Nx;k++)
		{
			RandomNum[i][j][k]=rand()%maxnum+1;
		}
	//��ȡ���·��
	if (ReadPath==1)
	{
		FILE *Pathfl;
		if ((err1 = fopen_s(&Pathfl, RPfilename.c_str(), "r")) != 0)
		{
			OutputErrorTipsMsg("===Random path file Path Error!!!");
			exit(0);
		}
		else
		{
			//Read file top
			fgets(top,99,Pathfl);
			fgets(top,99,Pathfl);
			fgets(top,99,Pathfl);
			fgets(top,99,Pathfl);
			fgets(top,99,Pathfl);
			fgets(top,99,Pathfl);
			//read conditional data
			for (i=0;i<Nz;i++)
				for(j=0;j<Ny;j++)
					for(k=0;k<Nx;k++)
					{
						fscanf_s(Pathfl,"%d\t%d\t%d\t%d",&v1,&v2,&v3,&v4);
						RR[v4].rpx=v1;RR[v4].rpy=v2;RR[v4].rpz=v3;
					}
		}
		fclose(Pathfl);
	}
	//Ϊģ�������������������·��
	if (ReadPath==0)
	{
		//���·�����
		FILE *RRF; //���·������ļ�ָ��
		err = fopen_s(&RRF, "RandomPath.txt", "w");
		fprintf(RRF, "RandomPath File\n");
		fprintf(RRF, "4\nx\ny\nz\norder\n");
		for(int m=0;m<maxnum;m++)
		{
			for (i=0;i<Nz;i++)
				for(j=0;j<Ny;j++)
					for(k=0;k<Nx;k++)
					{
						if(maxrandomnum<RandomNum[i][j][k])
						{
							maxrandomnum=RandomNum[i][j][k];
							RR[m].rpx=k;
							RR[m].rpy=j;
							RR[m].rpz=i;
						}
					}
					maxrandomnum=0;
					RandomNum[RR[m].rpz][RR[m].rpy][RR[m].rpx]=-1;     //��ֹ�Ѿ���Ϊ·���ĵ���ź��·��
					//cout<<"random path point N.O."<<m+1<<":  "<<RR[m].rpx<<"   "<<RR[m].rpy<<endl;   //�������
					//������·��
					fprintf(RRF,"%d\t%d\t%d\t%d\n",RR[m].rpx,RR[m].rpy,RR[m].rpz,m+1);
		}
		fclose(RRF);
	}
	GreenTipsMsg("Random Path is ready !!!");
}

//-----------------------------------------------------------------
//          Construct a geopattern with a original point
//
//  Input parameters: order ��ģ��˳�����·���ĵ�ţ�
//  Original point from randompath array
//  ע�⣺����ģʽ(GP)��һ��ȫ�ֱ������䳤��ΪNCD��
//-----------------------------------------------------------------
void PatternConstruction(int order)
{
	int i=0,j=0,k=0,m=0;   //ѭ�����ƣ�m����GP���������л�ȡ��������������
	int	sx=0,sy=0,sz=0;  //�������ڴ�С
	int NN=0;  //�������ڵ���
	sx=searchwindowscale[iLevel][0];sy=searchwindowscale[iLevel][1];sz=searchwindowscale[iLevel][2];  //��ʼ���������ڴ�С,���ղ�ͬ���ν���
	NCD=0;  //���������������ݵ�ĸ�������ʼ��Ϊ0����
	GPmatching=0;    //��ʼ��������GPƥ��
	//ͨ�����·���㴴������ģʽ
	if (TemplateType==0)
	{
	for(i=0;i<sz;i++)
		for (j=0;j<sy;j++)
			for (k=0;k<sx;k++)
			{ 
				if((RR[order].rpy+j-sy/2)>=0 && (RR[order].rpy+j-sy/2)<Ny && (RR[order].rpx+k-sx/2)>=0 && (RR[order].rpx+k-sx/2)<Nx && (RR[order].rpz+i-sz/2)<Nz && (RR[order].rpz+i-sz/2)>=0)
				{	
					if(DistanceType==0)
					{
						if(GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].discprop>-1)
							NCD++;
					}
					if(DistanceType==1)
					{
						if (GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].discprop>-1)
						//if (GridValue[RR[order].rpz + i - sz / 2][RR[order].rpy + j - sy / 2][RR[order].rpx + k - sx / 2].discprop>-1 && GridValue[RR[order].rpz][RR[order].rpy][RR[order].rpx].discprop<0)
						{
							NCD++;
						}
							
					}
					if(DistanceType==2)
					{
						if(GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].contprop>-1)
							NCD++;
					}
					NN++;
					//�������
					//cout<<"TIType="<<TIType<<"  �������ڵ���"<<NN<<"    NCD:"<<NCD<<"   DATA-"<<GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].contprop<<endl;

				}
			}
	}
	if (TemplateType == 1)
	{
		for (i = 0; i<NTemplateNode[iLevel]; i++)
		{
			if ((RR[order].rpy + TemplateNode[iLevel][i][1]) >= 0 && (RR[order].rpy + TemplateNode[iLevel][i][1])<Ny && (RR[order].rpx + TemplateNode[iLevel][i][0]) >= 0 && (RR[order].rpx + TemplateNode[iLevel][i][0])<Nx && (RR[order].rpz + TemplateNode[iLevel][i][2])<Nz && (RR[order].rpz + TemplateNode[iLevel][i][2]) >= 0)
			{
				if (DistanceType == 0)
				{
					if (GridValue[RR[order].rpz + TemplateNode[iLevel][i][2]][RR[order].rpy + TemplateNode[iLevel][i][1]][RR[order].rpx + TemplateNode[iLevel][i][0]].discprop > -1)
						NCD++;
				}
				if (DistanceType == 1)
				{
					if (GridValue[RR[order].rpz + TemplateNode[iLevel][i][2]][RR[order].rpy + TemplateNode[iLevel][i][1]][RR[order].rpx + TemplateNode[iLevel][i][0]].discprop > -1)
						//if (GridValue[RR[order].rpz + i - sz / 2][RR[order].rpy + j - sy / 2][RR[order].rpx + k - sx / 2].discprop>-1 && GridValue[RR[order].rpz][RR[order].rpy][RR[order].rpx].discprop<0)
					{
						NCD++;
					}

				}
				if (DistanceType == 2)
				{
					if (GridValue[RR[order].rpz + TemplateNode[iLevel][i][2]][RR[order].rpy + TemplateNode[iLevel][i][1]][RR[order].rpx + TemplateNode[iLevel][i][0]].contprop > -1)
						NCD++;
				}
				NN++;
			}
		}
	}
	//���������ݵ����ȥ������
	if (GridValue[RR[order].rpz][RR[order].rpy][RR[order].rpx].discprop >-1)
		NCD = NCD - 1;

	//���δ��ȡ���㹻���������ݵ㣬����ʾ����
	if (NCD<MinCDNum)
	{
		//��¼NCD̫С����޷�����GP���������
		NumInvalidGP[iTI][iLevel] = NumInvalidGP[iTI][iLevel] + 1;
		GPmatching=0;   //������GP������ƥ��
		//cout << "NCD<MinCDNum" << endl;
		if(DisplayDetails)
		{
			CString ERRTIP;
			ERRTIP.Format("NCD is less than MinCDNum: NCD=%d, MinCDNum=%d.  The current node is ignored.",NCD,MinCDNum);
			OutputErrorTipsMsg(ERRTIP);
			cout<<"\nLocation:\nrpx="<<RR[order].rpx<<"\trpy="<<RR[order].rpy<<"\trpz="<<RR[order].rpz<<"\nOrder="<<order<<"\n---------------"<<endl;
			//PlayTipsSound(1);
			if (NCD>=1)
			{
				PlayEmergencySound(1);
			}
		}
	}
	//������������ڵ��������ݵ�������꣬�򴴽�����ģʽ GeoPattern	
	if(NCD>=MinCDNum)
	{
		/*if (GridValue[RR[order].rpz][RR[order].rpy][RR[order].rpx].discprop>-1)
		{
			NumCDGP++;
			cout << "Num CDGP="<<NumCDGP<<endl;
			Sleep(2000);
		}*/  //�������λ���������ݵ��GP����
		//���ȼ�¼���ݵ���
		//NDAVG[iLevel][iTI]=NDAVG[iLevel][iTI]+1.0*NCD/ConsideredCellNum;
		//��̬�������ģʽ���鲢��ʼ��
		GP=new GeoPattern[NCD];
		for(i=0;i<NCD;i++)
		{
			GP[i].facies=-1; GP[i].ContVariable=-1; GP[i].px=0; GP[i].py=0; GP[i].pz=0;
		}
		//����GP
		//�����������ʾ	
		for (i=0;i<sz;i++)
			for (j=0;j<sy;j++)
				for (k=0;k<sx;k++)
				{
					if (!((j - sy / 2) == 0 && (k - sx / 2) == 0 && (i - sz / 2)==0))
					if((RR[order].rpy+j-sy/2)>=0 && (RR[order].rpy+j-sy/2)<Ny && (RR[order].rpx+k-sx/2)>=0 && (RR[order].rpx+k-sx/2)<Nx && (RR[order].rpz+i-sz/2)<Nz && (RR[order].rpz+i-sz/2)>=0)
					{
						if(DistanceType==0)
							if(GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].discprop>-1)
								{
									//��Բ�ͬ���͵ı�������GP�ڲ����������
									GP[m].facies=GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].discprop;
									//����GP�ڲ�������������
									GP[m].px=k-sx/2;GP[m].py=j-sy/2;GP[m].pz=i-sz/2;
									//������
									m++;
									//�������
									//cout<<"PatternConstruction: ����"<<m-1<<"����"<<GP[m-1].px<<"  "<<GP[m-1].py<<"   "<<GP[m-1].pz<<endl;
									//Sleep(1000);
								}
						if(DistanceType==1)
							if(GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].discprop>-1)
								{
									//��Բ�ͬ���͵ı�������GP�ڲ����������
									GP[m].facies=GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].discprop;
									//����GP�ڲ�������������
									GP[m].px=k-sx/2; GP[m].py=j-sy/2; GP[m].pz=i-sz/2;
									//������
									m++;
								}
						if(DistanceType==2)
							if(GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].contprop>-1)
								{
									//��Բ�ͬ���͵ı�������GP�ڲ����������
									GP[m].ContVariable=GridValue[RR[order].rpz+i-sz/2][RR[order].rpy+j-sy/2][RR[order].rpx+k-sx/2].contprop;
									//����GP�ڲ�������������
									GP[m].px=k-sx/2;GP[m].py=j-sy/2;GP[m].pz=i-sz/2;
									//������
									m++;
									//�������
									//cout<<"PatternConstruction: ����"<<m-1<<"����"<<GP[m-1].px<<"  "<<GP[m-1].py<<"   "<<GP[m-1].pz<<endl;
									//Sleep(500);
								}
					}
				}
		GPmatching=1;
	}
	//if(DisplayDetails==1)
	//cout<<"---NCD:"<<NCD<<"     M:"<<m<<endl;   //NCD���������ݸ�����m��GP��ʵ�ʻ�ȡ�����������ݡ�
}

//-----------------------------------------------------------------
// ��ѵ��ͼ������ѰPatterns������Conditional Ppatterns����ƥ��Ա�
// �Ӷ���ȡ�����¼��������Сֵ
//-----------------------------------------------------------------
void search_match(int order) 
{
	int Nvisit=0;  //��ǰorder�����£�GP�Ƿ��н�����Ч�ļ���
	int NenoughnodesinTIGP = 0;  //��ǰorder�����£�GP�Ƿ��н�����Ч�ļ���
	//Variables to control the loop
	int CDorder=0;
	double md=0.0;  //Matching Degree
	int ND=0;  //����ѵ��ͼ���ڵ�������ĸ�����
	double MatchingNum=0;   //����������Pattern �� GP���Ǻϳ̶ȣ�������׼���ӣ�Ӧ���ۺϿ��ǵ������Ǻϵ�����
	CString TIPs;
	//double MaxMatch=0.0;   //ÿ�������¼��Ա�������ƥ���
	double minDistance=1000;  //��С����
	int naz=0;  //�ų�����Чֵ��������ĵ���
	CString  warntip;    //���󡢾�����ʾ
	int displayGP=0;   //��ƥ��Ƚϵͻ��ȡ���������¼�����������ʾGP�Ŀ���
	double DTop=0,DBase=0;   //�����¼���������У���ʽ�ķ��ӷ�ĸ
	double ED=0;   //���ڼ��㵱ǰ�������ĵ��ŷʽ����
	double mddisp = 0; //���ڵ���������鿴naz=0ʱ��md
	int tix = 0, tiy = 0, tiz = 0; //���ڵ���������鿴TI GP
	match=0;
	//First: Search for the similar GeoPattern within the TI
	//How to search and find the most similar patterns?
	//����ѵ��ͼ�����˳��ɨ��
	for (int i=0;i<TINz;i++)
		for (int j=0;j<TINy;j++)
			for (int k=0;k<TINx;k++)
				{
					naz = 0;
					//minDistance=1000;
					//ƥ��-�����Ϊ1�������򲻼�--������Ҫ�Ľ���
					for (CDorder=0;CDorder<NCD;CDorder++)   
					{
						tiz = i; tiy = j; tix = k;  //���ڵ���������鿴TI GP
						int NED_0 = 0;
						if((k+GP[CDorder].px)<TINx && (k+GP[CDorder].px)>=0 && (j+GP[CDorder].py)<TINy && (j+GP[CDorder].py)>=0 && (i+GP[CDorder].pz)<TINz && (i+GP[CDorder].pz)>=0)
						{
							//Distance type 0: The simple type
							if (DistanceType==0)
							{
								ND++;
								if (GP[CDorder].facies!=TIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px] && TIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px]>-1)    //��Ϊ���������¼�����   2015-05-28
								{
									MatchingNum+=1;    //������þ�������0-������򵥵ľ��룬��������ֵΪ1����СֵΪ0.
								}
							}
							//Distance type 1:Weighting based on distance
							if (DistanceType==1)
							{
								ND++;
								ED=sqrt(1.0*(GP[CDorder].px*GP[CDorder].px+GP[CDorder].py*GP[CDorder].py+GP[CDorder].pz*GP[CDorder].pz));  //��ǰ���ŷ�Ͼ���
								if (GP[CDorder].facies!=TIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px] && ED>0 && TIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px]>-1)     // && TIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px]>-99
									{
										//cout << TIValue[i + GP[CDorder].pz][j + GP[CDorder].py][k + GP[CDorder].px] << "    " << i + GP[CDorder].pz << "   " << j + GP[CDorder].py << "   " << k + GP[CDorder].px << endl;
										//cout << i << "   " << j << "   " << k << endl;
										//cout << GP[CDorder].pz << "   " << GP[CDorder].py << "   " << GP[CDorder].px << endl;
										//cout << "-------------------------------------------" << endl;
										//cout << "gp facies: " << GP[CDorder].facies << "    tivalue:" << TIValue[i + GP[CDorder].pz][j + GP[CDorder].py][k + GP[CDorder].px] << endl;
										//if (TIValue[i + GP[CDorder].pz][j + GP[CDorder].py][k + GP[CDorder].px]!=1)
										//{
										//	Sleep(10000);
										//}
										DTop=DTop+1.0/ED;
										naz++;
										//cout << "NCD= " << NCD<<"   "<<"ND= "<<ND<<"   "<<"DTop="<<DTop<<"\n"<<endl;
										//Sleep(50);
									}
								if (GP[CDorder].facies == TIValue[i + GP[CDorder].pz][j + GP[CDorder].py][k + GP[CDorder].px] && ED>0 && TIValue[i + GP[CDorder].pz][j + GP[CDorder].py][k + GP[CDorder].px]>-1)
								{
									naz++;
								}
							}
							//Distance type 2:Distance of continual variables
							//�������ĺ����ڱ����޸���δ���ģ�ò���ǶԵ�-2015-05-28
							if (DistanceType==2)
							{
								ND++;
								ED=sqrt(1.0*(GP[CDorder].px*GP[CDorder].px+GP[CDorder].py*GP[CDorder].py+GP[CDorder].pz*GP[CDorder].pz));  //�����¼�����
								if (ED == 0)
								{
									NED_0 = NED_0 + 1;
									nED_0total = nED_0total + 1;
								}
								if (ED>0)
								{
									if (ContTIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px]>-99)
									{
										DTop=DTop+1.0*(sqrt(1.0*(GP[CDorder].ContVariable-ContTIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px])*(GP[CDorder].ContVariable-ContTIValue[i+GP[CDorder].pz][j+GP[CDorder].py][k+GP[CDorder].px])))/ED;
										naz++;
									}
								}
								//cout<<"ED="<<ED<<"   dtop="<<DTop<<endl;
								//Sleep(1000);	
							}
						}   

						if (NED_0>1)
						{
							cout << "N ED_0: " << NED_0 <<"     nED_0total: "<< nED_0total <<"    x,y,z:"<<RR[order].rpx<<","<< RR[order].rpy<<","<< RR[order].rpz << endl;
						}
						NED_0 = 0;
					}  //���һ��GP�ļ������
				//	if (GridValue[RR[order].rpz][RR[order].rpy][RR[order].rpx].discprop > -1)
				//		cout << "��ɶ�������ļ���" <<"ND: "<< ND<<"MinCDNum: " << MinCDNum <<"dtop="<<DTop<< endl;

					//�Լ���Ľ�������ж�
					if (ND >= MinCDNum && ND>0)   //MinD Ϊ���������ָ����һ��ģʽ�У����ٵ�����������,����Ҫ��һЩ���������������
					{
						Nvisit = Nvisit + 1;  //��¼Ŀǰorder�Ƿ��дﵽ��Ҫ����С��ѵ��ͼ���������;
						if(DistanceType==0) md=MatchingNum/ND;
						if(DistanceType==1) md=DTop/ND;
						if(DistanceType==2) md=DTop/ND;
					//	if (GridValue[RR[order].rpz][RR[order].rpy][RR[order].rpx].discprop > -1)
					//	{
					//		cout << "1--md=" << md << "   rpx:" << RR[order].rpx << "   rpy:" << RR[order].rpy << "   rpz:" << RR[order].rpz << endl;
					//		cout << "naz:" << naz << "   NCD*MinD:" << NCD*MinD<< endl;
					//		Sleep(5000);
					//	}
						if (naz<int(NCD*MinD))
						{
							NenoughnodesinTIGP = NenoughnodesinTIGP + 1;
							md = 1000;
							naz = 0;
						}
						//��ȡ��С�������¼�����
						if(md<minDistance)
						{
							//��MatchingDegree���鸳ֵ
							MatchingDegree[iTI][iLevel][RR[order].rpz][RR[order].rpy][RR[order].rpx]=md;
							//cout<<"MD="<<md<<endl;
					//		if (GridValue[RR[order].rpz][RR[order].rpy][RR[order].rpx].discprop > -1)
					//		{
					//			cout << "2--md=" << md << endl;
					//			cout << "!!!!!!!!!!!!!" << endl;
					//			Sleep(5000);
					//		}
							//��ֵ��minDistance
							minDistance=md;
							mddisp = md;
							//���������Ϣ
							//cout << "ND:" << ND << "   MD:" << md << "   NAZ:" << naz << endl;
							if (md<=MinMatchingDegree)  goto stend2;
						}
					}
					//ִ���������¼��������֮�󣬶���Ҫ�������й���
					ND=0;   //����ѵ��ͼ���ڵĵ�����Ӧ��ѵ��ͼ����������Ե��λ�������
					MatchingNum=0.0;    //��������¼�������㷽��0������ɢ������δ��Ȩ���㡣
					md=0;    //��С����Ĺ��㣬û��ʵ�����á�
					DTop=0;    //�����¼����㹫ʽ1��Ӧ�ĺ������ӹ��㣬������������
					DBase=0;    //�����¼����㹫ʽ1��Ӧ�ĺ�����ĸ���㣬������������
				}
			    //�ж����order�Ƿ��н�����Ч�ļ��㣬�����Ѿ��ų����޷�����GP�����
				if (Nvisit <=0)
					NumNotEnoughNodesinTI[iTI][iLevel] = NumNotEnoughNodesinTI[iTI][iLevel] + 1;
				if (NenoughnodesinTIGP <= 0)
					NumNotEnoughNodesinTIGP[iTI][iLevel] = NumNotEnoughNodesinTIGP[iTI][iLevel] + 1;
			//��ƥ���δ�ܴﵽ1.0����δ����ȫƥ��ģ�һ������ɢ�ͱ�����������ϸ��Ϣ��ʾ
			if(MatchingDegree[iTI][iLevel][RR[order].rpz][RR[order].rpy][RR[order].rpx]>0.00001)    //����δ�ﵽ����Ϊ0�����������ϸ��Ϣ����ʾ
			{
				if(DisplayException==1)  //���������ʾ����
				{
					warntip.Format("The matching Degree finally is %f ,order:%d,NCD=%d",MatchingDegree[iTI][iLevel][RR[order].rpz][RR[order].rpy][RR[order].rpx],order,NCD);
					YellowWarnMsg(warntip);
					cout<<"Location:\nx="<<RR[order].rpx<<"\ty="<<RR[order].rpy<<"\tz="<<RR[order].rpz<<endl;
					cout<<"Display GP? Yes-1; NO-0"<<endl;
					cin>>displayGP;
					if (displayGP==1)
					{
						cout<<"--------------- GP Body Title ---------------"<<endl;
						for (int i=0;i<NCD;i++)
						{
							cout<<"Facies:"<<GP[i].facies<<"  px:"<<GP[i].px<<"  py:"<<GP[i].py<<"  pz:"<<GP[i].pz<<endl;
						}
						cout<<"---------------- GP Body End ----------------"<<endl;
					}
					system("pause");
				}
				goto stend2;
			}
		stend2: ;
			//�������GP��ѵ��GP
			if (DisplaySearch_Match==1)
			{
				cout << "--------------------------------Top--------------------------------" << endl;
				cout << "Search Order:" << (tix + 1)*(tiy + 1)*(tiz + 1) << endl;
				cout << "Cond GP" << endl;
				for (int a = 0; a < NCD; a++)
				{
					cout << GP[a].facies << "\t";
				}
				cout << endl;
				cout << "TI GP" << endl;
				for (CDorder = 0; CDorder < NCD; CDorder++)
					if ((tix + GP[CDorder].px) < TINx && (tix + GP[CDorder].px) >= 0 && (tiy + GP[CDorder].py) < TINy && (tiy + GP[CDorder].py) >= 0 && (tiz + GP[CDorder].pz) < TINz && (tiz + GP[CDorder].pz) >= 0)
					{
						cout << TIValue[tiz + GP[CDorder].pz][tiy + GP[CDorder].py][tix + GP[CDorder].px] << "\t";
					}
					else
						cout << "X\t";
				cout << endl;
				cout << "================================Bot================================" << endl;
				Sleep(2000);
				if (mddisp > 0.00000001 && naz > 0)
				{
					cout << "naz=" << naz << "   md=" << mddisp << endl;
					PlayTipsSound(1);
				}
			}
			//cout << "MD=" << MatchingDegree[iTI][iLevel][RR[order].rpz][RR[order].rpy][RR[order].rpx]<<endl;   //�������
}


//-----------------------------------------------------------------
//         GeoPaterns Reproduction with the GP get before
//
//  Data Used:GP,NCD
//  
//-----------------------------------------------------------------
void GeoPatternMatching(int	order)
{
	//������Ѱ��GP�����Ƶ�Pattern
	search_match(order);
	//if(DisplayDetails==1)
	//cout<<"NCD:"<<NCD<<"\torder:"<<order<<endl;    //���ƥ��������¼��������ݸ�������ǰλ��
	//����ģ��--��ʱ����ģ�⣬���ע�͵���������ʹ��
}


//-----------------------------------------------------------------
//                   Matching Results output
//
//  Data Used:MatchingDegree,NCD
//  Format:1--GSlib & Petrel
//  
//-----------------------------------------------------------------
void OutputMatchingResults(long **timecost)
{
	int i=0,j=0,k=0,m=0,n=0;
	//����ļ���ʼ��
	ofstream fout;  //�������ļ�
	ofstream Exceptionout;  //������������ͳ����������
	fout.open(MatchingResultfl);
	//File definitions
	fout<<"         Training Image Selection(TIS) Result Version 2016-06-17 "<<"CLOCKS_PER_SEC="<<CLOCKS_PER_SEC<<"\n";
	fout<<NLevel*NTI+1<<"\n";
	if(TIType==0)   fout<<"Facies CondData\n";
	if(TIType==1)   fout<<"Continual CondData\n";
//	if(TIType==0)   fout << "Last TI\n";   //������һ��ѵ��ͼ��
	for(j=0;j<NTI;j++)
		for(int i=0;i<NLevel;i++)
			fout<<"General_TI_"<<TrainingImageFl[j]<<"_"<<searchwindowscale[i][0]<<"*"<<searchwindowscale[i][1]<<"*"<<searchwindowscale[i][2]<<"_timecost="<<timecost[j][i]<<"ms"<<"\n";
			//fout<<"MatchingDegree_TI"<<TrainingImageFl[j]<<" Search_window_scale(x,y,z):"<<searchwindowscale[i][0]<<","<<searchwindowscale[i][1]<<","<<searchwindowscale[i][2]<<"--timecost="<<timecost[j][i]<<"ms"<<"\n";
			//fout<<"MatchingDegree_TI"<<j+1<<"_Level"<<i+1<<"\n";
	//Reals
	for (i = Nz-1; i>=0; i--)
		for(j=0;j<Ny;j++)
			for(k=0;k<Nx;k++)
			{
				if(TIType==0) fout<<GridValue[i][j][k].discprop<<"\t";
				if(TIType==1) fout<<GridValue[i][j][k].contprop<<"\t";
				//if (TIType == 0) fout << TIValue[i][j][k] << "\t";   //���ѵ��ͼ��������
				for(m=0;m<NTI;m++)
					for(n=0;n<NLevel;n++)
					{
						fout<<MatchingDegree[m][n][i][j][k]<<"\t";
					}
				fout<<"\n";
			}
			fout.close();
			GreenTipsMsg("Matching result output completed!!!");
			GreenTipsMsg(MatchingResultfl);
	//������������ͳ������
	Exceptionout.open("Exceptionout.txt");
	Exceptionout << "��������ͳ�ƽ��\n";
	for (i = 0; i < NTI; i++)
	{
		Exceptionout << "TI-" << i << "\n";
		for (j = 0; j < 3;j++)
		{ 
			if (j == 0) Exceptionout << "NumInvalidGP\t";
			if (j == 1) Exceptionout << "NumNotEnoughNodesinTI\t";
			if (j == 2) Exceptionout << "NumNotEnoughNodesinTIGP\t";
			for (k = 0; k < NLevel;k++)
			{
				if (j == 0) Exceptionout << NumInvalidGP[i][k]<<"\t";
				if (j == 1) Exceptionout << NumNotEnoughNodesinTI[i][k]<<"\t";
				if (j == 2) Exceptionout << NumNotEnoughNodesinTIGP[i][k]<<"\t";
			}
			Exceptionout << "\n";
		}
		Exceptionout << "\n";
	}
	Exceptionout.close();
}

void OutputAverageNumberofDataCovered()
{
	int i=0,j=0;
	//����ļ���ʼ��
	ofstream fout;
	fout.open("CDout.dat");
	//File definitions
	fout<<"         Training Image Selection(TIS) Result-Average of Number of Data Covered in Data Events"<<"\n";
	fout<<NLevel*NTI<<"\n";
	for(j=0;j<NTI;j++)
		for(int i=0;i<NLevel;i++)
			fout<<"TI"<<TrainingImageFl[j]<<"SWS_"<<searchwindowscale[i][0]<<"*"<<searchwindowscale[i][1]<<"*"<<searchwindowscale[i][2]<<"\n";
			//fout<<"TI"<<TrainingImageFl[j]<<"SWS_"<<searchwindowscale[i][0]<<"*"<<searchwindowscale[i][1]<<"*"<<searchwindowscale[i][2]<<"_CDAVG="<<NDAVG[i][j]<<"\n";
	fout.close();
	GreenTipsMsg("NDAVG output completed!!!");
}



//-----------------------------------------------------------------
//                     Main Function
//-----------------------------------------------------------------
void main()
{
	//����׼��
	int CellNum=0;   //ƽ���ϵ��������
	clock_t start,finish;  //��ȡ������ʼʱ�� 
	long **timecost;   //����ʱ��
	struct tm *ptr;   //ʱ��ṹ��
	int PRatio=0;  //�������
	string RPfilename = "                               ";	 //��ȡ�����·���ļ���
	time_t lt; 
	lt =time(NULL); 
	//Version info
	WriteVersion();	

	//Read Parameter,Conditional data and TI
	if(ReadParaAndData()==0)
		goto endline;
	else
	{
		//��ʼ������ʱ������
		timecost=new long*[NTI];
		for(int i=0;i<NTI;i++)
			timecost[i]=new long[NLevel];
		//generate a set of random numbers with the seed
		srand(seed);
		//Set a random path.
		//�Ƿ��ȡ�ֳɵ����·��
		cout<<"Read Random Path? 0-No, 1-Yes"<<endl;
		cin>>ReadPath;
		if (ReadPath == 1)
		{
			cout << "Input the Random Path file name:" << endl;
			cin >> RPfilename;
		}
		RandomPathConstruction(RPfilename);
		//��ʼ������
		CellNum=Nx*Ny*Nz;
		ConsideredCellNum=int(CellNum*CalRatio);
		//��Բ�ͬ��ѵ��ͼ����з���
		for (iTI=0;iTI<NTI;iTI++)
		{
			TINx=TIscalex[iTI]; TINy=TIscaley[iTI]; TINz=TIscalez[iTI]; 
			//��ɢ��ѵ��ͼ�������ʼ��
			if(TIType==0)
			{
				if(iTI>0)
					DeAllocateTrainingImageValue(TIscalex[iTI-1],TIscaley[iTI-1],TIscalez[iTI-1]);  //�ͷ�����
				AllocateTrainingImageValue();     //��̬����ѵ��ͼ������
			}
			//������ѵ��ͼ�������ʼ��
			if(TIType==1)
			{
				if(iTI>0)
					DeAllocateTrainingImageValue(TIscalex[iTI-1],TIscaley[iTI-1],TIscalez[iTI-1]);  //�ͷ�����
				AllocateContTrainingImageValue();
			}	
			//ѵ��ͼ���ȡ
			if(readTI(TrainingImageFl[iTI])==0)         //��ȡTI
			{
				OutputErrorTipsMsg("===check your Training Image file path!!!");
				exit(0);
			}
			
			//--------------------Perform Analysis---------------------
			//Stage 1��
			//�Բ�ͬ�����������м���
			for(iLevel=0;iLevel<NLevel;iLevel++)
			{
				NumCDGP = 0;   //��¼�������Ƿ��д���GP��������㡣
				//��¼��ʼʱ��
				start=clock();
				//��ʾ��ǰ�����ĳ߶�level
				CString levelnow;
				levelnow.Format("Working on TI:%d   level: %d",iTI+1,iLevel+1);
				GreenTipsMsg(levelnow);
				Sleep(1000);
				//Set a random path.
				//RandomPathConstruction();
				//��ʱ������ѭ��
				for(int i=0;i<ConsideredCellNum;i++)     //���ѭ������Ҫ����ͽ�һ����ƣ�Ӧ��ʹ�����·������Ҫ����������򷽷���---������
					{
						//step 1:Setup the search patterns
						//if(DisplayDetails==1)
						//cout<<"****************************************************************"<<endl;
						PatternConstruction(i);
						//step 2:Patterns matching
						if(GPmatching==1)
							GeoPatternMatching(i);
						if(((i+1)%(int(ConsideredCellNum/10)))==0)
						{
							lt =time(NULL); 
							ptr=gmtime(&lt); 
							printf(ctime(&lt)); 
							PRatio=int(floor(100.0*(i+1)/ConsideredCellNum+0.5));
							cout<<"Progressing: "<<"TI->"<<TrainingImageFl[iTI]<<"  Search Window scale->"<<"x-"<<searchwindowscale[iLevel][0]<<"  y-"<<searchwindowscale[iLevel][1]<<"  z-"<<searchwindowscale[iLevel][2]<<"----"<<PRatio<<"%"<<endl;
						}
					}
				finish=clock();
				timecost[iTI][iLevel]=finish-start;
				//NDAVG[iLevel][iTI]=1.0*ND[iLevel][iTI]/ConsideredCellNum;
				//end loop
			}
		}
		OutputMatchingResults(timecost);
		//OutputAverageNumberofDataCovered();
	}
	//End line to exit.
endline: Sleep(100);
	system("pause");
}