/***************************************************************************
* 单模板匹配算法文件，实现了SingleMatch类的所有函数
****************************************************************************/

#include <iostream>
#include "SingleMatch.h"

//************************************
// Method:    comp
// FullName:  SingleMatch::comp
// Access:    private static 
// Returns:   bool
// Qualifier: 
// Parameter: MatchRst a
// Parameter: MatchRst b
//************************************
bool SingleMatch::comp(MatchRst a, MatchRst b)
{
	return (a.dScore) > (b.dScore);
}

SingleMatch::SingleMatch()
{
	nMaxPyCnt = 2;
	nMinTmpSize = 20;
	nDispersion = 1;
	maxThre = 100;
	minThre = 10;
	dGreediness = 0.8;
}

SingleMatch::~SingleMatch()
{

}

bool SingleMatch::CreateDoubleMatrix(double **&matrix, CvSize size)
{
	if (size.width <= 0 || size.height <= 0)
	{
		return false;
	}

	matrix = new double*[size.height];
	if (NULL == matrix)
	{
		return false;
	}

	for (int nInd = 0; nInd < size.height; nInd++)
	{
		matrix[nInd] = new double[size.width];
		if (NULL == matrix[nInd])
		{
			return false;
		}
	}

	return true;
}

bool SingleMatch::DeleteDoubleMatrix(double **&matrix, CvSize size)
{
	if (size.width <= 0 || size.height <= 0)
	{
		return false;
	}

	for (int i = 0; i < size.height; i++)
	{
		delete[] matrix[i];
	}
	matrix = NULL;

	return true;
}

bool SingleMatch::GetSobelEdge(Mat InputImg, Mat &InputSobelDx, Mat &InputSobelDy)
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

int SingleMatch::pyr(int nTmpHeight, int nTmpWidth)      //降采样次数
{
	if (nTmpWidth <= 0 || nTmpHeight <= 0)
	{
		return -1;
	}

	int nCount = 0;
	if (MIN(nTmpHeight, nTmpWidth) < nMinTmpSize)
	{
		return nCount;
	}

	for (int i = 1;i <= 10;i++)
	{
		nTmpHeight /= 2;
		nTmpWidth /= 2;
		if (MIN(nTmpHeight, nTmpWidth) < nMinTmpSize)
		{
			nCount = MIN(nMaxPyCnt, i - 1);
			break;
		}
	}
	return nCount;
}

bool SingleMatch::CreateMatchEdgeModel(Mat TemplateSobelXImg, Mat TemplateSobelYImg, double maxThre, double minThre, int &nofEdgePiex,
	CvPoint *pPositionEdge, double *pEdgeMagnitude, double *pEdgeMX, double *pEdgeMY)
{
	if (TemplateSobelXImg.empty() || TemplateSobelYImg.empty() || NULL == pPositionEdge
		|| NULL == pEdgeMagnitude || NULL == pEdgeMX || NULL == pEdgeMY)
	{
		return false;
	}

	int nTmpHeight = TemplateSobelXImg.rows;
	int nTmpWidth = TemplateSobelXImg.cols;

	int nCount = 0;

	int *pnOrients = new int[nTmpWidth * nTmpHeight];//存储方向
	if (NULL == pnOrients)
	{
		return false;
	}
	memset(pnOrients, 0, sizeof(nTmpWidth*nTmpHeight));

	double dDx = 0, dDy = 0;
	double dMag = 0, dDirection = 0;
	double dMaxGradient = -99999.99;

	bool bRev = false;
	double **pMagMat = NULL;
	Size size(nTmpWidth, nTmpHeight);
	bRev = CreateDoubleMatrix(pMagMat, size);
	if (!bRev)
	{
		delete[] pnOrients;
		pnOrients = NULL;
		return false;
	}

	// 计算模板图边缘强度和方向
	const short *pSdx = NULL;
	const short *pSdy = NULL;
	for (int i = 1; i < nTmpHeight - 1; i++)
	{
		pSdx = (short*)(TemplateSobelXImg.ptr<uchar>(i));
		pSdy = (short*)(TemplateSobelYImg.ptr<uchar>(i));
		for (int j = 1; j < nTmpWidth - 1; j++)
		{
			dDx = pSdx[j];
			dDy = pSdy[j];
			dMag = sqrt((float)(dDx * dDx) + (float)(dDy * dDy));  //边界强度
			dDirection = cvFastArctan((float)dDy, (float)dDx);	  //边界方向
			pMagMat[i][j] = dMag;

			if (dMag > dMaxGradient)
			{
				dMaxGradient = dMag; //正则化 get maximum gradient value for normalizing.
			}

			// get closest angle from 0, 45, 90, 135 set
			if ((dDirection > 0 && dDirection < 22.5) || (dDirection > 157.5 && dDirection < 202.5)
				|| (dDirection > 337.5 && dDirection < 360))
			{
				dDirection = 0;
			}
			else if ((dDirection > 22.5 && dDirection < 67.5) || (dDirection > 202.5 && dDirection < 247.5))
			{
				dDirection = 45;
			}
			else if ((dDirection > 67.5 && dDirection < 112.5) || (dDirection > 247.5 && dDirection < 292.5))
			{
				dDirection = 90;
			}
			else if ((dDirection > 112.5 && dDirection < 157.5) || (dDirection > 292.5 && dDirection < 337.5))
			{
				dDirection = 135;
			}
			else
			{
				dDirection = 0;
			}

			pnOrients[nCount] = (int)dDirection;
			nCount++;
		}

	}

	//根据边缘方向进行非最大抑制
	nCount = 0;
	double dLeftPixel = 0, dRightPixel = 0;
	Mat nmsEdges = Mat::zeros(nTmpHeight, nTmpWidth, CV_8UC1);
	for (int i = 1; i < nTmpHeight - 1; i++)
	{
		for (int j = 1; j < nTmpWidth - 1; j++)
		{
			switch (pnOrients[nCount])
			{
			case 0:
				dLeftPixel = pMagMat[i][j - 1];
				dRightPixel = pMagMat[i][j + 1];
				break;
			case 45:
				dLeftPixel = pMagMat[i - 1][j + 1];
				dRightPixel = pMagMat[i + 1][j - 1];
				break;
			case 90:
				dLeftPixel = pMagMat[i - 1][j];
				dRightPixel = pMagMat[i + 1][j];
				break;
			case 135:
				dLeftPixel = pMagMat[i - 1][j - 1];
				dRightPixel = pMagMat[i + 1][j + 1];
				break;
			}
			// 将当前像素值与相邻像素进行比较compare current pixels value with adjacent pixels
			if ((pMagMat[i][j] < dLeftPixel) || (pMagMat[i][j] < dRightPixel))
			{
				nmsEdges.at<uchar>(i, j) = 0;
			}
			else
			{
				nmsEdges.at<uchar>(i, j) = (uchar)(pMagMat[i][j] / dMaxGradient * 255);
			}

			nCount++;
		}
	}

	//滞后阈值
	int nFlg = 1;
	int nRSum = 0, nCSum = 0;
	CvPoint ptCenterOfGravity(0, 0);

	for (int i = 1; i < nTmpHeight - 1; i++)
	{
		pSdx = (short*)(TemplateSobelXImg.ptr<uchar>(i));
		pSdy = (short*)(TemplateSobelYImg.ptr<uchar>(i));
		for (int j = 1; j < nTmpWidth; j++)
		{
			dDx = pSdx[j];
			dDy = pSdy[j];
			dMag = sqrt(dDx * dDx + dDy * dDy); //梯度强度Magnitude = Sqrt(gx^2 +gy^2)												
			nFlg = 1;
			if (((double)(nmsEdges.at<uchar>(i, j))) < maxThre)
			{
				if ((double)(nmsEdges.at<uchar>(i, j)) < minThre)
				{

					(nmsEdges.at<uchar>(i, j)) = 0;
					nFlg = 0; // 不是边缘，remove from edge
				}
				else
				{   // 如果8个相邻像素中的任何一个不大于最大值，则从边缘移除
					if ((((double)(nmsEdges.at<uchar>(i - 1, j - 1))) < maxThre) &&
						(((double)(nmsEdges.at<uchar>(i - 1, j))) < maxThre) &&
						(((double)((nmsEdges.at<uchar>(i - 1, j + 1)))) < maxThre) &&
						(((double)((nmsEdges.at<uchar>(i, j - 1)))) < maxThre) &&
						(((double)((nmsEdges.at<uchar>(i, j + 1)))) < maxThre) &&
						(((double)((nmsEdges.at<uchar>(i + 1, j - 1)))) < maxThre) &&
						(((double)((nmsEdges.at<uchar>(i + 1, j)))) < maxThre) &&
						(((double)((nmsEdges.at<uchar>(i + 1, j + 1)))) < maxThre))
					{
						(nmsEdges.at<uchar>(i, j)) = 0;
						nFlg = 0;
					}
				}
			}
			//	imwrite("滞后阈值nmsEdges.bmp", nmsEdges);

			//保存提取到的边缘信息
			if (nFlg != 0)
			{
				if (dDx != 0 || dDy != 0)
				{
					nRSum = nRSum + i;	nCSum = nCSum + j; // Row sum and column sum for center of gravity

					pPositionEdge[nofEdgePiex].x = i;
					pPositionEdge[nofEdgePiex].y = j;
					pEdgeMX[nofEdgePiex] = dDx;
					pEdgeMY[nofEdgePiex] = dDy;

					//handle divide by zero
					if (dMag != 0)
					{
						pEdgeMagnitude[nofEdgePiex] = 1 / dMag;  // gradient magnitude 
					}
					else
					{
						pEdgeMagnitude[nofEdgePiex] = 0;
					}

					nofEdgePiex++;
				}
			}
		}
	}

	ptCenterOfGravity.x = nRSum / nofEdgePiex; // center of gravity
	ptCenterOfGravity.y = nCSum / nofEdgePiex;	// center of gravity

    // change coordinates to reflect center of gravity
	for (int m = 0; m < nofEdgePiex; m++)
	{
		pPositionEdge[m].x = pPositionEdge[m].x - ptCenterOfGravity.x;
		pPositionEdge[m].y = pPositionEdge[m].y - ptCenterOfGravity.y;
	}

	bRev = DeleteDoubleMatrix(pMagMat, size);
	if (!bRev)
	{
		delete[] pnOrients;
		pnOrients = NULL;
		return false;
	}

	delete[] pnOrients;
	pnOrients = NULL;
	return true;
}

float SingleMatch::InvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);
	return x;
}

bool SingleMatch::FindGeoMatchModel(Mat srcSobelDx, Mat srcSobelDy, MatchSection matchsect, size_t nofEdgePiex,
	CvPoint *pPositionEdge, double *pEdgeMagnitude, double *pEdgeMX, double *pEdgeMY,
	double ScoreThreshold, double dGgreediness, vector<MatchRst> &RstVec)
{
	if (srcSobelDx.empty() || srcSobelDy.empty() || matchsect.nStartX >= srcSobelDx.cols
		|| matchsect.nStartY >= srcSobelDx.rows || 0 > matchsect.nEndX || 0 > matchsect.nEndY || NULL == pPositionEdge
		|| NULL == pEdgeMagnitude || NULL == pEdgeMX || NULL == pEdgeMY || nofEdgePiex <= 0)
	{
		return false;
	}

	int nWidth = srcSobelDx.cols;
	int nHeight = srcSobelDx.rows;

	double dTx = 0, dTy = 0, dSx = 0, dSy = 0;
	double dGradMag = 0;

	Size size(nWidth, nHeight);
	double **matGradMag = NULL;
	bool bRev = false;
	bRev = CreateDoubleMatrix(matGradMag, size);
	if (!bRev)
	{
		cout << "Create Double Matrix Failed" << endl;
		return false;
	}

	// stoping criterias to search for model
	// nofEdgePiex:all edge dot count
	// dGgreediness: 边缘图中，有80%的点能匹配上认为OK
	// ScoreThreshold: 最终匹配可接受的分数阈值
	// dGgreediness * ScoreThreshold: 降低匹配要求，可匹配的点数减少了
	double dNormMinScore = ScoreThreshold / nofEdgePiex; // 每个点的匹配分数 
	double dNormGreediness = (1 - dGgreediness * ScoreThreshold) / ((1 - dGgreediness)* nofEdgePiex); // precompute greedniness 

	//计算源图像梯度强度倒数
	const short *pSdx = NULL;
	const short *pSdy = NULL;
	for (int i = 0; i < nHeight; ++i)
	{
		pSdx = (short*)(srcSobelDx.ptr<uchar>(i));
		pSdy = (short*)(srcSobelDy.ptr<uchar>(i));
		for (int j = 0; j < nWidth; ++j)
		{
			dSx = pSdx[j];
			dSy = pSdy[j];

			dGradMag = (dSx * dSx) + (dSy * dSy);
			matGradMag[i][j] = (dGradMag) ? (InvSqrt(dGradMag)) : 0;
		}
	}

	double dPartialSum = 0;
	int nCurX = 0, nCurY = 0;
	double dSumOfCoords = 0;
	double dPartialScore = 0;

	for (int i = matchsect.nStartY; i < matchsect.nEndY; ++i)
	{
		for (int j = matchsect.nStartX; j < matchsect.nEndX; ++j)
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

				pSdx = (short*)(srcSobelDx.ptr<uchar>(nCurX));
				pSdy = (short*)(srcSobelDy.ptr<uchar>(nCurX));
				dSx = pSdx[nCurY];   // 在源图中找到相应点的梯度
				dSy = pSdy[nCurY];

				if ((dSx != 0 || dSy != 0) && (dTx != 0 || dTy != 0))
				{
					// 原图X方向梯度值*模板图X方向梯度值 + 原图Y方向梯度值*模板图Y方向梯度值 的和
					// * 模板图梯度开方的倒数 * 原图梯度开方的倒数
					dPartialSum += ((dSx * dTx) + (dSy * dTy))*(pEdgeMagnitude[m] * matGradMag[nCurX][nCurY]);
				}

				dSumOfCoords = m + 1;   // 当前匹配到的个数
				dPartialScore = dPartialSum / dSumOfCoords;//计算模板每一像素匹配得到的分数
				if (dPartialScore < (MIN((ScoreThreshold - 1) + dNormGreediness * dSumOfCoords, dNormMinScore * dSumOfCoords)))
					break;
			}

			if (dPartialScore >= ScoreThreshold)
			{
				MatchRst Rst;
				Rst.nCentX = j;
				Rst.nCentY = i;
				Rst.dScore = dPartialScore;
				RstVec.push_back(Rst);
			}

			if (dPartialScore < 0.3)
			{
				j += nDispersion;
			}
		}
	}

	DeleteDoubleMatrix(matGradMag, size);

	return true;
}

bool SingleMatch::DoInspect(Mat InputSrcImg, Mat InputTmpImg, vector<MatchRst> &RstVec, int nMatchNum, double dScoreThreshold)
{
	if (InputSrcImg.empty() || InputTmpImg.empty() || dScoreThreshold <= 0 || dScoreThreshold >= 1
		|| nMatchNum < 1)
	{
		return false;
	}

	bool bRes = false;

	//计算降采样次数
	int nPyrCont = pyr(InputTmpImg.rows, InputTmpImg.cols);
	if (nPyrCont < 0)
	{
		return false;
	}

	clock_t start, end;
	double time;
	

	if (nPyrCont > 0) //需要降采样
	{
		Mat pyrsrc = InputSrcImg;
		Mat pyrtmp = InputTmpImg;
		Mat dst, dst1;
		//模板与源图像降采样
		for (int i = 1; i <= nPyrCont; i++)
		{
			pyrDown(pyrtmp, dst, Size(pyrtmp.cols / 2, pyrtmp.rows / 2));
			blur(dst, dst, Size(2, 2));
			pyrtmp = dst;
			pyrDown(pyrsrc, dst1, Size(pyrsrc.cols / 2, pyrsrc.rows / 2));
			blur(dst1, dst1, Size(2, 2));
			pyrsrc = dst1;
		}

		//降采样模板匹配过程
		vector<MatchRst> RstVect;
		MatchSection pyrSection;
		pyrSection.nStartX = pyrSection.nStartY = 0;
		pyrSection.nEndX = pyrsrc.cols;
		pyrSection.nEndY = pyrsrc.rows;

		start = clock();
		bRes = Match(pyrsrc, pyrtmp, dScoreThreshold, nPyrCont, pyrSection, RstVect);
		end = clock();
		time = (double)(end - start);
		cout << "降采样时间：" << time << endl;

		if (!bRes)
		{
			return false;
		}

		if (RstVect.size() > 0)
		{
			if (nMatchNum == 1)
			{
				sort(RstVect.begin(), RstVect.end(), comp);
				MatchRst rst;
				rst.nCentX = RstVect[0].nCentX;
				rst.nCentY = RstVect[0].nCentY;
				rst.dScore = RstVect[0].dScore;
				RstVect.clear();
				RstVect.push_back(rst);
			}
			else
			{
				bRes = DelNearElem(RstVect, 3);
				if (!bRes)
				{
					return false;
				}
			}

			//降采样模板匹配中心映射至源图像的区域范围
			int nSize = RstVect.size();
			if (nMatchNum <= nSize)
			{
				nSize = 2 * nMatchNum;
			}

			//降采样模板匹配中心映射至源图像的区域范围
			for (int i = 0; i <nSize; ++i)
			{
				pyrSection.nStartX = (RstVect[i].nCentX << nPyrCont) - InputTmpImg.cols;
				pyrSection.nStartY = (RstVect[i].nCentY << nPyrCont) - InputTmpImg.rows;
				pyrSection.nEndX = pyrSection.nStartX + 2 * InputTmpImg.cols + 1;
				pyrSection.nEndY = pyrSection.nStartY + 2 * InputTmpImg.rows + 1;

// 				pyrSection.nStartX = (RstVect[i].nCentX << nPyrCont) - InputTmpImg.cols/2;
// 				pyrSection.nStartY = (RstVect[i].nCentY << nPyrCont) - InputTmpImg.rows/2;
// 				pyrSection.nEndX = pyrSection.nStartX + InputTmpImg.cols + 1;
// 				pyrSection.nEndY = pyrSection.nStartY + InputTmpImg.rows + 1;

				if (pyrSection.nStartX < 0)
				{
					pyrSection.nStartX = 0;
				}
				if (pyrSection.nStartY < 0)
				{
					pyrSection.nStartY = 0;
				}
				if (pyrSection.nEndX > InputSrcImg.cols)
				{
					pyrSection.nEndX = InputSrcImg.cols;
				}
				if (pyrSection.nEndY > InputSrcImg.rows)
				{
					pyrSection.nEndY = InputSrcImg.rows;
				}

				//源模板匹配过程
				//VecSect.push_back(pyrSection);
				bRes = Match(InputSrcImg, InputTmpImg, dScoreThreshold, 0, pyrSection, RstVec);
				if (!bRes)
				{
					return false;
				}
			}
		}
		else
		{
			//cout << "不存在匹配结果!" << endl;
			return true;
		}
	}
	else
	{
		MatchSection Section;
		Section.nStartX = Section.nStartY = 0;
		Section.nEndX = InputSrcImg.cols;
		Section.nEndY = InputSrcImg.rows;

		bRes = Match(InputSrcImg, InputTmpImg, dScoreThreshold, 0, Section, RstVec);
		if (!bRes)
		{
			return false;
		}

	}

	if (RstVec.size() > 0)
	{
		bRes = DelNearElem(RstVec, 5);
		if (!bRes)
		{
			return false;
		}
	}

	if (RstVec.size() > nMatchNum)
	{
		RstVec.assign(RstVec.begin(), RstVec.begin() + nMatchNum);
	}

	return true;
}

bool SingleMatch::Match(Mat InputSrcImg, Mat InputTmpImg, double dScoreThreshold, int nPyrCont,
	MatchSection matchsect, vector<MatchRst> &RstVec)
{
	if (InputSrcImg.empty() || InputTmpImg.empty() || dScoreThreshold <= 0 || dScoreThreshold > 1
		|| matchsect.nStartX >= InputSrcImg.cols || matchsect.nStartY >= InputSrcImg.rows
		|| 0 > matchsect.nEndX || 0 > matchsect.nEndY)
	{
		return false;
	}

	bool bRes = false;
	//获取模板与源图的边缘
	Mat TmpSobelDx, TmpSobelDy, SrcSobelDx, SrcSobelDy;
	bRes = GetSobelEdge(InputTmpImg, TmpSobelDx, TmpSobelDy);
	if (!bRes)
	{
		cout << "sobel edge failed!" << endl;
		return false;
	}
	bRes = GetSobelEdge(InputSrcImg, SrcSobelDx, SrcSobelDy);
	if (!bRes)
	{
		cout << "sobel edge failed!" << endl;
		return false;
	}

	

	//创建模板匹配模型
	int nofEdgePiex = 0;
	CvPoint * pPositionEdge = new CvPoint[InputTmpImg.rows * InputTmpImg.cols];
	if (NULL == pPositionEdge)
	{
		return false;
	}
	double * pEdgeMagnitude = new double[3 * InputTmpImg.rows * InputTmpImg.cols];
	if (NULL == pEdgeMagnitude)
	{
		delete[] pPositionEdge;
		pPositionEdge = NULL;
		return false;
	}
	memset(pEdgeMagnitude, 0, sizeof(3 * InputTmpImg.rows * InputTmpImg.cols));
	double *pEdgeMX = pEdgeMagnitude + InputTmpImg.rows * InputTmpImg.cols;
	double *pEdgeMY = pEdgeMX + InputTmpImg.rows * InputTmpImg.cols;

	bRes = CreateMatchEdgeModel(TmpSobelDx, TmpSobelDy, maxThre, minThre, nofEdgePiex, pPositionEdge,
		pEdgeMagnitude, pEdgeMX, pEdgeMY);
	if (!bRes)
	{
		cout << " create match model failed!" << endl;
		delete[] pPositionEdge;
		delete[] pEdgeMagnitude;
		pPositionEdge = NULL;
		pEdgeMagnitude = NULL;
		return false;
	}

	//模板匹配
	double dScore = 0.0;
	if (nPyrCont > 0)  // 下采样次数大于0，分数降低进行匹配；否则不降采样在第一次匹配周围进行细匹配，提高分数阈值
	{
		dScore = 0.5;
	}
	else
	{
		dScore = dScoreThreshold;
	}

	bRes = FindGeoMatchModel(SrcSobelDx, SrcSobelDy, matchsect, nofEdgePiex, pPositionEdge,
		pEdgeMagnitude, pEdgeMX, pEdgeMY, dScore, dGreediness, RstVec);
	if (!bRes)
	{
		cout << "Find shape model failed!" << endl;
		delete[] pPositionEdge;
		delete[] pEdgeMagnitude;
		pPositionEdge = NULL;
		pEdgeMagnitude = NULL;
		return false;
	}

	delete[] pPositionEdge;
	delete[] pEdgeMagnitude;
	pPositionEdge = NULL;
	pEdgeMagnitude = NULL;
	return true;
}

bool SingleMatch::DelNearElem(vector<MatchRst> &RstVec, int nDotDist)
{
	if (RstVec.size() < 1)
	{
		return false;
	}

	sort(RstVec.begin(), RstVec.end(), comp);
	vector<MatchRst> TmpRstVect;
	for (int i = 0; i < RstVec.size(); ++i)
	{
		for (int k = 0; k <= i; ++k)
		{
			TmpRstVect.push_back(RstVec[k]);
		}

		for (int j = i + 1; j < RstVec.size(); ++j)
		{
			if (abs(RstVec[i].nCentX - RstVec[j].nCentX) > nDotDist || abs(RstVec[i].nCentY - RstVec[j].nCentY) > nDotDist)
			{
				TmpRstVect.push_back(RstVec[j]);
			}
		}

		RstVec.clear();
		RstVec.assign(TmpRstVect.begin(), TmpRstVect.end());
		TmpRstVect.clear();
	}

	return true;
}