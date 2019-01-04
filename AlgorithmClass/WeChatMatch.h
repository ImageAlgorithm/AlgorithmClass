/***************************************************************************
*微信页面识别算法的头文件，声明了WeChat类
****************************************************************************/

#ifndef _WECHATMATCH_H
#define _WECHATMATCH_H

#include <opencv2/opencv.hpp>
#include "SingleMatch.h"
#include "structure.h"

using namespace std;
using namespace cv;

//************************************
// 微信页面识别算法的类，类接口：
// NewsExist：识别是否有新消息
// MatchNewsPosition：识别聊天页面气泡的位置
// MatchButtonPos：识别某个按钮的位置
//************************************
class WeChat
{
public:
	WeChat();
	~WeChat();

	//************************************
	// Method:    NewsExist 判断是否有新消息
	// FullName:  WeChat::NewsExist
	// Access:    public 
	// Returns:   bool 是否识别成功
	// Qualifier:
	// Parameter: Mat InputImg 待识别图片
	// Parameter: bool & bHave 识别结果
	//************************************
	bool NewsExist(Mat InputImg, bool &bHave);

	//************************************
	// Method:    MatchNewsPosition 判断消息气泡的位置
	// FullName:  WeChat::MatchNewsPosition
	// Access:    public 
	// Returns:   bool 识别是否成功
	// Qualifier:
	// Parameter: Mat InputImg 待识别图片
	// Parameter: Mat TmpImg 模板
	// Parameter: vector<MatchRst> & rst 结果集
	//************************************
	bool MatchNewsPosition(Mat InputImg, Mat TmpImg, vector<MatchRst> &rst);

	//************************************
	// Method:    MatchCopyButton  判断复制按钮的位置    
	// FullName:  WeChat::MatchCopyButton
	// Access:    public 
	// Returns:   bool 识别是否成功
	// Qualifier:
	// Parameter: Mat InputImg 待识别图片
	// Parameter: Mat TmpImg 模板
	// Parameter: MatchRst & res 结果集
	//************************************
	bool MatchButtonPos(Mat InputImg, Mat TmpImg, MatchRst &res);

	//************************************
	// Method:    comp：返回a的y坐标是否小于b的y坐标
	// FullName:  WeChat::comp
	// Access:    public static 
	// Returns:   bool
	// Qualifier:
	// Parameter: MatchRst a	
	// Parameter: MatchRst b
	//************************************
	static bool comp(MatchRst a, MatchRst b);

private:
	SingleMatch SM;
};

#endif