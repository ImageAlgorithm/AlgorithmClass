#ifndef SLIDINGCODE_H_
#define SLIDINGCODE_H_

#include <vector>
#include <opencv2/opencv.hpp>
#include "structure.h"
using namespace std;
using namespace cv;

class SlidingCode
{
public:

	SlidingCode();
	~SlidingCode();

	//************************************
	// Method:    DoInspect：得到滑动验证码的滑动位置
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
	bool DoInspect(Mat InputImg, POINT StartPos, POINT DetectRegLftPos, POINT DetectRegRgtPos, int nHuaDongKuaiWidth,
					int nHuaDongKuaiHeight, int nArcLength, bool bRot, POINT &EndPos);

private:

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
	bool GetRowDotNum(Mat InputImg, int nDotDist, vector<PosLength> &LengthList);

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
	bool GetColDotNum(Mat InputImg, int nDotDist, vector<PosLength> &ColLengthList);

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
	bool MergeSameRow(vector<PosLength> CandiList, Mat InputImg, int nArcLength, vector<PosLength> &RstCandiList);

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
	bool GetLeftHuaKuaiTopBtm(Mat InputImg, int nHuaDongKuaiWidth, int nHuaDongKuaiHeight, int &nHuaKuaiTop, int &nHuaKuaiBtm);

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
	bool GetCandiList(vector<PosLength> RowLengthList, Mat RightDetectEdgeImg, int nHuaDongKuaiWidth, vector<PosLength> &CandiList);

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
	bool GetMinX(vector<PosLength> LastCandiList, int nHuaDongKuaiWidth, int nHuaDongKuaiHeight, int &nMinX);

	//************************************
	// Method:    myRotateAntiClockWise90  逆时针旋转90°
	// FullName:  HuaDongYZMFind::myRotateAntiClockWise90
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat &src   待旋转图像
	//************************************
	bool myRotateAntiClockWise90(Mat &src);

	//************************************
	// Method:    matRotateClockWise90  顺时针旋转90°
	// FullName:  HuaDongYZMFind::matRotateClockWise90
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat &src    待旋转图像
	//************************************
	bool matRotateClockWise90(Mat &src);

private:
	int nError1;//左滑块图像误差
	int nMinDist;//统计点数的最小间隔
	int nMinXDist;//与nMinX的最小距离
};

#endif
