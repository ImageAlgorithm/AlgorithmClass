#ifndef _STRUCTURE_HPP
#define _STRUCTURE_HPP
# include <string>
# include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//单模板匹配
struct MatchRst
{
	int nCentX;
	int nCentY;
	double dScore;
	MatchRst()
	{
		nCentX = 0;
		nCentY = 0;
		dScore = 0.0;
	}

	MatchRst(int nX, int nY, double dSc)
	{
		nCentX = nX;
		nCentY = nY;
		dScore = dSc;
	}
};

//降采样模糊匹配区域
struct MatchSection
{
	int nStartX; //区域左上角的x坐标
	int nStartY; //区域左上角的Y坐标
	int nEndX;  // 区域右下角的X坐标
	int nEndY;  // 区域右下角的Y坐标

	MatchSection()
	{
		nStartX = 0;
		nStartY = 0;
		nEndX = 0;
		nEndY = 0;
	}

	MatchSection(int startX, int startY, int endX, int endY)
	{
		nStartX = startX;
		nStartY = startY;
		nEndX = endX;
		nEndY = endY;
	}
};

//多模板匹配
struct TmpInfo
{
	int nTmpWidth;  //原模板宽度
	int nTmpHeight; //原模板高度
	int nPyrCount;   // 降采样次数
	string tmpname;   // 模板名
	vector<double> TmpSrcData;   // 模板没降采样的边缘数据
	vector<double> TmpPyrData;   // 模板降采样的边缘数据
};

struct InputImagInfo
{
	Mat InputImg;    // 原图数据
	Mat InputSobelDx;   // 原图X方向Sobel数据
	Mat InputSobelDy;   // 原图Y方向Sobel数据
};

struct Result
{
	int nCentX;		//模板识别结果X坐标
	int nCentY;		//模板识别结果的y坐标
	string targetname;	//模板名称
	Result()
	{
		nCentX = 0;
		nCentY = 0;
		targetname = "";
	}
	Result(int nX, int nY, string name)
	{
		nCentX = nX;
		nCentY = nY;
		targetname = name;
	}
};

//滑动验证码坐标点类型
struct POINT
{
	int x;
	int y;
	POINT()
	{
		x = 0;
		y = 0;
	}
	POINT(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};


struct PosLength
{
	int nPos;
	int nLength;
	int nStartPos;
};

#endif   // _STRUCTURE_HPP
