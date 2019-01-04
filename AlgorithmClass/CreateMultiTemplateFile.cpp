/***************************************************************************
* 创建多模板文件（.mdl)程序文件，实现了CreateModel类的所有函数
****************************************************************************/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <iomanip>
#include <cstring>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <errno.h>
#include "CreateMultiTemplateMatch.h"

using namespace std;
using namespace cv;

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
bool CreateModel::GetSobelEdge(Mat InputImg, Mat& InputSobelDx, Mat& InputSobelDy)
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
// Method:    CreateDoubleMatrix：创建二维数组
// FullName:  CreateModel::CreateDoubleMatrix
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: double * *  & matrix：二维数组的头指针  
// Parameter: int height：二维数组的高度
// Parameter: int width：二维数组的宽度
//************************************
bool CreateModel::CreateDoubleMatrix(double** &matrix, int nHeight, int nWidth)
{
	if (nHeight <= 0 || nWidth <= 0)
	{
		return false;
	}

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
// Method:    DeleteDoubleMatrix：删除二维数组
// FullName:  CreateModel::DeleteDoubleMatrix
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: double * * & matrix：二维数组的头指针
// Parameter: int height：二维数组的高度
//************************************
bool CreateModel::DeleteDoubleMatrix(double **&matrix, int nHeight)
{
	if (nHeight <= 0)
	{
		return false;
	}

	for (int i = 0; i < nHeight; ++i)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
	matrix = NULL;

	return true;
}


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
bool CreateModel::CreateMatchEdgeModel(Mat templateSobelXImg, Mat templateSobelYImg, double dMaxThre, double dMinThre,
	int &nOfEdgePiex, CvPoint *pPositionEdge, double *pEdgeMagnitude, double *pEdgeMX, double *pEdgeMY)
{
	if (templateSobelXImg.empty() || templateSobelYImg.empty())
	{
		return false;
	}

	if (dMaxThre <= 0 || dMaxThre > 255 || dMinThre <= 0 || dMinThre > 255 || dMinThre >= dMaxThre)
	{
		return false;
	}
	if (pPositionEdge == NULL || pEdgeMagnitude == NULL || pEdgeMX == NULL || pEdgeMY == NULL)
	{
		return false;
	}

	int nTmpHeight = templateSobelXImg.rows;
	int nTmpWidth = templateSobelXImg.cols;

	int nCount = 0;

	int *pnOrients = new int[nTmpWidth * nTmpHeight];//存储方向
	if (NULL == pnOrients)
	{
		return false;
	}
	memset(pnOrients, 0, sizeof(nTmpWidth * nTmpHeight));

	double dDx = 0, dDy = 0;
	double dMag = 0, dDirection = 0;

	double dMaxGradient = -99999.99;

	bool bRev = false;
	double **pMagMat = NULL;
	bRev = CreateDoubleMatrix(pMagMat, nTmpHeight, nTmpWidth);
	if (!bRev)
	{
		delete[] pnOrients;
		pnOrients = NULL;
		return false;
	}

	const short *pSdx = NULL;
	const short *pSdy = NULL;
	for (int i = 1; i < nTmpHeight - 1; i++)
	{
		pSdx = (short*)(templateSobelXImg.ptr<uchar>(i));
		pSdy = (short*)(templateSobelYImg.ptr<uchar>(i));
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

	//非最大抑制法
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
		pSdx = (short*)(templateSobelXImg.ptr<uchar>(i));
		pSdy = (short*)(templateSobelYImg.ptr<uchar>(i));
		for (int j = 1; j < nTmpWidth; j++)
		{
			dDx = pSdx[j];
			dDy = pSdy[j];
			dMag = sqrt(dDx * dDx + dDy * dDy); //梯度强度Magnitude = Sqrt(gx^2 +gy^2)												
			nFlg = 1;
			if (((double)(nmsEdges.at<uchar>(i, j))) < dMaxThre)
			{
				if ((double)(nmsEdges.at<uchar>(i, j)) < dMinThre)
				{

					(nmsEdges.at<uchar>(i, j)) = 0;
					nFlg = 0; // 不是边缘，remove from edge
				}
				else
				{   // 如果8个相邻像素中的任何一个不大于最大值，则从边缘移除
					if ((((double)(nmsEdges.at<uchar>(i - 1, j - 1))) < dMaxThre) &&
						(((double)(nmsEdges.at<uchar>(i - 1, j))) < dMaxThre) &&
						(((double)((nmsEdges.at<uchar>(i - 1, j + 1)))) < dMaxThre) &&
						(((double)((nmsEdges.at<uchar>(i, j - 1)))) < dMaxThre) &&
						(((double)((nmsEdges.at<uchar>(i, j + 1)))) < dMaxThre) &&
						(((double)((nmsEdges.at<uchar>(i + 1, j - 1)))) < dMaxThre) &&
						(((double)((nmsEdges.at<uchar>(i + 1, j)))) < dMaxThre) &&
						(((double)((nmsEdges.at<uchar>(i + 1, j + 1)))) < dMaxThre))
					{
						(nmsEdges.at<uchar>(i, j)) = 0;
						nFlg = 0;
					}
				}
			}

			//保存提取到的边缘信息
			if (nFlg != 0)
			{
				if (dDx != 0 || dDy != 0)
				{
					nRSum = nRSum + i;	nCSum = nCSum + j; // Row sum and column sum for center of gravity

					pPositionEdge[nOfEdgePiex].x = i;
					pPositionEdge[nOfEdgePiex].y = j;
					pEdgeMX[nOfEdgePiex] = dDx;
					pEdgeMY[nOfEdgePiex] = dDy;

					//handle divide by zero
					if (dMag != 0)
					{
						pEdgeMagnitude[nOfEdgePiex] = 1 / dMag;  // gradient magnitude 
					}
					else
					{
						pEdgeMagnitude[nOfEdgePiex] = 0;
					}

					nOfEdgePiex++;
				}
			}
		}
	}

	ptCenterOfGravity.x = nRSum / nOfEdgePiex; // center of gravity
	ptCenterOfGravity.y = nCSum / nOfEdgePiex;	// center of gravity

	 // change coordinates to reflect center of gravity
	for (int m = 0; m < nOfEdgePiex; m++)
	{
		pPositionEdge[m].x = pPositionEdge[m].x - ptCenterOfGravity.x;
		pPositionEdge[m].y = pPositionEdge[m].y - ptCenterOfGravity.y;
	}

	bRev = DeleteDoubleMatrix(pMagMat, nTmpHeight);
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

//************************************
// Method:    CreateDirectory：路径下创建新文件夹
// FullName:  CreateModel::CreateDirectory
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const string folder：文件路径
//************************************
bool CreateModel::CreateDirectory(const string folder)
{
	if ("" == folder)
	{
		return false;
	}

	string folder_builder;
	string sub;
	sub.reserve(folder.size());
	for (auto it = folder.begin(); it != folder.end(); ++it)
	{
		//cout << *(folder.end()-1) << endl;
		const char c = *it;
		sub.push_back(c);
		if (c == PATH_DELIMITER || it == folder.end() - 1)
		{
			folder_builder.append(sub);

			if (0 != _access(folder_builder.c_str(), 0))  // 文件夹不存在，则创建
			{
				// this folder not exist
				if (0 != _mkdir(folder_builder.c_str()))
				{
					// create failed
					return false;
				}
			}

			sub.clear();
		}
	}
	return true;
}

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
bool CreateModel::Write(const string strFolder, const string strFileName, int nOfEdgePiex, CvPoint *pPositionEdge, double *pdEdgeMagnitude,
	double *pdEdgeMX, double *pdEdgeMY, int nTmpWidth, int nTmpHeight, int nPyrCount, bool IsSrc)
{
	if ("" == strFolder || "" == strFileName || nOfEdgePiex <= 0
		|| NULL == pPositionEdge || NULL == pdEdgeMagnitude || NULL == pdEdgeMX || NULL == pdEdgeMY)
	{
		return false;
	}

	string strPath = strFolder + "\\" + strFileName;
	ofstream fout(strPath, ios::app);
	if (!fout)
	{
		cout << "File Not Opened" << endl;
		return false;
	}

	double dSum = 0;
	for (int i = 0; i < nOfEdgePiex; i++)
	{
		dSum += pdEdgeMagnitude[i];
	}

	if (IsSrc)
	{
		fout << "srcTemplate" << setw(8) << nTmpWidth << setw(8) << nTmpHeight << endl;
	}
	else
	{
		fout << "pyrTemplate" << setw(8) << nPyrCount << endl;
	}

	double dEdgeThreshold = dSum / nOfEdgePiex;
	for (int i = 0; i < nOfEdgePiex; i++)
	{
		if (pdEdgeMagnitude[i] < dEdgeThreshold)
		{
			fout << setw(8) << pPositionEdge[i].x << setw(8) << pPositionEdge[i].y
				<< setw(16) << pdEdgeMagnitude[i] << setw(8) << pdEdgeMX[i] << setw(8) << pdEdgeMY[i] << endl;
		}
	}

	fout.close();
	return true;
}


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
bool CreateModel::listFiles(const char *pDir, vector<string> &files)
{
	if (NULL == pDir)
	{
		return false;
	}

	char dirNew[200];
	strcpy_s(dirNew, sizeof(dirNew) / sizeof(dirNew[0]), pDir);
	strcat_s(dirNew, sizeof(dirNew), "\\*.*");   // 在目录后面加上"\\*.*"进行第一次搜索
	intptr_t handle;
	_finddata_t findData;
	handle = _findfirst(dirNew, &findData);
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

			// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
			// 			strcpy_s(dirNew, sizeof(dirNew) / sizeof(dirNew[0]), pDir);
			// 			strcat_s(dirNew, sizeof(dirNew), "\\");
			// 			strcat_s(dirNew, sizeof(dirNew), findData.name);
			// 			listFiles(dirNew, files);
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
// Method:    is_dir：判断文件属性是目录还是文件
// FullName:  CreateModel::is_dir
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: int attrib：文件的一个属性
//************************************
bool CreateModel::is_dir(int attrib)
{
	return attrib == 16 || attrib == 18 || attrib == 20;
}

//************************************
// Method:    show_error：显示删除失败原因
// FullName:  CreateModel::show_error
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const char * file_name：
//************************************
void CreateModel::show_error(const char *file_name)
{
	errno_t err;
	_get_errno(&err);
	switch (err)
	{
	case ENOTEMPTY:
		printf("Given path is not a directory, the directory is not empty, or the directory is either the current working directory or the root directory.\n");
		break;
	case ENOENT:
		printf("Path is invalid.\n");
		break;
	case EACCES:
		printf("%s had been opend by some application, can't delete.\n", file_name);
		break;
	}
}

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
bool CreateModel::get_file_path(const char *pcPath, const char *pcFileName, char *pcFilePath)
{
	if (NULL == pcPath || NULL == pcFileName || NULL == pcFilePath)
	{
		return false;
	}

	strcpy_s(pcFilePath, sizeof(char) * _MAX_PATH, pcPath);
	pcFilePath[strlen(pcFilePath) - 1] = '\0';
	strcat_s(pcFilePath, sizeof(char) * _MAX_PATH, pcFileName);
	strcat_s(pcFilePath, sizeof(char) * _MAX_PATH, "\\*");
	return true;
}

//************************************
// Method:    deletefile：递归搜索目录中文件并删除
// FullName:  CreateModel::deletefile
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * path
//************************************
bool CreateModel::deletefile(const char *pcPath)
{
	if (NULL == pcPath)
	{
		return false;
	}

	_finddata_t dir_info;
	_finddata_t file_info;
	intptr_t f_handle;
	char tmp_path[_MAX_PATH];
	if ((f_handle = _findfirst(pcPath, &dir_info)) != -1)
	{
		while (_findnext(f_handle, &file_info) == 0)
		{
			if (strcmp(file_info.name, ".") == 0 || strcmp(file_info.name, "..") == 0)  //判断是否是".."目录和"."目录
				continue;
			if (is_dir(file_info.attrib))//如果是目录，生成完整的路径
			{
				if (!get_file_path(pcPath, file_info.name, tmp_path))
				{
					cout << "获取文件路径失败" << endl;
					return false;
				}

				deletefile(tmp_path);//开始递归删除目录中的内容
				tmp_path[strlen(tmp_path) - 2] = '\0';
				if (file_info.attrib == 20)
					printf("This is system file, can't delete!\n");
				else
				{
					//删除空目录，必须在递归返回前调用_findclose,否则无法删除目录
					if (_rmdir(tmp_path) == -1)
					{
						show_error();//目录非空则会显示出错原因
					}
				}
			}
			else
			{
				strcpy_s(tmp_path, pcPath);
				tmp_path[strlen(tmp_path) - 1] = '\0';
				strcat_s(tmp_path, file_info.name);//生成完整的文件路径

				if (remove(tmp_path) == -1)
				{
					show_error(file_info.name);
				}

			}
		}
		_findclose(f_handle);//关闭打开的文件句柄，并释放关联资源，否则无法删除空目录
	}
	else
	{
		show_error();//若路径不存在，显示错误信息
		return false;
	}
	return true;
}

//************************************
// Method:    pyr：获取降采样次数
// FullName:  CreateModel::pyr
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: int tmpheight：模板的高度
// Parameter: int tmpwidth：模板的宽度
//************************************
int CreateModel::pyr(int nTmpHeight, int nTmpWidth) //降采样次数
{
	if (nTmpWidth <= 0 || nTmpHeight <= 0)
	{
		return -1;
	}

	int nCnt = 0;

	if (MIN(nTmpHeight, nTmpWidth) < nMinTmpSize)
	{
		return nCnt;
	}

	for (int i = 1; i <= 10; i++)
	{
		nTmpHeight /= 2;
		nTmpWidth /= 2;
		if (MIN(nTmpHeight, nTmpWidth) < nMinTmpSize)
		{
			nCnt = MIN(nMaxPyCnt, i - 1);
			break;
		}
	}

	return nCnt;
}