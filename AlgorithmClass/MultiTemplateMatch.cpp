/***************************************************************************
* 多模板匹配算法文件，实现了MultiMatch类的所有函数
****************************************************************************/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <io.h>
#include "structure.h"
#include "MultiTemplateMatch.h"

using namespace std;
using namespace cv;

vector<Result> MultiMatch::res;


MultiMatch::MultiMatch()
{
	this->res.clear();
	ClearVector(this->res);
}

MultiMatch::~MultiMatch()
{

}

template <class T>
void MultiMatch::ClearVector(vector<T>& vt)
{
	vector< T > vtTemp;
	vtTemp.swap(vt);
}

//************************************
// Method:    GetSobelEdge：得到图像的X、Y方向的Sobel信息
// FullName:  MultiMatch::GetSobelEdge
// Access:    private 
// Returns:   bool：返回是否执行函数成功
// Qualifier:
// Parameter: Mat InputImg：待处理的图像
// Parameter: Mat & InputSobelDx：存放图像X方向Sobel信息的Mat
// Parameter: Mat & InputSobelDy：存放图像Y方向Soble信息的Mat
//************************************
bool MultiMatch::GetSobelEdge(Mat InputImg, Mat& InputSobelDx, Mat& InputSobelDy)
{
	if (InputImg.empty())
	{
		return false;
	}
	Mat GrayInputImg;  //灰度图像
	cvtColor(InputImg, GrayInputImg, COLOR_BGR2GRAY);

	Sobel(GrayInputImg, InputSobelDx, CV_16S, 1, 0, 3);
	Sobel(GrayInputImg, InputSobelDy, CV_16S, 0, 1, 3);

	return true;
}

//************************************
// Method:    Read：从文件中读取模板信息
// FullName:  MultiMatch::Read
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: const string path：文件的路径（路径+文件名）
// Parameter: int & nTmpWidth：模板的宽度
// Parameter: int & nTmpHeight：模板的高度
// Parameter: int & nPyrCount：存储降采样次数的变量
// Parameter: vector<double> & srcData：存储原模板信息的变量
// Parameter: vector<double> & pyrData：存储降采样以后模板信息的变量
//************************************
bool MultiMatch::Read(const string path,int &nTmpWidth, int &nTmpHeight, int &nPyrCount, vector<double>& srcData, vector<double>& pyrData)
{
	if (path == "")
	{
		return false;
	}

	ifstream infile(path.c_str());
	if (!infile)
	{
		cout << "File Not Opened" << endl;
		infile.close();
		return  false;
	}

	string  str1, str2;
	infile >> str1 >> nTmpWidth >> nTmpHeight;

	double dNumber1;
	while (!infile.eof())
	{
		infile >> str2;
		if (str2 == "pyrTemplate")
		{
			infile >> nPyrCount;
			while (!infile.eof())
			{
				infile >> dNumber1;
				pyrData.push_back(dNumber1);
			}
		}
		else
		{
			dNumber1 = atof(str2.c_str());
			srcData.push_back(dNumber1);
			for (int i = 0;i < 4;++i)
			{
				infile >> dNumber1;
				srcData.push_back(dNumber1);
			}
		}
	}
	if (pyrData.size() > 0)
		pyrData.pop_back();
	if (!nPyrCount)
	{
		for (int i = 0;i < 5;++i)
		{
			srcData.pop_back();
		}
	}

	infile.close();
	return true;
}

//************************************
// Method:    listFiles：获取带绝对路径的文件名
// FullName:  MultiMatch::listFiles
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * pDir：文件夹的路径（绝对路径）
// Parameter: vector<string> & files：带绝对路径的文件名的集合
//************************************
bool MultiMatch::ListFiles(const char *pDir, vector<string> &files)
{
	if (NULL == pDir)
	{
		return false;
	}

	char cDirNew[200];
	strcpy_s(cDirNew, sizeof(cDirNew) / sizeof(cDirNew[0]), pDir);
	strcat_s(cDirNew, sizeof(cDirNew), "\\*.*");   // 在目录后面加上"\\*.*"进行第一次搜索
	intptr_t handle;
	_finddata_t findData;
	handle = _findfirst(cDirNew, &findData);
	if (handle == -1)        // 检查是否成功
	{
		return false;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;
		}
		else
		{
			files.push_back(string(pDir).append("\\").append(findData.name));
		}

		//cout << findData.name << "\t" << findData.size << " bytes.\n";
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // 关闭搜索句柄

	return true;
}

//************************************
// Method:    comp：比较两个对象的大小（按分数大小）
// FullName:  MultiMatch::comp
// Access:    private static 
// Returns:   bool：返回a的分数是否大于b的分数
// Qualifier:
// Parameter: MatchRst a
// Parameter: MatchRst b
//************************************
bool MultiMatch::comp(MatchRst a, MatchRst b)
{
	return (a.dScore) > (b.dScore);
}

//************************************
// Method:    CreateDoubleMatrix：为double类型的二位指针申请空间
// FullName:  MultiMatch::CreateDoubleMatrix
// Access:    private 
// Returns:   bool：返回申请空间是否成功
// Qualifier:
// Parameter: double * * & matrix：double类型的二维指针
// Parameter: int height：而二维数组的高度
// Parameter: int width：二维数组的宽度
//************************************
bool MultiMatch::CreateDoubleMatrix(double** &matrix, int nHeight, int nWidth)
{
	matrix = new double*[nHeight];
	if (NULL == matrix)
	{
		return false;
	}

	for (int iInd = 0; iInd < nHeight; iInd++)
	{
		matrix[iInd] = new double[nWidth];
		if (NULL == matrix[iInd])
		{
			return false;
		}
	}

	return true;
}

//************************************
// Method:    DeleteDoubleMatrix：释放double类型的二维数组的空间
// FullName:  MultiMatch::DeleteDoubleMatrix
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: double * * & matrix：double类型的二位指针
// Parameter: int nHeight：二维数组的高度
//************************************
void MultiMatch::DeleteDoubleMatrix(double **&matrix, int nHeight)
{
	for (int i = 0; i < nHeight; ++i)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
	matrix = NULL;
}

//************************************
// Method:    InvSqrt：计算开方的倒数
// FullName:  MultiMatch::InvSqrt
// Access:    private 
// Returns:   float：返回结果
// Qualifier:
// Parameter: float x：需要开方的数据
//************************************
float MultiMatch::InvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);
	return x;
}

//************************************
// Method:    FindGeoMatchModel：获取识别的结果
// FullName:  MultiMatch::FindGeoMatchModel
// Access:    private 
// Returns:   bool：返回是否识别成功
// Qualifier:
// Parameter: Mat srcSobelDx：原图的X方向Sobel信息
// Parameter: Mat srcSobelDy：原图的Y方向Sobel信息
// Parameter: MatchSection matchSection：待识别的区域（起点的X,Y和终点的X,Y）
// Parameter: int nofEdgePiex：待处理的模板边缘信息的个数
// Parameter: vector<CvPoint> pPositionEdge：模板边缘信息的坐标
// Parameter: vector<double> pEdgeMagnitude：模板边缘梯度的倒数
// Parameter: vector<double> pEdgeMX：模板边缘X方向信息
// Parameter: vector<double> pEdgeMY：模板边缘Y方向信息
// Parameter: double ScoreThreshold：分数阈值
// Parameter: double dGgreediness：贪婪系数
// Parameter: vector<MatchRst> & RstVec：存放结果的集合
//************************************
bool MultiMatch::FindGeoMatchModel(Mat srcSobelDx, Mat srcSobelDy, MatchSection matchSection, int nofEdgePiex,
	vector<CvPoint> pPositionEdge, vector<double> pEdgeMagnitude, vector<double> pEdgeMX, vector<double> pEdgeMY,
	double ScoreThreshold, double dGgreediness, vector<MatchRst> &RstVec)
{
	if (srcSobelDx.empty() || srcSobelDy.empty())  // 参数判断
	{
		return false;
	}
	if (matchSection.nStartX < 0 || matchSection.nStartX > srcSobelDx.cols ||
		matchSection.nStartY < 0 || matchSection.nStartY > srcSobelDx.rows ||
		matchSection.nEndX < 0 || matchSection.nEndX > srcSobelDx.cols ||
		matchSection.nEndY < 0 || matchSection.nEndY > srcSobelDx.rows)
	{
		return false;
	}
	if (nofEdgePiex <= 0 || ScoreThreshold <= 0.3 || ScoreThreshold >= 1 || dGgreediness <= 0 || dGgreediness >= 1)
	{
		return false;
	}
	if (pPositionEdge.size() <= 0 || pEdgeMagnitude.size() <= 0 || pEdgeMX.size() <= 0 || pEdgeMY.size() <= 0)
	{
		return false;
	}

	int nWidth = srcSobelDx.cols;
	int nHeight = srcSobelDx.rows;

	double dTx = 0, dTy = 0, dSx = 0, dSy = 0;
	double gradMag = 0;

	vector<vector<double>> matGradMag(nHeight, vector<double>(nWidth));
	bool bRev = false;

	// stoping criterias to search for model
	double dNormMinScore = ScoreThreshold / nofEdgePiex; // precompute minumum score 
	double dNormGreediness = (1 - dGgreediness * ScoreThreshold) / ((1 - dGgreediness)* nofEdgePiex); // precompute greedniness 

	for (int i = 0; i < nHeight; ++i)
	{
		for (int j = 0; j < nWidth; ++j)
		{
			dSx = srcSobelDx.at<short>(i, j);
			dSy = srcSobelDy.at<short>(i, j);

			gradMag = (dSx * dSx) + (dSy * dSy);
			matGradMag[i][j] = (gradMag) ? (InvSqrt(gradMag)) : 0;
		}
	}

	double dPartialSum = 0;
	int nCurX = 0, nCurY = 0;
	double dSumOfCoords = 0;
	double dPartialScore = 0;

	for (int i = matchSection.nStartY; i < matchSection.nEndY; ++i)
	{
		for (int j = matchSection.nStartX; j < matchSection.nEndX; ++j)
		{
			dPartialSum = 0;
			for (int m = 0; m < nofEdgePiex; ++m)
			{
				nCurX = i + pPositionEdge[m].x;	 // 模板点的x坐标template X coordinate
				nCurY = j + pPositionEdge[m].y;  // 模板点的y坐标template Y coordinate
				dTx = pEdgeMX[m];	// template X derivative
				dTy = pEdgeMY[m];    // template Y derivative
				if (nCurX < 0 || nCurY < 0 || nCurX >nHeight - 1 || nCurY > nWidth - 1)//防止越界
					continue;

				dSx = srcSobelDx.at<short>(nCurX, nCurY);
				dSy = srcSobelDy.at<short>(nCurX, nCurY);

				if ((dSx != 0 || dSy != 0) && (dTx != 0 || dTy != 0))
				{
					dPartialSum += ((dSx * dTx) + (dSy * dTy))*(pEdgeMagnitude[m] * matGradMag[nCurX][nCurY]);
				}

				dSumOfCoords = m + 1;
				dPartialScore = dPartialSum / dSumOfCoords;//计算模板每一像素匹配得到的分数
				if (dPartialScore < (MIN((ScoreThreshold - 1) + dNormGreediness * dSumOfCoords, dNormMinScore* dSumOfCoords)))
					break;
			}

			if (dPartialScore > ScoreThreshold)
			{
				MatchRst Rst;
				Rst.nCentX = j;
				Rst.nCentY = i;
				Rst.dScore = dPartialScore;
				RstVec.push_back(Rst);
			}

			if (dPartialScore < 0.3)
			{
				j += 2;
			}
		}
	}

	return true;
}

//************************************
// Method:    threadproc：线程中调用的函数，用来获得每一个模板的识别结果
// FullName:  MultiMatch::threadproc
// Access:    private static 
// Returns:   void
// Qualifier:
// Parameter: InputImagInfo info：输入图像的信息（InputImageInfo类型）
// Parameter: TmpInfo tmpinfo：模板的信息（TmpInfo类型）
// Parameter: double dScoreThreshold：分数阈值
// Parameter: MultiMatch & sMM：MultiMatch对象
//************************************
void MultiMatch::threadproc(InputImagInfo info, TmpInfo tmpInfo, double dScoreThreshold, MultiMatch &sMM)
{
    Mat InputImg = info.InputImg;
    Mat InputSobelDx = info.InputSobelDx;
    Mat InputSobelDy = info.InputSobelDy;

	// tmpinfo通过read读进来
    string str = tmpInfo.tmpname;

    bool Res = false;
    Mat pyrImg = InputImg, dst;
	MatchSection matSec(0, 0, InputImg.cols, InputImg.rows);

	clock_t start1, finish1;
	double totaltime1;
	start1 = clock();

    //存在下采样
    if (tmpInfo.nPyrCount > 0) // 获得降采样次数
	{
        //源图像降采样
        for (int i = 0; i < tmpInfo.nPyrCount; ++i)
		{
            pyrDown(pyrImg, dst, Size(pyrImg.cols / 2, pyrImg.rows / 2));
            blur(dst, dst, Size(2, 2));
            pyrImg = dst;
        }

        int nPyrSize = tmpInfo.TmpPyrData.size();  // 获得降采样个数
        if (nPyrSize <= 0)
		{
            cout << str << "empty pyrdata" << endl;
            //cout << "线程" << id + 1 << "失败";
            return;
        }

        int nPyrnOfEdgePiex = nPyrSize / 5;  // 降采样个数/5=边缘信息个数
        if (nPyrnOfEdgePiex <= 0)
		{
            //cout << "线程" << id + 1 << "失败";
            return;
        }

        vector<CvPoint> pPositionEdge(nPyrnOfEdgePiex);
        vector<double> pEdgeMagnitude(nPyrnOfEdgePiex);
        vector<double> pEdgeMX(nPyrnOfEdgePiex);
        vector<double> pEdgeMY(nPyrnOfEdgePiex);

		// 变量接收保存的边缘数据，用于后续匹配
        int m = 0;
        for (int i = 0; i < nPyrSize; i = i + 5)
		{
            pPositionEdge[m].x = (int) tmpInfo.TmpPyrData[i];
            pPositionEdge[m].y = (int) tmpInfo.TmpPyrData[i + 1];
            pEdgeMagnitude[m] = tmpInfo.TmpPyrData[i + 2];
            pEdgeMX[m] = tmpInfo.TmpPyrData[i + 3];
            pEdgeMY[m] = tmpInfo.TmpPyrData[i + 4];
            m++;
        }

        //获取降采样源图像边缘梯度
        Mat InputpyrSobelDx, InputpyrSobelDy;
        Res = sMM.GetSobelEdge(pyrImg, InputpyrSobelDx, InputpyrSobelDy);
        if (!Res)
		{
            cout << str << "Inputpyr image sobel edge failed！" << endl;
            return;
        }

        //降采样模板匹配
        vector<MatchRst> pyrRstVec;

        double pyrScoreThreshold = 0.0;
		if(dScoreThreshold >= 0.5)
		{
			pyrScoreThreshold = 0.5;
		}
		else
		{
			pyrScoreThreshold = dScoreThreshold;
		}
		double dPyrdGreediness = 0.8;
		matSec.nStartX = 0;
		matSec.nStartY = 0;
		matSec.nEndX = pyrImg.cols;
		matSec.nEndY = pyrImg.rows;
        Res = sMM.FindGeoMatchModel(InputpyrSobelDx, InputpyrSobelDy, matSec, nPyrnOfEdgePiex,
                                    pPositionEdge, pEdgeMagnitude, pEdgeMX, pEdgeMY,
                                    pyrScoreThreshold, dPyrdGreediness, pyrRstVec);

        if (!Res)
		{
            cout << str << "Find shape pyrmodel failed" << endl;
            return;
        }

        int nCentX = -1, nCentY = -1;
        if (pyrRstVec.size() > 0)
		{
            sort(pyrRstVec.begin(), pyrRstVec.end(), comp);
            nCentX = pyrRstVec[0].nCentX;
            nCentY = pyrRstVec[0].nCentY;
        }
		else
		{
            return;
        }

        //降采样匹配中心映射至源图像的区域
// 		matSec.nStartX = (nCentX << tmpInfo.nPyrCount) - tmpInfo.nTmpWidth;
// 		matSec.nStartY = (nCentY << tmpInfo.nPyrCount) - tmpInfo.nTmpHeight;
// 		matSec.nEndX = matSec.nStartX + 2 * tmpInfo.nTmpWidth;
// 		matSec.nEndY = matSec.nStartY + 2 * tmpInfo.nTmpHeight;
		matSec.nStartX = (nCentX << tmpInfo.nPyrCount) - tmpInfo.nTmpWidth/2;
		matSec.nStartY = (nCentY << tmpInfo.nPyrCount) - tmpInfo.nTmpHeight/2;
		matSec.nEndX = matSec.nStartX + tmpInfo.nTmpWidth;
		matSec.nEndY = matSec.nStartY + tmpInfo.nTmpHeight;

        if (matSec.nStartX < 0) matSec.nStartX = 0;
        if (matSec.nStartY < 0) matSec.nStartY = 0;
        if (matSec.nEndX > InputImg.cols) matSec.nEndX = InputImg.cols;
        if (matSec.nEndY > InputImg.rows) matSec.nEndY = InputImg.rows;
    }

	finish1 = clock();
	totaltime1 = (double)(finish1 - start1);
	cout << "降采样时间" << totaltime1 << endl;

	// 模板图映射後的信息
    int nSrcSize = tmpInfo.TmpSrcData.size();
    if (nSrcSize <= 0)
	{
        cout << str << "empty srcdata" << endl;
        //cout << "线程" << id + 1 << "失败";
        return;
    }

	clock_t start2, finish2;
	double totaltime2;
	start2 = clock();

    int nSrcnOfEdgePiex = nSrcSize / 5;
    if (nSrcnOfEdgePiex <= 0)
	{
        return;
    }

    vector<CvPoint> srcpPositionEdge(nSrcnOfEdgePiex);
    vector<double> srcpEdgeMagnitude(nSrcnOfEdgePiex);
    vector<double> srcpEdgeMX(nSrcnOfEdgePiex);
    vector<double> srcpEdgeMY(nSrcnOfEdgePiex);

    int m = 0;
    for (int i = 0;i < nSrcSize;i = i + 5)
    {
        srcpPositionEdge[m].x = (int)tmpInfo.TmpSrcData[i];
        srcpPositionEdge[m].y = (int)tmpInfo.TmpSrcData[i + 1];
        srcpEdgeMagnitude[m] = tmpInfo.TmpSrcData[i + 2];
        srcpEdgeMX[m] = tmpInfo.TmpSrcData[i + 3];
        srcpEdgeMY[m] = tmpInfo.TmpSrcData[i + 4];
        m++;
    }

    //源图像模板匹配
    vector<MatchRst> srcRstVec;
    double dSrcScoreThreshold = 0.0;
	if(dScoreThreshold >= 0.6)
	{
		dSrcScoreThreshold = dScoreThreshold;
	}
	else
	{
		dSrcScoreThreshold = 0.6;
	}

	double dSrcGreediness = 0.8;
	// startX, startY, endX, endY 修改为结构体与单模板保持一致 ？
    Res = sMM.FindGeoMatchModel(InputSobelDx, InputSobelDy, matSec, nSrcnOfEdgePiex, srcpPositionEdge,
                                srcpEdgeMagnitude, srcpEdgeMX, srcpEdgeMY, dSrcScoreThreshold, dSrcGreediness, srcRstVec);
    if (!Res)
    {
        cout << str << "Find shape srcmodel failed" << endl;
        return;
    }

	finish2 = clock();
	totaltime2 = (double)(finish2 - start2);
	cout << "原图时间" << totaltime2 << endl;

    if (srcRstVec.size() > 0)
    {
        sort(srcRstVec.begin(), srcRstVec.end(), comp);
        Result r;
        r.nCentX = srcRstVec[0].nCentX;
        r.nCentY = srcRstVec[0].nCentY;
        r.targetname = str;
        res.push_back(r);   //res 作为参数引用传出
    }
    return;
}

//************************************
// Method:    DoInspect：得到识别结果
// FullName:  MultiMatch::DoInspect
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: Mat & InputImg：待识别的原图
// Parameter: vector<string> files：带绝对路径的文件名
// Parameter: double scoreThreshold：识别的阈值
// Parameter: MultiMatch & sMM：MultiMatch类的对象
//************************************
bool MultiMatch::DoInspect(Mat &InputImg, vector<string> vecFiles, double dScoreThreshold, int nThrNum, MultiMatch &sMM)
{
	bool Res = false;
	//读取txt文件
	
	int nFileNum = vecFiles.size();
	TmpInfo* tmpInfo= new TmpInfo[nFileNum];
	if (tmpInfo == NULL) 
	{
		return 1;
	}

	int pos = 0;
	string str;
	for (int i = 0; i < nFileNum; ++i)
	{
		// 读取txt内模板图数据（降采样+非降采样）
		Res = Read(vecFiles[i], tmpInfo[i].nTmpWidth, tmpInfo[i].nTmpHeight, tmpInfo[i].nPyrCount, tmpInfo[i].TmpSrcData, tmpInfo[i].TmpPyrData);
		if (!Res)
		{
			cout << " read failed" << endl;
            delete[] tmpInfo;
            tmpInfo = NULL;
			return 1;
		}

		//获取模板名字
		str = vecFiles[i];
		pos = str.rfind('.');
		str.erase(pos);
		pos = str.rfind('/');
		tmpInfo[i].tmpname = str.erase(0, pos + 1);
	}
	//获取源图像x、y方向梯度
	Mat InputSobelDx, InputSobelDy;
	Res = GetSobelEdge(InputImg, InputSobelDx, InputSobelDy);
	if (!Res)
	{
		cout << "Input image sobel edge failed！" << endl;
        delete[] tmpInfo;
        tmpInfo = NULL;
		return false;
	}

	InputImagInfo info;
	info.InputImg = InputImg;
	info.InputSobelDx = InputSobelDx;
	info.InputSobelDy = InputSobelDy;

	thr::threadpool pool(nThrNum);
	for (int i = 0; i < nFileNum;)
	{	
		//
		if (pool.idlCount() > 0) 
		{
			pool.commit(threadproc, info, tmpInfo[i], dScoreThreshold, sMM);
			cout << "空闲线程数量：" << pool.idlCount() << endl;
			i++;
		}
	}
	while (pool.idlCount() < nThrNum)
	{
		;
	}

    delete[] tmpInfo;
    tmpInfo = NULL;
    return true;
}
