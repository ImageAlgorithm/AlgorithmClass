/***************************************************************************
* 创建多模板文件（.mdl)程序的头文件，声明了CreateModel类
****************************************************************************/

#ifndef _CREATE_MULTI_TEMPLATE_MATCH_H
#define _CREATE_MULTI_TEMPLATE_MATCH_H
#include "structure.h"
#include <iomanip>
#include <io.h>
#include <fstream>

#define PATH_DELIMITER '\\'

//************************************
// 创建多模板文件(.mdl)的类
//************************************
class CreateModel
{
public:

	//************************************
	// Method:    GetSobelEdge 得到图片的X、Y方向图
	// FullName:  CreateModel::GetSobelEdge
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat InputImg	原图像
	// Parameter: Mat & InputSobelDx  图像的SobelX方向图
	// Parameter: Mat & InputSobelDy  图像的SobelX方向图
	//************************************
	bool GetSobelEdge(Mat InputImg, Mat& InputSobelDx, Mat& InputSobelDy);

	//************************************
	// Method:    CreateDoubleMatrix：创建二维数组
	// FullName:  CreateModel::CreateDoubleMatrix
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: double * *  & matrix：二维数组的头指针  
	// Parameter: int height：二维数组的高度
	// Parameter: int width：二维数组的宽度
	//************************************
	bool CreateDoubleMatrix(double** &matrix, int nHeight, int nWidth);

	//************************************
	// Method:    DeleteDoubleMatrix：删除二维数组
	// FullName:  CreateModel::DeleteDoubleMatrix
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: double * * & matrix：二维数组的头指针
	// Parameter: int height：二维数组的高度
	//************************************
	bool DeleteDoubleMatrix(double **&matrix, int nHeight);

	//************************************
	// Method:    CreateMatchEdgeModel：创建模板的模型数据
	// FullName:  CreateModel::CreateMatchEdgeModel
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: Mat templateSobelXImg：模板的X方向图（Sobel）
	// Parameter: Mat templateSobelYImg：模板的Y方向图（Sobel）
	// Parameter: double maxThre：最大阈值
	// Parameter: double minThre：最小阈值
	// Parameter: int & nofEdgePiex：边缘像素数
	// Parameter: CvPoint * pPositionEdge：存储边缘坐标
	// Parameter: double * pEdgeMagnitude：存储边缘梯度大小的倒数
	// Parameter: double * pEdgeMX：存储x方向边缘梯度
	// Parameter: double * pEdgeMY：存储y方向边缘梯度
	//************************************
	bool CreateMatchEdgeModel(Mat templateSobelXImg, Mat templateSobelYImg, double maxThre, double minThre, int& nofEdgePiex,
		CvPoint *pPositionEdge, double *pEdgeMagnitude, double *pEdgeMX, double *pEdgeMY);

	//************************************
	// Method:    CreateDirectory：路径下创建新文件夹
	// FullName:  CreateModel::CreateDirectory
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const string folder：文件路径
	//************************************
	bool CreateDirectory(const string folder);

	//************************************
	// Method:    Write：将模板数据写入mdl文件
	// FullName:  CreateModel::Write
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const string strFolder：文件路径
	// Parameter: const string strFileName：文件名
	// Parameter: int nOfEdgePiex：边缘特征点的个数
	// Parameter: CvPoint * pPositionEdge：边缘特征点的坐标
	// Parameter: double * pdEdgeMagnitude：边缘特征点的梯度的倒数
	// Parameter: double * pdEdgeMX：X方向的边缘梯度
	// Parameter: double * pdEdgeMY：Y方向的边缘梯度
	// Parameter: int nTmpWidth：模板的宽度
	// Parameter: int nTmpHeight：模板的高度
	// Parameter: int nPyrCount：降采样的次数
	// Parameter: bool IsSrc：是否是原图
	//************************************
	bool Write(const string strFolder, const string strFileName, int nOfEdgePiex, CvPoint *pPositionEdge, double *pdEdgeMagnitude,
		double *pdEdgeMX, double *pdEdgeMY, int nTmpWidth, int nTmpHeight, int nPyrCont = 0, bool IsSrc = true);

	//************************************
	// Method:    listFiles函数其实在利用递归，这个函数不仅可以找目录中的文件，还可以找到目录下每一层的文件
	// 	在大多数情况下并不需要区分是遍历目录下还是遍历目录中，因为目录是我们自己创建的，要遍历的路径也是自己输入
	// 	所以我们完全可以把这个当做遍历目录中文件的函数来用目录中的所有图片(到每一级目录)
	// FullName:  CreateModel::listFiles
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const char * pDir：路径
	// Parameter: vector<string> & files：得到的带路径的文件名
	//************************************
	bool listFiles(const char *pDir, vector<string> &files);

	//************************************
	// Method:    is_dir：判断文件属性是目录还是文件
	// FullName:  CreateModel::is_dir
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: int attrib：文件的一个属性
	//************************************
	bool is_dir(int attrib);

	//************************************
	// Method:    show_error：显示删除失败原因
	// FullName:  CreateModel::show_error
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * file_name：
	//************************************
	void show_error(const char *file_name = NULL);

	//************************************
	// Method:    get_file_path：获取文件路径
	// FullName:  CreateModel::get_file_path
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const char * path
	// Parameter: const char * file_name
	// Parameter: char * file_path
	//************************************
	bool get_file_path(const char *path, const char *file_name, char *file_path);

	//************************************
	// Method:    deletefile：递归搜索目录中文件并删除
	// FullName:  CreateModel::deletefile
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const char * path
	//************************************
	bool deletefile(const char *path);

	//************************************
	// Method:    pyr：获取降采样次数
	// FullName:  CreateModel::pyr
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Parameter: int tmpheight：模板的高度
	// Parameter: int tmpwidth：模板的宽度
	//************************************
	int pyr(int tmpheight, int tmpwidth);

private:
	const int nMaxPyCnt = 3;    //最大降采样次数
	const int nMinTmpSize = 15; //降采样后模板的最小的宽或高
};


#endif
