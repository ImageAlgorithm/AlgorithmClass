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
// Method:    GetSobelEdge �õ�ͼƬ��X��Y����ͼ
// FullName:  CreateModel::GetSobelEdge
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: Mat InputImg	ԭͼ��
// Parameter: Mat & InputSobelDx  ͼ���SobelX����ͼ
// Parameter: Mat & InputSobelDy  ͼ���SobelX����ͼ
//************************************
bool CreateModel::GetSobelEdge(Mat InputImg, Mat& InputSobelDx, Mat& InputSobelDy)
{
	if (InputImg.empty())
	{
		return false;
	}

	Mat GrayInputImg;  //�Ҷ�ͼ��
	cvtColor(InputImg, GrayInputImg, COLOR_BGR2GRAY);

	Sobel(GrayInputImg, InputSobelDx, CV_16S, 1, 0, 3);
	Sobel(GrayInputImg, InputSobelDy, CV_16S, 0, 1, 3);

	return true;
}

//************************************
// Method:    CreateDoubleMatrix��������ά����
// FullName:  CreateModel::CreateDoubleMatrix
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: double * *  & matrix����ά�����ͷָ��  
// Parameter: int height����ά����ĸ߶�
// Parameter: int width����ά����Ŀ��
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
// Method:    DeleteDoubleMatrix��ɾ����ά����
// FullName:  CreateModel::DeleteDoubleMatrix
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: double * * & matrix����ά�����ͷָ��
// Parameter: int height����ά����ĸ߶�
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
// Method:    CreateMatchEdgeModel������ģ���ģ������
// FullName:  CreateModel::CreateMatchEdgeModel
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: Mat templateSobelXImg��ģ���X����ͼ��Sobel��
// Parameter: Mat templateSobelYImg��ģ���Y����ͼ��Sobel��
// Parameter: double maxThre�������ֵ
// Parameter: double minThre����С��ֵ
// Parameter: int & nofEdgePiex����Ե������
// Parameter: CvPoint * pPositionEdge���洢��Ե����
// Parameter: double * pEdgeMagnitude���洢��Ե�ݶȴ�С�ĵ���
// Parameter: double * pEdgeMX���洢x�����Ե�ݶ�
// Parameter: double * pEdgeMY���洢y�����Ե�ݶ�
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

	int *pnOrients = new int[nTmpWidth * nTmpHeight];//�洢����
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
			dMag = sqrt((float)(dDx * dDx) + (float)(dDy * dDy));  //�߽�ǿ��
			dDirection = cvFastArctan((float)dDy, (float)dDx);	  //�߽緽��
			pMagMat[i][j] = dMag;

			if (dMag > dMaxGradient)
			{
				dMaxGradient = dMag; //���� get maximum gradient value for normalizing.
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

	//��������Ʒ�
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
			// ����ǰ����ֵ���������ؽ��бȽ�compare current pixels value with adjacent pixels
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

	//�ͺ���ֵ
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
			dMag = sqrt(dDx * dDx + dDy * dDy); //�ݶ�ǿ��Magnitude = Sqrt(gx^2 +gy^2)												
			nFlg = 1;
			if (((double)(nmsEdges.at<uchar>(i, j))) < dMaxThre)
			{
				if ((double)(nmsEdges.at<uchar>(i, j)) < dMinThre)
				{

					(nmsEdges.at<uchar>(i, j)) = 0;
					nFlg = 0; // ���Ǳ�Ե��remove from edge
				}
				else
				{   // ���8�����������е��κ�һ�����������ֵ����ӱ�Ե�Ƴ�
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

			//������ȡ���ı�Ե��Ϣ
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
// Method:    CreateDirectory��·���´������ļ���
// FullName:  CreateModel::CreateDirectory
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const string folder���ļ�·��
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

			if (0 != _access(folder_builder.c_str(), 0))  // �ļ��в����ڣ��򴴽�
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
// Method:    Write����ģ������д��mdl�ļ�
// FullName:  CreateModel::Write
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const string strFolder���ļ�·��
// Parameter: const string strFileName���ļ���
// Parameter: int nOfEdgePiex����Ե������ĸ���
// Parameter: CvPoint * pPositionEdge����Ե�����������
// Parameter: double * pdEdgeMagnitude����Ե��������ݶȵĵ���
// Parameter: double * pdEdgeMX��X����ı�Ե�ݶ�
// Parameter: double * pdEdgeMY��Y����ı�Ե�ݶ�
// Parameter: int nTmpWidth��ģ��Ŀ��
// Parameter: int nTmpHeight��ģ��ĸ߶�
// Parameter: int nPyrCount���������Ĵ���
// Parameter: bool IsSrc���Ƿ���ԭͼ
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
// Method:    listFiles������ʵ�����õݹ飬�����������������Ŀ¼�е��ļ����������ҵ�Ŀ¼��ÿһ����ļ�
// 	�ڴ��������²�����Ҫ�����Ǳ���Ŀ¼�»��Ǳ���Ŀ¼�У���ΪĿ¼�������Լ������ģ�Ҫ������·��Ҳ���Լ�����
// 	����������ȫ���԰������������Ŀ¼���ļ��ĺ�������Ŀ¼�е�����ͼƬ(��ÿһ��Ŀ¼)
// FullName:  CreateModel::listFiles
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * pDir��·��
// Parameter: vector<string> & files���õ��Ĵ�·�����ļ���
//************************************
bool CreateModel::listFiles(const char *pDir, vector<string> &files)
{
	if (NULL == pDir)
	{
		return false;
	}

	char dirNew[200];
	strcpy_s(dirNew, sizeof(dirNew) / sizeof(dirNew[0]), pDir);
	strcat_s(dirNew, sizeof(dirNew), "\\*.*");   // ��Ŀ¼�������"\\*.*"���е�һ������
	intptr_t handle;
	_finddata_t findData;
	handle = _findfirst(dirNew, &findData);
	if (handle == -1)        // ����Ƿ�ɹ�
	{
		return false;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			// ��Ŀ¼�������"\\"����������Ŀ¼��������һ������
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

	_findclose(handle);    // �ر��������

	return true;
}

//************************************
// Method:    is_dir���ж��ļ�������Ŀ¼�����ļ�
// FullName:  CreateModel::is_dir
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: int attrib���ļ���һ������
//************************************
bool CreateModel::is_dir(int attrib)
{
	return attrib == 16 || attrib == 18 || attrib == 20;
}

//************************************
// Method:    show_error����ʾɾ��ʧ��ԭ��
// FullName:  CreateModel::show_error
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const char * file_name��
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
// Method:    get_file_path����ȡ�ļ�·��
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
// Method:    deletefile���ݹ�����Ŀ¼���ļ���ɾ��
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
			if (strcmp(file_info.name, ".") == 0 || strcmp(file_info.name, "..") == 0)  //�ж��Ƿ���".."Ŀ¼��"."Ŀ¼
				continue;
			if (is_dir(file_info.attrib))//�����Ŀ¼������������·��
			{
				if (!get_file_path(pcPath, file_info.name, tmp_path))
				{
					cout << "��ȡ�ļ�·��ʧ��" << endl;
					return false;
				}

				deletefile(tmp_path);//��ʼ�ݹ�ɾ��Ŀ¼�е�����
				tmp_path[strlen(tmp_path) - 2] = '\0';
				if (file_info.attrib == 20)
					printf("This is system file, can't delete!\n");
				else
				{
					//ɾ����Ŀ¼�������ڵݹ鷵��ǰ����_findclose,�����޷�ɾ��Ŀ¼
					if (_rmdir(tmp_path) == -1)
					{
						show_error();//Ŀ¼�ǿ������ʾ����ԭ��
					}
				}
			}
			else
			{
				strcpy_s(tmp_path, pcPath);
				tmp_path[strlen(tmp_path) - 1] = '\0';
				strcat_s(tmp_path, file_info.name);//�����������ļ�·��

				if (remove(tmp_path) == -1)
				{
					show_error(file_info.name);
				}

			}
		}
		_findclose(f_handle);//�رմ򿪵��ļ���������ͷŹ�����Դ�������޷�ɾ����Ŀ¼
	}
	else
	{
		show_error();//��·�������ڣ���ʾ������Ϣ
		return false;
	}
	return true;
}

//************************************
// Method:    pyr����ȡ����������
// FullName:  CreateModel::pyr
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: int tmpheight��ģ��ĸ߶�
// Parameter: int tmpwidth��ģ��Ŀ��
//************************************
int CreateModel::pyr(int nTmpHeight, int nTmpWidth) //����������
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