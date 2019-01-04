#ifndef _SINGLEMATCH_H_
#define _SINGLEMATCH_H_

#include <opencv2/opencv.hpp>
#include "structure.h"

using namespace std;
using namespace cv;

class SingleMatch
{
public:
	SingleMatch();
	~SingleMatch();

	//************************************
	// Method:    DoInspect 类接口
	// FullName:  SingleMatch::DoInspect
	// Access:    public 
	// Returns:   bool  函数是否执行成功
	// Qualifier:
	// Parameter: Mat InputSrcImg  源图像
	// Parameter: Mat InputTmpImg  模板图像
	// Parameter: vector<MatchRst> & RstVec  匹配结果(out)
	// Parameter: int nMatchNum  结果个数的上限
	// Parameter: double dScoreThreshold  最小分数阈值,如果图像边缘比较多，阈值设为0.5-0.6；如果边缘较少，阈值设为0.8左右
	//************************************
	bool DoInspect(Mat InputSrcImg, Mat InputTmpImg, vector<MatchRst> &RstVec, int nMatchNum, double dScoreThreshold);

private:

	//************************************
	// Method:    CreateDoubleMatrix 动态申请二维数组
	// FullName:  SingleMatch::CreateDoubleMatrix
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: double **&matrix    数组指针
	// Parameter: CvSize size         数组大小
	//************************************
	bool CreateDoubleMatrix(double **&matrix, CvSize size);

	//************************************
	// Method:    DeleteDoubleMatrix 释放动态申请的二维数组
	// FullName:  SingleMatch::DeleteDoubleMatrix
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: double **&matrix      数组指针
	// Parameter: CvSize size           数组大小
	//************************************
	bool DeleteDoubleMatrix(double **&matrix, CvSize size);

	//************************************
	// Method:    GetSobelEdge 获取图像的xy方向边缘梯度
	// FullName:  SingleMatch::GetSobelEdge
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat InputImg          输入图像
	// Parameter: Mat &InputSobelDx     图像的x方向边缘梯度
	// Parameter: Mat &InputSobelDy     图像的y方向边缘梯度
	//************************************
	bool GetSobelEdge(Mat InputImg, Mat &InputSobelDx, Mat &InputSobelDy);

	//************************************
	// Method:    pyr 计算降采样次数
	// FullName:  SingleMatch::pyr
	// Access:    private 
	// Returns:   int             降采样次数
	// Qualifier:
	// Parameter: int nTmpHeight  图像高度
	// Parameter: int nTmpWidth   图像宽度
	//************************************
	int pyr(int nTmpHeight, int nTmpWidth);

	//************************************
	// Method:    CreateMatchEdgeModel 创建匹配模板
	// FullName:  SingleMatch::CreateMatchEdgeModel
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat templateSobelXImg   模板x方向边缘图像
	// Parameter: Mat templateSobelYImg   模板y方向边缘图像
	// Parameter: double maxThre          最大阈值
	// Parameter: double minThre          最小阈值
	// Parameter: int &nofEdgePiex        边缘点数目
	// Parameter: CvPoint *pPositionEdge  边缘点坐标数组
	// Parameter: double *pEdgeMagnitude  边缘梯度倒数
	// Parameter: double *pEdgeMX         x方向边缘梯度
	// Parameter: double *pEdgeMY        y方向边缘梯度
	//************************************
	bool CreateMatchEdgeModel(Mat templateSobelXImg, Mat templateSobelYImg, double maxThre, double minThre, int &nofEdgePiex,
							  CvPoint *pPositionEdge, double *pEdgeMagnitude, double *pEdgeMX, double *pEdgeMY);

	//************************************
	// Method:    InvSqrt  快速计算开方倒数
	// FullName:  SingleMatch::InvSqrt
	// Access:    private 
	// Returns:   float     
	// Qualifier:
	// Parameter: float x
	//************************************
	float InvSqrt(float x);

	//************************************
	// Method:    FindGeoMatchModel  模板匹配
	// FullName:  SingleMatch::FindGeoMatchModel
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat srcSobelDx             源图像x方向边缘图像
	// Parameter: Mat srcSobelDy             源图像y方向边缘图像
	// Parameter: MatchSection matchsect     匹配区域
	// Parameter: size_t nofEdgePiex	     边缘点数目
	// Parameter: CvPoint *pPositionEdge     边缘点坐标数组
	// Parameter: double *pEdgeMagnitude     边缘梯度倒数
	// Parameter: double *pEdgeMX            x方向边缘梯度
	// Parameter: double *pEdgeMY            y方向边缘梯度
	// Parameter: double dScoreThreshold     最小分数阈值
	// Parameter: double dGgreediness        贪婪系数
	// Parameter: vector<MatchRst> &RstVec   匹配结果
	//************************************
	bool FindGeoMatchModel(Mat srcSobelDx, Mat srcSobelDy, MatchSection matchsect, size_t nofEdgePiex,
		CvPoint *pPositionEdge, double *pEdgeMagnitude, double *pEdgeMX, double *pEdgeMY,
		double dScoreThreshold, double dGgreediness, vector<MatchRst> &RstVec);

	//************************************
	// Method:    Match  模板匹配过程
	// FullName:  SingleMatch::Match
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat InputSrcImg               源图像
	// Parameter: Mat InputTmpImg               模板图像
	// Parameter: double dScoreThreshold        最小分数阈值
	// Parameter: int pyrcont                   降采样次数
	// Parameter: MatchSection matchsect        匹配区域
	// Parameter: vector<MatchRst> &RstVec      匹配结果
	//************************************
	bool Match(Mat InputSrcImg, Mat InputTmpImg, double dScoreThreshold, int pyrcont, MatchSection matchsect,
		vector<MatchRst> &RstVec);

	//************************************
	// Method:    comp 比较两个MatchRst对象的大小（根据分数）
	// FullName:  SingleMatch::comp
	// Access:    private static 
	// Returns:   bool
	// Qualifier:
	// Parameter: MatchRst a
	// Parameter: MatchRst b
	//************************************
	static bool comp(MatchRst a, MatchRst b);

	//************************************
	// Method:    DelNearElem  去除重复结果
	// FullName:  SingleMatch::DelNearElem
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: vector<MatchRst> &RstVec    匹配结果
	// Parameter: int nDotDist                       坐标间隔
	//************************************
	bool DelNearElem(vector<MatchRst> &RstVec, int nDotDist);

private:
	int		nMaxPyCnt;          //最大降采样次数
	int		nMinTmpSize;        //降采样后的最小模板大小
	double	maxThre, minThre;   //阈值
	double	dGreediness;        //贪婪系数
	int		nDispersion;        //离散度
};

#endif  // _SINGLEMATCH_H_



