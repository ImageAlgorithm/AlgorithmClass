#include "SlidingCode.h"
#include "SingleMatch.h"


SlidingCode::SlidingCode()
{
	nError1 = 5;
	nMinDist = 3;
	nMinXDist = 10;
}


SlidingCode::~SlidingCode()
{
}

//************************************
// Method:    GetRowDotNum    边缘水平直线部分统计
// FullName:  HuaDongYZMFind::GetRowDotNum
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: Mat InputImg
// Parameter: int nDotDist                      最小间隔点数
// Parameter: vector<PosLength> &LengthList     水平边缘
//************************************
bool SlidingCode::GetRowDotNum(Mat InputImg, int nDotDist, vector<PosLength> &LengthList)
{
	if (InputImg.empty() || nDotDist < 1)
	{
		cout << "Input image is empty" << endl;
		return false;
	}

	// 统计每行的像素数
	vector<POINT> ptList;
	for (int i = 0; i < InputImg.rows; ++i)
	{
		uchar *pucData = InputImg.ptr<uchar>(i);
		for (int j = 0; j < InputImg.cols; ++j)
		{
			if (pucData[j] == 255)
			{
				POINT pt;
				pt.x = j;
				pt.y = i;
				ptList.push_back(pt);
			}
		}
	}

	if (ptList.size() < 1)
	{
		return false;
	}

	//获得每行的点数
	int nCount = 0;
	int nStartPos = 0;
	for (size_t i = 0; i < ptList.size() - 1; ++i)
	{
		if (i == 0)
		{
			nStartPos = ptList[0].x;
		}
		if (ptList[i + 1].y == ptList[i].y)
		{
			int nErr = abs(ptList[i + 1].x - ptList[i].x);
			if (nErr <= nDotDist)  // 5  3
			{
				nCount++;
			}
			else  // Y相等，断开的X区域，存每段区域
			{
				nCount++;
				PosLength lgt;
				lgt.nPos = ptList[i].y;
				lgt.nLength = nCount;
				lgt.nStartPos = nStartPos;
				LengthList.push_back(lgt);
				nCount = 0;
				nStartPos = ptList[i + 1].x;
			}
		}
		else   // Y 不相等，换行存上行
		{
			nCount++;  // add myself
			PosLength lgt;
			lgt.nPos = ptList[i].y;
			lgt.nLength = nCount;
			lgt.nStartPos = nStartPos;
			LengthList.push_back(lgt);
			nCount = 0;
			nStartPos = ptList[i + 1].x;
		}
		if (i == ptList.size() - 2) // 保存最后一行的点数
		{
			nCount++;  // add myself
			PosLength lgt;
			lgt.nPos = ptList[i].y;
			lgt.nLength = nCount;
			lgt.nStartPos = nStartPos;
			LengthList.push_back(lgt);
		}
	}

	return true;
}

//************************************
// Method:    GetColDotNum  垂直边缘直线部分统计
// FullName:  HuaDongYZMFind::GetColDotNum
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: Mat InputImg
// Parameter: int nDotDist                         最小间隔点数 
// Parameter: vector<PosLength> &ColLengthList     水平边缘
//************************************
bool SlidingCode::GetColDotNum(Mat InputImg, int nDotDist, vector<PosLength> &ColLengthList)
{
	if (InputImg.empty() || nDotDist < 1)
	{
		cout << "Input image is empty" << endl;
		return false;
	}

	vector<Point> ColPtList;
	for (int i = 0; i < InputImg.cols; ++i)
	{
		for (int j = 0; j < InputImg.rows; ++j)
		{
			if (InputImg.at<uchar>(j, i) == 255)
			{
				Point pt;
				pt.x = i;
				pt.y = j;
				ColPtList.push_back(pt);
			}
		}
	}

	if (ColPtList.size() < 1)
	{
		return false;
	}

	// 获得每列的点数
	int nCount = 0;
	int nStartPos = 0;
	for (size_t i = 0; i < ColPtList.size() - 1; ++i)
	{
		if (i == 0)
		{
			nStartPos = ColPtList[0].y;
		}
		if (ColPtList[i + 1].x == ColPtList[i].x)
		{
			int nErr = abs(ColPtList[i + 1].y - ColPtList[i].y);
			if (nErr <= nDotDist)  // 3
			{
				nCount++;
			}
			else  // X相等，断开的Y区域，存每段区域
			{
				nCount++;
				PosLength lgt;
				lgt.nPos = ColPtList[i].x;
				lgt.nLength = nCount;
				lgt.nStartPos = nStartPos;
				ColLengthList.push_back(lgt);
				nCount = 0;
				nStartPos = ColPtList[i + 1].y;
			}
		}
		else   // X 不相等，换列存上列
		{
			nCount++;  // add myself
			PosLength lgt;
			lgt.nPos = ColPtList[i].x;
			lgt.nLength = nCount;
			lgt.nStartPos = nStartPos;
			ColLengthList.push_back(lgt);
			nCount = 0;
			nStartPos = ColPtList[i + 1].y;
		}
		if (i == ColPtList.size() - 2) // 保存最后一列数据
		{
			nCount++;  // add myself
			PosLength lgt;
			lgt.nPos = ColPtList[i].x;
			lgt.nLength = nCount;
			lgt.nStartPos = nStartPos;
			ColLengthList.push_back(lgt);

		}
	}

	return true;
}

//************************************
// Method:    MergeSameRow  合并同一行多段边缘
// FullName:  HuaDongYZMFind::MergeSameRow
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: vector<PosLength> CandiList      右滑块的水平边缘
// Parameter: Mat InputImg
// Parameter: int nArcLength                    滑动块的弧长度
// Parameter: vector<PosLength> &RstCandiList   合并后的边缘
//************************************
bool SlidingCode::MergeSameRow(vector<PosLength> CandiList, Mat InputImg, int nArcLength, vector<PosLength> &RstCandiList)
{
	if (CandiList.size() < 1 || InputImg.empty())
	{
		return false;
	}

	bool bTopMerge = false;
	bool bBtmMerge = false;
	int nError = 5;
	for (size_t i = 0; i < CandiList.size() - 1; ++i)
	{
		if (CandiList[i].nPos == CandiList[i + 1].nPos
			&& CandiList[i].nPos <= InputImg.rows / 4) // Top长度合并
		{
			if (i + 2 < CandiList.size() - 1 && CandiList[i].nPos == CandiList[i + 2].nPos) // Top 3段
			{
				int nLength = CandiList[i].nStartPos + CandiList[i].nLength + nArcLength;
				if (nLength > CandiList[i + 1].nStartPos
					|| CandiList[i + 1].nStartPos - nLength <= nError)
				{
					PosLength tmp;
					tmp.nLength = CandiList[i].nLength + CandiList[i + 1].nLength;
					tmp.nPos = CandiList[i].nPos;
					tmp.nStartPos = CandiList[i + 1].nStartPos;
					RstCandiList.push_back(tmp);
					i += 2;
					bTopMerge = true;
				}
			}
			else  // 2段
			{
				int nLength = CandiList[i].nStartPos + CandiList[i].nLength + nArcLength;
				if (nLength > CandiList[i + 1].nStartPos
					|| CandiList[i + 1].nStartPos - nLength <= nError)
				{
					PosLength tmp;
					tmp.nLength = CandiList[i].nLength + CandiList[i + 1].nLength;
					tmp.nPos = CandiList[i].nPos;
					tmp.nStartPos = CandiList[i].nStartPos;
					RstCandiList.push_back(tmp);
					i++;
					bTopMerge = true;
				}
			}
		}
		else if (CandiList[i].nPos == CandiList[i + 1].nPos
			&& CandiList[i].nPos >= 3 * InputImg.rows / 4) // Btm长度合并
		{
			if (i + 2 < CandiList.size() - 1 && CandiList[i].nPos == CandiList[i + 2].nPos)  // 3段相同的
			{
				int nLength = CandiList[i].nStartPos + CandiList[i].nLength + nArcLength;
				if (nLength > CandiList[i + 1].nStartPos
					|| CandiList[i + 1].nStartPos - nLength <= nError)
				{
					PosLength tmp;
					tmp.nLength = CandiList[i].nLength + CandiList[i + 1].nLength;
					tmp.nPos = CandiList[i].nPos;
					tmp.nStartPos = CandiList[i + 1].nStartPos;
					RstCandiList.push_back(tmp);
					i += 2;
					bBtmMerge = true;
				}
			}
			else  // 2段
			{
				int nLength = CandiList[i].nStartPos + CandiList[i].nLength + nArcLength;
				if (nLength > CandiList[i + 1].nStartPos
					|| CandiList[i + 1].nStartPos - nLength <= nError)
				{
					PosLength tmp;
					tmp.nLength = CandiList[i].nLength + CandiList[i + 1].nLength;
					tmp.nPos = CandiList[i].nPos;
					tmp.nStartPos = CandiList[i].nStartPos;
					RstCandiList.push_back(tmp);
					i++;
					bBtmMerge = true;
				}
			}
		}
		else
		{
			// one by one copy data
			PosLength tmp;
			tmp.nLength = CandiList[i].nLength;
			tmp.nPos = CandiList[i].nPos;
			tmp.nStartPos = CandiList[i].nStartPos;
			RstCandiList.push_back(tmp);
		}
	}

	if (!bBtmMerge) // 除了bottom merge，其他情况都copy last data
	{
		int nLastNum = CandiList.size() - 1;
		PosLength tmp;
		tmp.nLength = CandiList[nLastNum].nLength;
		tmp.nPos = CandiList[nLastNum].nPos;
		tmp.nStartPos = CandiList[nLastNum].nStartPos;
		RstCandiList.push_back(tmp);
	}

	return true;
}

//************************************
// Method:    GetLeftHuaKuaiTopBtm   获得左侧滑块的Top、Btm位置
// FullName:  HuaDongYZMFind::GetLeftHuaKuaiTopBtm
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: Mat inputImg
// Parameter: int nHuaDongKuaiWidth     滑动块的宽度
// Parameter: int nHuaDongKuaiHeight    滑动块的高度
// Parameter: int &nHuaKuaiTop          滑块的Top位置
// Parameter: int &nHuaKuaiBtm          滑块的Btm位置
//************************************
bool SlidingCode::GetLeftHuaKuaiTopBtm(Mat inputImg, int nHuaDongKuaiWidth, int nHuaDongKuaiHeight,
	                                      int &nHuaKuaiTop, int &nHuaKuaiBtm)
{
	if (inputImg.empty() || nHuaDongKuaiWidth <= 0 || nHuaDongKuaiHeight <= 0)
	{
		return false;
	}

	Mat InputGrayImg, InputEdgeImg, HoughLineImg;
	cvtColor(inputImg, InputGrayImg, CV_BGR2GRAY);
	Canny(InputGrayImg, InputEdgeImg, 50, 220, 3);

	vector<Vec4i> mylines;
	HoughLineImg = Mat::zeros(Size(InputGrayImg.cols, InputGrayImg.rows), CV_8UC1);

	int anXPos[1024] = { 0 };
	int anYPos[1024] = { 0 };
	int nXCount = 0;
	int nYCount = 0;
	// 参数可变：背景复杂例如头条、哔哩哔哩的图参数为：20、8、5；
	HoughLinesP(InputEdgeImg, mylines, 0.5, CV_PI / 180, 20, 8, 5); // Left:20, 8, 5  QQYZM:30, 10, 5
	for (size_t i = 0; i < mylines.size(); ++i)
	{
		Vec4i l = mylines[i];
		line(HoughLineImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, LINE_AA);
		anXPos[nXCount++] = l[0];
		anYPos[nYCount++] = l[1];
		anXPos[nXCount++] = l[2];
		anYPos[nYCount++] = l[3];
	}

	//imwrite("Line.bmp", HoughLineImg);

	Mat LeftHoughLineBinImg;
	threshold(HoughLineImg, LeftHoughLineBinImg, 128, 255, CV_THRESH_BINARY);

	bool bRev = false;
	int nError = 5;
	if (nHuaKuaiBtm - nHuaKuaiTop >= nHuaDongKuaiHeight + 2 * nError
		|| (nHuaKuaiBtm - nHuaKuaiTop) < nHuaDongKuaiHeight / 2)
	{
		vector<PosLength> LeftLengthList;
		vector<PosLength> LeftCandiTopBtm;
		bRev = GetRowDotNum(LeftHoughLineBinImg, nMinXDist, LeftLengthList);
		for (size_t i = 0; i < LeftLengthList.size(); ++i)
		{
			if (LeftLengthList[i].nLength >= nHuaDongKuaiWidth / 5)
			{
				PosLength tmp;
				tmp.nLength = LeftLengthList[i].nLength;
				tmp.nPos = LeftLengthList[i].nPos;
				tmp.nStartPos = LeftLengthList[i].nStartPos;
				LeftCandiTopBtm.push_back(tmp);
			}
		}

		// 获得与滑块高最接近的上下边界位置,add且至少有一个宽度与滑块接近
		if (LeftCandiTopBtm.size() > 0)
		{
			int nMinError = 9999;
			for (size_t i = 0; i < LeftCandiTopBtm.size(); ++i)
			{
				for (size_t j = i + 1; j < LeftCandiTopBtm.size(); ++j)
				{
					int nDist = abs(LeftCandiTopBtm[i].nPos - LeftCandiTopBtm[j].nPos) + 1;
					int nWidthErr1 = abs(LeftCandiTopBtm[i].nLength - nHuaDongKuaiWidth);
					int nWidthErr2 = abs(LeftCandiTopBtm[j].nLength - nHuaDongKuaiWidth);
					if (abs(nDist - nHuaDongKuaiHeight) < nMinError
						&& (nWidthErr1 <= 4 * nError || nWidthErr2 <= 4 * nError))
					{
						nMinError = abs(nDist - nHuaDongKuaiHeight);
						nHuaKuaiTop = LeftCandiTopBtm[i].nPos;
						nHuaKuaiBtm = LeftCandiTopBtm[j].nPos;
					}
				}
			}
		}

		// Top、Btm没找到时，降低范围，2018-10-12 add
		if (nHuaKuaiTop == 0 && nHuaKuaiBtm == 0)
		{
			int nMinError = 9999;
			for (size_t i = 0; i < LeftCandiTopBtm.size(); ++i)
			{
				for (size_t j = i + 1; j < LeftCandiTopBtm.size(); ++j)
				{
					int nDist = abs(LeftCandiTopBtm[i].nPos - LeftCandiTopBtm[j].nPos) + 1;
					//int nWidthErr1 = abs(LeftCandiTopBtm[i].nLength - nHuaDongKuaiWidth);
					//int nWidthErr2 = abs(LeftCandiTopBtm[j].nLength - nHuaDongKuaiWidth);
					if (abs(nDist - nHuaDongKuaiHeight) < nMinError
						&& (LeftCandiTopBtm[i].nLength >= 3 * nError || LeftCandiTopBtm[j].nLength >= 3 * nError))
					{
						nMinError = abs(nDist - nHuaDongKuaiHeight);
						nHuaKuaiTop = LeftCandiTopBtm[i].nPos;
						nHuaKuaiBtm = LeftCandiTopBtm[j].nPos;
					}
				}
			}
		}
	}

	return true;
}

//************************************
// Method:    GetCandiList  获得符合点数要求的边缘水平直线
// FullName:  HuaDongYZMFind::GetCandiList
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: vector<PosLength> RowLengthList    边缘水平直线
// Parameter: Mat RightDetectEdgeImg             右滑块边缘图像
// Parameter: int nHuaDongKuaiWidth              滑动块宽度
// Parameter: vector<PosLength> &CandiList       符合点数要求的边缘水平直线
//************************************
bool SlidingCode::GetCandiList(vector<PosLength> RowLengthList, Mat RightDetectEdgeImg, int nHuaDongKuaiWidth,
	                              vector<PosLength> &CandiList)
{
	if (RowLengthList.size() < 1 || RightDetectEdgeImg.empty() || nHuaDongKuaiWidth <= 0)
	{
		return false;
	}

	// 添加前半部分长度符合要求的行
	int nError = 5;
	for (size_t i = 0; i < RowLengthList.size(); ++i)
	{
		if (RowLengthList[i].nPos <= RightDetectEdgeImg.rows / 4)
		{
			if (RowLengthList[i].nLength >= nHuaDongKuaiWidth / 4
				&& RowLengthList[i].nLength <= nHuaDongKuaiWidth + 2 * nError
				&& RowLengthList[i].nStartPos > nError)
			{
				PosLength tmp;
				tmp.nLength = RowLengthList[i].nLength;
				tmp.nPos = RowLengthList[i].nPos;
				tmp.nStartPos = RowLengthList[i].nStartPos;
				CandiList.push_back(tmp);
			}
		}
	}

	// 添加后半部分符合要求的行
	for (size_t i = 0; i < RowLengthList.size(); ++i)
	{
		if (RowLengthList[i].nPos >= 3 * RightDetectEdgeImg.rows / 4)
		{
			if (RowLengthList[i].nLength >= nHuaDongKuaiWidth / 4
				&& RowLengthList[i].nLength <= nHuaDongKuaiWidth + 2 * nError
				&& RowLengthList[i].nStartPos > nError)
			{
				PosLength tmp1;
				tmp1.nLength = RowLengthList[i].nLength;
				tmp1.nPos = RowLengthList[i].nPos;
				tmp1.nStartPos = RowLengthList[i].nStartPos;
				CandiList.push_back(tmp1);
			}
		}
	}

	return true;
}

//************************************
// Method:    GetMinX  获取终止位置的x坐标
// FullName:  HuaDongYZMFind::GetMinX
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: vector<PosLength> LastCandiList   边缘水平直线
// Parameter: int nHuaDongKuaiWidth             滑动块宽度
// Parameter: int nHuaDongKuaiHeight            滑动块高度
// Parameter: int &nMinX                        终止位置的最小x坐标
//************************************
bool SlidingCode::GetMinX(vector<PosLength> LastCandiList, int nHuaDongKuaiWidth, int nHuaDongKuaiHeight, int &nMinX)
{
	if (LastCandiList.size() < 1 || nHuaDongKuaiWidth <= 0 || nHuaDongKuaiHeight <= 0)
	{
		return false;
	}

	int nMinStartErr = 9999;
	int nError = 5;
	for (size_t i = 0; i < LastCandiList.size(); ++i)
	{
		for (size_t j = i + 1; j < LastCandiList.size(); ++j)
		{
			int nEndPos1 = LastCandiList[i].nStartPos + LastCandiList[i].nLength;
			int nEndPos2 = LastCandiList[j].nStartPos + LastCandiList[j].nLength;
			int nMaxLength = max(LastCandiList[i].nLength, LastCandiList[j].nLength);
			int nStartPosError = abs(LastCandiList[i].nStartPos - LastCandiList[j].nStartPos);
			int nHeight = abs(LastCandiList[i].nPos - LastCandiList[j].nPos);
			if (nStartPosError <= 2 * nHuaDongKuaiWidth / 5 && nStartPosError < nMinStartErr
				&& abs(nHeight - nHuaDongKuaiHeight) <= 2 * nError)  // nStartPosError <= nHuaDongKuaiWidth / 2
			{
				nMinStartErr = nStartPosError;
				nMinX = (LastCandiList[i].nStartPos + LastCandiList[j].nStartPos) / 2;
				//nMinX = min(LastCandiList[i].nStartPos, LastCandiList[j].nStartPos);
			}
			else if (abs(nEndPos1 - nEndPos2) <= 2 * nError
				&& abs(nHeight - nHuaDongKuaiHeight) <= 2 * nError
				&& abs(nMaxLength - nHuaDongKuaiWidth) <= 2 * nError)
			{
				nMinX = min(LastCandiList[i].nStartPos, LastCandiList[j].nStartPos);
				break;
			}
		}
	}

	// 如果没找到，就取高度符合要求的起点
	if (nMinX == 0)
	{
		int nMinHeight = 9999;
		for (size_t i = 0; i < LastCandiList.size(); ++i)
		{
			for (size_t j = i + 1; j < LastCandiList.size(); ++j)
			{
				int nHeight = abs(LastCandiList[i].nPos - LastCandiList[j].nPos);
				if (abs(nHeight - nHuaDongKuaiHeight) <= 2 * nError
					&& abs(nHeight - nHuaDongKuaiHeight) < nMinHeight)
				{
					nMinHeight = abs(nHeight - nHuaDongKuaiHeight);
					nMinX = min(LastCandiList[i].nStartPos, LastCandiList[j].nStartPos);
				}
			}
		}
	}

	return true;
}

//************************************
// Method:    myRotateAntiClockWise90  逆时针旋转90°
// FullName:  HuaDongYZMFind::myRotateAntiClockWise90
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: Mat &src   待旋转图像
//************************************
bool SlidingCode::myRotateAntiClockWise90(Mat &src)
{
	if (src.empty())
	{
		cout << "myRotateAntiClockWise90 param 'mat' is empty!";
		return false;
	}
	transpose(src, src);
	flip(src, src, 0);
	return true;
}

//************************************
// Method:    matRotateClockWise90  顺时针旋转90°
// FullName:  HuaDongYZMFind::matRotateClockWise90
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: Mat &src    待旋转图像
//************************************
bool SlidingCode::matRotateClockWise90(Mat &src)
{
	if (src.empty())
	{
		cout << "RorateMat src is empty!";
		return false;
	}
	// 矩阵转置
	transpose(src, src);
	//0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
	flip(src, src, 1);// 翻转模式，flipCode == 0垂直翻转（沿X轴翻转），flipCode>0水平翻转（沿Y轴翻转），flipCode<0水平垂直翻转（先沿X轴翻转，再沿Y轴翻转，等价于旋转180°）
	return true;
}

//************************************
// Method:    DoInspect
// FullName:  HuaDongYZMFind::DoInspect
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: Mat &InputImg            输入图像
// Parameter: POINT StartPos           起始点坐标
// Parameter: POINT DetectRegLftPos    查找区域左上角坐标
// Parameter: POINT DetectRegRgtPos    查找区域右下角坐标
// Parameter: int nHuaDongKuaiWidth    滑动块的宽度
// Parameter: int nHuaDongKuaiHeight   滑动块的高度
// Parameter: int nArcLength           滑动块的弧长度
// Parameter: bool bRot                图像旋转标志
// Parameter: POINT &EndPos            终点位置
//************************************
bool SlidingCode::DoInspect(Mat InputImg, POINT StartPos, POINT DetectRegLftPos, POINT DetectRegRgtPos, int nHuaDongKuaiWidth,
	                           int nHuaDongKuaiHeight, int nArcLength, bool bRot, POINT &EndPos)
{
    if (InputImg.empty())
    {
		cout << "DoInspect输入图片为空！！";
        return false;
    }
	int nWidth = InputImg.cols;
	int nHeight = InputImg.rows;
	if (StartPos.x<0 || StartPos.y<0 || StartPos.x>nWidth || StartPos.y>nHeight)
	{
		cout << "参数StartPos参数错误！";
		return false;
	}
	if (DetectRegLftPos.x<0 || DetectRegLftPos.y<0 || DetectRegLftPos.x>nWidth || DetectRegLftPos.y>nHeight)
	{
		cout << "参数DetectRegLftPos参数错误！";
		return false;
	}
	if (DetectRegRgtPos.x<0 || DetectRegRgtPos.y<0 || DetectRegRgtPos.x>nWidth || DetectRegRgtPos.y>nHeight)
	{
		cout << "参数DetectRegRgtPos参数错误！";
		return false;
	}

	if (nHuaDongKuaiWidth >= nWidth || nHuaDongKuaiHeight >= nHeight || nHuaDongKuaiWidth <= 0 || nHuaDongKuaiHeight <= 0)
	{
		cout << "参数nHuaDongKuaiHeight或nHuaDongKuaiWidth参数错误！";
		return false;
	}
	if (nArcLength >= nHuaDongKuaiWidth || nArcLength >= nHuaDongKuaiHeight || nArcLength <= 0)
	{
		cout << "参数nArcLength参数错误！";
		return false;
	}

    if (bRot)
    {
        if (!(myRotateAntiClockWise90(InputImg)))
        {
            return false;
        }

        int ntmp = StartPos.x;
        StartPos.x = StartPos.y;
        StartPos.y = InputImg.rows - ntmp;

        ntmp = DetectRegLftPos.x;
        DetectRegLftPos.x = DetectRegLftPos.y;
        DetectRegLftPos.y = InputImg.rows - DetectRegRgtPos.x;

        DetectRegRgtPos.x = DetectRegRgtPos.y;
        DetectRegRgtPos.y = InputImg.rows - ntmp;

        ntmp = nHuaDongKuaiWidth;
        nHuaDongKuaiWidth = nHuaDongKuaiHeight;
        nHuaDongKuaiHeight = ntmp;
    }

    int nError2 = MIN(nHuaDongKuaiWidth, nHuaDongKuaiHeight) / 5 >= 20 ? 15 : 5; //偏差大小
    int nLeftRegRgt = StartPos.x + nHuaDongKuaiWidth / 2 + nError1;
    int nLeftRegWidth = nLeftRegRgt - DetectRegLftPos.x + 1;
    int nLeftRegHeight = DetectRegRgtPos.y - DetectRegLftPos.y + 1;
    int nRightRegWidth = DetectRegRgtPos.x - nLeftRegRgt;

	// 对变量非法结果进行判断，防止程序崩溃
	if (nLeftRegRgt < 0 || nLeftRegRgt > InputImg.cols || nLeftRegWidth < 0 || nLeftRegWidth > InputImg.cols
		|| nLeftRegHeight < 0 || nLeftRegHeight > InputImg.rows || nRightRegWidth < 0 || nRightRegWidth > InputImg.cols)
	{
		return false;
	}

	// 滑块贴近时，截取右侧ROI会少左边界
	if (DetectRegLftPos.x < 0 || nLeftRegRgt - 2 * nError2 < 0 || DetectRegLftPos.x > InputImg.cols || DetectRegLftPos.y < 0 || DetectRegLftPos.y > InputImg.rows
		|| DetectRegLftPos.x + nLeftRegWidth>InputImg.cols || DetectRegLftPos.y + nLeftRegHeight > InputImg.rows || nLeftRegRgt - 2 * nError2 + nRightRegWidth > InputImg.cols)
	{
		return false;
	}

    Mat leftROIImg = InputImg(Rect(DetectRegLftPos.x, DetectRegLftPos.y, nLeftRegWidth, nLeftRegHeight));
    Mat RightROIImg = InputImg(Rect(nLeftRegRgt - 2 * nError2, DetectRegLftPos.y, nRightRegWidth, nLeftRegHeight));

    int nHuaKuaiTop = 0;
    int nHuaKuaiBtm = 0;

    bool bRev = false;
    // 获得左侧滑块的Top、Btm位置
    bRev = GetLeftHuaKuaiTopBtm(leftROIImg, nHuaDongKuaiWidth, nHuaDongKuaiHeight, nHuaKuaiTop, nHuaKuaiBtm);
    if (!bRev)
    {
        return false;
    }

    // 如果上下边界没找到，直接退出
    if (0 == nHuaKuaiTop && 0 == nHuaKuaiBtm)
    {
        return false;
    }

    // 根据Top、Btm位置截图右侧滑块区域，并对右侧滑块区域进行彩色转灰度—Canny边缘检测
    Mat RightDetectImg = RightROIImg(Rect(0, nHuaKuaiTop - nError1, RightROIImg.cols, MIN(RightROIImg.rows- nHuaKuaiTop + nError1,nHuaKuaiBtm - nHuaKuaiTop + 2 * nError1)));

    Mat RightDetectGrayImg, RightDetectEdgeImg;
    cvtColor(RightDetectImg, RightDetectGrayImg, CV_BGR2GRAY);
    Canny(RightDetectGrayImg, RightDetectEdgeImg, 50, 220, 3);

    // 统计右侧滑块每行点数
    vector<PosLength> RowLengthList;
    bRev = GetRowDotNum(RightDetectEdgeImg, nMinDist, RowLengthList); // 1  3
    if (!bRev)
    {
        return false;
    }

    // 获得符合点数要求的行位置
    vector <PosLength> CandiList;
    bRev = GetCandiList(RowLengthList, RightDetectEdgeImg, nHuaDongKuaiWidth, CandiList);
    if (!bRev)
    {
        return false;
    }

    int nMinX = 0;
    EndPos.x = StartPos.x;
    if (CandiList.size() > 0)
    {
        // 将同一行有多段长度符合要求的进行合并
        vector<PosLength> LastCandiList;
        bRev = MergeSameRow(CandiList, RightDetectEdgeImg, nArcLength, LastCandiList);
        if (!bRev)
        {
            return false;
        }

        bRev = GetMinX(LastCandiList, nHuaDongKuaiWidth, nHuaDongKuaiHeight, nMinX);
        if (!bRev)
        {
            return false;
        }

        vector<PosLength> ColLengthList;
        bRev = GetColDotNum(RightDetectEdgeImg, nMinDist, ColLengthList);
        if (!bRev)
        {
            return false;
        }

        if (ColLengthList.size() < 1)
        {
            return false;
        }

        LastCandiList.clear();
        // 垂直方向只要点数大于滑块高的1/4或1/5即可
        for (size_t i = 0; i < ColLengthList.size(); ++i)
        {
            if (ColLengthList[i].nLength >= nHuaDongKuaiHeight / 5)
            {
                PosLength tmp;
                tmp.nLength = ColLengthList[i].nLength;
                tmp.nPos = ColLengthList[i].nPos;
                tmp.nStartPos = ColLengthList[i].nStartPos;
                LastCandiList.push_back(tmp);
            }
        }

        bool bFlag = false;
        for (size_t i = 0; i < LastCandiList.size(); ++i)
        {
            if (abs(LastCandiList[i].nPos - nMinX) <= nMinXDist/* 3 * nError1*/)
            {
                EndPos.x = nMinX + nHuaDongKuaiWidth / 2;
                bFlag = true;
                break;
            }
        }

        // 如果没有找到，用模板匹配的方法再次找
        if (!bFlag)
        {
            SingleMatch SM;
            vector<MatchRst> rst;
            Mat Tmp = InputImg(Rect(StartPos.x - nHuaDongKuaiWidth / 2 + nArcLength / 2,
                                    DetectRegLftPos.y + (nHuaKuaiTop + nHuaKuaiBtm) / 2 - nHuaDongKuaiHeight / 2 + nArcLength / 2,
                                    nHuaDongKuaiWidth - nArcLength, nHuaDongKuaiHeight - nArcLength));

            SM.DoInspect(RightDetectImg, Tmp, rst, 1, 0.5);
			if (rst.size() < 1)
			{
				return false;
			}
			MatchRst Res = rst[0];
            EndPos.x = Res.nCentX + (nLeftRegRgt - 2 * nError2 + 1);
        }
        else
        {
            Mat Tmp = InputImg(Rect(StartPos.x - nHuaDongKuaiWidth / 2 + nArcLength / 2,
                                    DetectRegLftPos.y + (nHuaKuaiTop + nHuaKuaiBtm) / 2 - nHuaDongKuaiHeight / 2 + nArcLength / 2,
                                    nHuaDongKuaiWidth - nArcLength, nHuaDongKuaiHeight - nArcLength));

            EndPos.x += (nLeftRegRgt - 2 * nError2 + 1);
        }

        if (bRot)
        {
            EndPos.y = EndPos.x;
            EndPos.x = InputImg.rows - StartPos.y;
            int nTmp = StartPos.x;
            StartPos.x = InputImg.rows - StartPos.y;
            StartPos.y = nTmp;

            Point ptCent;
            ptCent.x = EndPos.x;
            ptCent.y = EndPos.y;
            if (!matRotateClockWise90(InputImg))
            {
                return false;
            }
            circle(InputImg, ptCent, 3, Scalar(0, 0, 255), 2, 8, 0);  // 画出最后结果
        }
        else
        {
            EndPos.y = StartPos.y;

            Point ptCent;
            ptCent.x = EndPos.x;
            ptCent.y = EndPos.y;
            circle(InputImg, ptCent, 3, Scalar(0, 0, 255), 2, 8, 0);  // 画出最后结果
        }
    }
    else
    {
        return false;
    }

    return true;
}
