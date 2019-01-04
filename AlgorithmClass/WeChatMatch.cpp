/***************************************************************************
* 微信页面识别算法文件，实现了WeChat类的所有函数
****************************************************************************/

#include "WeChatMatch.h"

WeChat::WeChat()
{
}
WeChat::~WeChat()
{
}

//************************************
// Method:    NewsExist 判断是否有新消息
// FullName:  WeChat::NewsExist
// Access:    public 
// Returns:   bool 是否识别成功
// Qualifier:
// Parameter: Mat InputImg 待识别图片
// Parameter: bool & bHave 识别结果
//************************************
bool WeChat::NewsExist(Mat InputImg, bool &bHave)
{
    if (InputImg.empty())
    {
        cout << "2InputImg is empty!" << endl;
        bHave = false;
        return false;
    }

    Mat InputRoiImg = InputImg(Rect(0, InputImg.rows * 0.87, InputImg.cols / 4, InputImg.rows * 0.07));

    Mat blurImg;
    medianBlur(InputRoiImg, blurImg, 3);//中值滤波去除噪点

    cvtColor(blurImg, blurImg, CV_BGR2GRAY);  //转灰度

    /*霍夫圆检测*/
    vector<Vec3f> pcircles; //数组
	//霍夫圆检测，8位输入，三维数组输出，霍夫梯度，尺度，半径最短间隔，canny低阈值，圆心累加阈值，最短半径，最长半径
    HoughCircles(blurImg, pcircles, CV_HOUGH_GRADIENT, 1, 5, 150, 30, 10, 35); 
    if (pcircles.size() < 1)
    {
        bHave = false;
        return false;
    }
    for (int k = 0; k < pcircles.size(); ++k)
    {
        Vec3f cc = pcircles[k];
        int nCirPosX = cc[0];
        int nCirPosY = cc[1];
        int nDp = cc[2];

        if(nCirPosX > 170)
		{
            bHave = true;
            return true;
        }
    }

   return true;
}

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
bool WeChat::MatchNewsPosition(Mat InputImg, Mat TmpImg, vector<MatchRst> &rst)
{
	if (InputImg.empty() || TmpImg.empty() )
	{
		cout << "图片为空" << endl;
		return false;
	}
	Mat InputRoiImg = InputImg(Rect(InputImg.cols / 9, 0, InputImg.cols / 5 - InputImg.cols / 9, InputImg.rows * 8 / 9));
	bool bRes = this->SM.DoInspect(InputRoiImg, TmpImg, rst, 20, 0.8);
	if (!bRes)
	{
		return false;
	}

    int nTemp = InputImg.cols/9 + 45;
    for(int i = 0; i < rst.size(); i++)
	{
        rst[i].nCentX += nTemp;
    }
	if (rst.size() > 0)
	{
		sort(rst.begin(), rst.end(), comp);
	}

	return true;
}

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
bool WeChat::MatchButtonPos(Mat InputImg, Mat TmpImg, MatchRst &res)
{
	if (InputImg.empty() || TmpImg.empty())
	{
		return false;
	}
	Mat InputRoiImg = InputImg(Rect(0, 0, InputImg.cols, InputImg.rows));
	vector<MatchRst> rst;
	bool bRes = this->SM.DoInspect(InputRoiImg, TmpImg, rst, 1, 0.8);
	if(bRes && rst.size()>0)
	{
		res = rst[0];
	}
    
	return bRes;
}

//************************************
// Method:    comp：返回a的y坐标是否小于b的y坐标
// FullName:  WeChat::comp
// Access:    public static 
// Returns:   bool
// Qualifier:
// Parameter: MatchRst a	
// Parameter: MatchRst b
//************************************
bool WeChat::comp(MatchRst a, MatchRst b)
{
    return (a.nCentY) < (b.nCentY);
}