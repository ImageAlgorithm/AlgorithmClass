#ifndef _MULTI_TEMPLATE_MATCH_H
#define _MULTI_TEMPLATE_MATCH_H

#include "structure.h"
#include "threadpool.h"

class MultiMatch
{
public:
	//************************************
	// Method:    getMultiResult：得到识别结果
	// FullName:  MultiMatch::getMultiResult
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat & InputImg：待识别的原图
	// Parameter: vector<string> files：带绝对路径的文件名
	// Parameter: double scoreThreshold：识别的阈值
	// Parameter: MultiMatch & sMM：MultiMatch类的对象
	//************************************
	bool getMultiResult(Mat &InputImg, vector<string> files, double scoreThreshold, MultiMatch &sMM);

	//************************************
	// Method:    listFiles：获取带绝对路径的文件名
	// FullName:  MultiMatch::listFiles
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const char * pDir：文件夹的路径（绝对路径）
	// Parameter: vector<string> & files：带绝对路径的文件名的集合
	//************************************
	bool ListFiles(const char *pDir, vector<string> &files);

	MultiMatch();
	~MultiMatch();

private:

	//************************************
	// Method:    ClearVector：主动清楚vector占据的内存
	// FullName:  MultiMatch::ClearVector
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: vector<T> & vt：被清除的vector
	//************************************
	template <class T>
	void ClearVector(vector<T>& vt);

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
	bool Read(const string path, int &nTmpWidth, int &nTmpHeight, int &nPyrCount, vector<double>& srcData, vector<double>& pyrData);

	//************************************
	// Method:    comp：比较两个对象的大小（按分数大小）
	// FullName:  MultiMatch::comp
	// Access:    private static 
	// Returns:   bool：返回a的分数是否大于b的分数
	// Qualifier:
	// Parameter: MatchRst a
	// Parameter: MatchRst b
	//************************************
	static bool comp(MatchRst a, MatchRst b);

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
	bool GetSobelEdge(Mat InputImg, Mat& InputSobelDx, Mat& InputSobelDy);

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
	bool CreateDoubleMatrix(double **&matrix, int nHeight, int nWidth);

	//************************************
	// Method:    DeleteDoubleMatrix：释放double类型的二维数组的空间
	// FullName:  MultiMatch::DeleteDoubleMatrix
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: double * * & matrix：double类型的二位指针
	// Parameter: int nHeight：二维数组的高度
	//************************************
	void DeleteDoubleMatrix(double **&matrix, int nHeight);

	//************************************
	// Method:    InvSqrt：计算开方的倒数
	// FullName:  MultiMatch::InvSqrt
	// Access:    private 
	// Returns:   float：返回结果
	// Qualifier:
	// Parameter: float x：需要开方的数据
	//************************************
	float InvSqrt(float x);

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
    bool FindGeoMatchModel( Mat srcSobelDx, Mat srcSobelDy, MatchSection matchSection, int nofEdgePiex,
                                        vector<CvPoint> pPositionEdge, vector<double> pEdgeMagnitude,
		                                vector<double> pEdgeMX,vector<double> pEdgeMY, double ScoreThreshold,
                                        double dGgreediness, vector<MatchRst> &RstVec);

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
	static void threadproc(InputImagInfo info, TmpInfo tmpinfo, double dScoreThreshold, MultiMatch &sMM);
public:
	static vector<Result> res;	//存放结果集合
};
#endif // !_MULTI_TEMPLATE_MATCH_H

