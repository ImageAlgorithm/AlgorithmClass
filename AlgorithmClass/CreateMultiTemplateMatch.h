#ifndef _CREATE_MULTI_TEMPLATE_MATCH_H
#define _CREATE_MULTI_TEMPLATE_MATCH_H
#include "structure.h"
#include <iomanip>
#include <io.h>
#include <fstream>

#define PATH_DELIMITER '\\'

class CreateModel
{
public:

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
	bool GetSobelEdge(Mat InputImg, Mat& InputSobelDx, Mat& InputSobelDy);

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
	bool CreateDoubleMatrix(double** &matrix, int nHeight, int nWidth);

	//************************************
	// Method:    DeleteDoubleMatrix��ɾ����ά����
	// FullName:  CreateModel::DeleteDoubleMatrix
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: double * * & matrix����ά�����ͷָ��
	// Parameter: int height����ά����ĸ߶�
	//************************************
	bool DeleteDoubleMatrix(double **&matrix, int nHeight);

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
	bool CreateMatchEdgeModel(Mat templateSobelXImg, Mat templateSobelYImg, double maxThre, double minThre, int& nofEdgePiex,
		CvPoint *pPositionEdge, double *pEdgeMagnitude, double *pEdgeMX, double *pEdgeMY);

	//************************************
	// Method:    CreateDirectory��·���´������ļ���
	// FullName:  CreateModel::CreateDirectory
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const string folder���ļ�·��
	//************************************
	bool CreateDirectory(const string folder);

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
	bool Write(const string strFolder, const string strFileName, int nOfEdgePiex, CvPoint *pPositionEdge, double *pdEdgeMagnitude,
		double *pdEdgeMX, double *pdEdgeMY, int nTmpWidth, int nTmpHeight, int nPyrCont = 0, bool IsSrc = true);

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
	bool listFiles(const char *pDir, vector<string> &files);

	//************************************
	// Method:    is_dir���ж��ļ�������Ŀ¼�����ļ�
	// FullName:  CreateModel::is_dir
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: int attrib���ļ���һ������
	//************************************
	bool is_dir(int attrib);

	//************************************
	// Method:    show_error����ʾɾ��ʧ��ԭ��
	// FullName:  CreateModel::show_error
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * file_name��
	//************************************
	void show_error(const char *file_name = NULL);

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
	bool get_file_path(const char *path, const char *file_name, char *file_path);

	//************************************
	// Method:    deletefile���ݹ�����Ŀ¼���ļ���ɾ��
	// FullName:  CreateModel::deletefile
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const char * path
	//************************************
	bool deletefile(const char *path);

	//************************************
	// Method:    pyr����ȡ����������
	// FullName:  CreateModel::pyr
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Parameter: int tmpheight��ģ��ĸ߶�
	// Parameter: int tmpwidth��ģ��Ŀ��
	//************************************
	int pyr(int tmpheight, int tmpwidth);

private:
	const int nMaxPyCnt = 3;    //��󽵲�������
	const int nMinTmpSize = 15; //��������ģ�����С�Ŀ���
};


#endif
