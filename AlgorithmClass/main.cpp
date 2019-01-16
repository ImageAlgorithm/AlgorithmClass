/***************************************************************************
* 所有算法的测试文件，每个算法对应一个测试函数，并在main函数中调用，测试相应的算法
****************************************************************************/

#include <string>
#include <opencv2/opencv.hpp>
#include "SingleMatch.h"
#include "MultiTemplateMatch2.h"
#include "MultiTemplateMatch.h"
#include "SlidingCode.h"
#include "CreateMultiTemplateMatch.h"
#include "WeChatMatch.h"
#include "threadpool.h"

using namespace std;
using namespace cv;


/*
 * 单模板测试代码，在Main函数中调用即可。
 */
int singleMatch()
{
	//获取运行起始时间
	clock_t start, end;
	double time;
	start = clock();

	string InputImgPath = "E:\\Images\\testImg\\2.jpg";//argv[1];
	string TemplatePath = "E:\\Images\\testImg\\template.jpg";//argv[2];


	Mat InputSrcImg = imread(InputImgPath);
	if (InputSrcImg.empty())
	{
		cout << "InputSrcImg image is empty!" << endl;
		return 1;
	}
	Mat InputTmpImg = imread(TemplatePath);
	if (InputTmpImg.empty())
	{
		cout << "InputTmpImg image is empty!" << endl;
		return 1;
	}

	vector<MatchRst> rst;
	SingleMatch pro;
	//计算时间
	clock_t readImg = clock();
	time = (double)(readImg - start);
	cout << "读取模板和原图时间：" << time << endl;

	pro.DoInspect(InputSrcImg, InputTmpImg, rst, 2, 0.6);

	MatchRst res = rst[0];
	for (MatchRst mr : rst)
	{
		cout << "x:" << mr.nCentX << "	y:" << mr.nCentY << "	score:" << mr.dScore << endl;
	}


	//计算运行时间
	end = clock();
	time = (double)(end - start);
	cout << "\n此程序的运行时间为" << time << endl;

	Rect rect;
	rect.x = res.nCentX - InputTmpImg.cols / 2;
	rect.y = res.nCentY - InputTmpImg.rows / 2;
	rect.height = InputTmpImg.rows;
	rect.width = InputTmpImg.cols;
	rectangle(InputSrcImg, rect, Scalar(0, 0, 255), 2, 8, 0);//对外轮廓加矩形框 
	imwrite("MatchRst.bmp", InputSrcImg);
	return 0;
}

/*
 * 多模板测试代码，在Main函数中调用即可。
 */
int multiMatch2()
{
	MultiMatch2 sMM;

	clock_t start, finish;
	double totaltime;
	start = clock();

	//读取源图像
	string InputImgPath = "E:\\Images\\multi\\testImg\\img.jpg";//argv[1];
	Mat InputImg = imread(InputImgPath);
	int nChannel = InputImg.channels();
	if (InputImg.empty())
	{
		cout << "Input image is empty" << endl;
		return 1;
	}
	imshow("tupian", InputImg);
	waitKey(0);

	bool Res = false;

	//获取各个模板文件的名字
	char* TemplatePath = "E:\\Images\\multi\\model-LocalModels";//argv[2];
	vector<string> files;	//带路径的文件名
	Res = sMM.ListFiles(TemplatePath, files);
	
	if (!Res)
	{
		cout << "模板文件获取失败!" << endl;
		return 1;
	}

	int num = files.size();
	MultiMatch2 MM(num);
	bool b = MM.InitData(InputImg, files);
	if (!b)
	{
		cout << "识别数据初始化失败" << endl;
		return -1;
	}

	for (int i = 0; i < files.size(); i++)
	{
		MM.threadproc(MM.imageInfo, MM.tmpInfo[i], 0.6);
	}

	int ressize = MM.res.size();
	for (int i = 0; i < ressize; i++)
	{
		cout << MM.res[i].targetname << ":" << " nCentX:" << MM.res[i].nCentX << "; nCentY:" << MM.res[i].nCentY << endl;
	}

	finish = clock();
	totaltime = (double)(finish - start);
	cout << "\n此程序的运行时间为" << totaltime << "毫秒！" << endl;
	return 0;
}

int multiMatch()
{
	//65个模板，1.9秒

	MultiMatch sMM;

	clock_t start, finish;
	double totaltime;
	start = clock();

	//读取源图像
	string InputImgPath = "E:\\Images\\multi\\testImg\\img.jpg";//argv[1];
	Mat InputImg = imread(InputImgPath);
	if (!InputImg.data)
	{
		cout << "Input image is empty" << endl;
		return 1;
	}

	bool Res = false;

	//获取各个模板文件的名字
	char* TemplatePath = "E:\\Images\\multi\\img2-LocalModels";//argv[2];
	vector<string> files;	//带路径的文件名
	Res = sMM.ListFiles(TemplatePath, files);
	if (!Res)
	{
		cout << "模板文件获取失败!" << endl;
		return 1;
	}

	sMM.DoInspect(InputImg, files, 0.8, 5, sMM);

	int ressize = sMM.res.size();
	for (int i = 0; i < ressize; i++)
	{
		cout << sMM.res[i].targetname << ":" << " nCentX:" << sMM.res[i].nCentX << "; nCentY:" << sMM.res[i].nCentY << endl;
	}

	finish = clock();
	totaltime = (double)(finish - start);
	cout << "此程序的运行时间为" << totaltime << "毫秒！" << endl;
	return 0;
}

int sliding() 
{
	Mat InputImg = imread("C:\\Users\\White\\Desktop\\Screenshots\\123.jpg");
	if (InputImg.empty())
	{
		cout << "Input image is empty" << endl;
		return 1;
	}

	//图像旋转标志
	bool bRot = false;

	//滑动块的宽度与高度
	int nHuaDongKuaiWidth = 55;
	int nHuaDongKuaiHeight = 55;

	int nArcLength = 20;//滑动块的弧长度

	SlidingCode HD;
	POINT StartPos;
	StartPos.x = 124;
	StartPos.y = 628;
	POINT DetectRegLft;
	DetectRegLft.x = 84;
	DetectRegLft.y = 345;
	POINT DetectRegRgt;
	DetectRegRgt.x = 454;
	DetectRegRgt.y = 573;
	POINT EndPos;

	bool bRev = HD.DoInspect(InputImg, StartPos, DetectRegLft, DetectRegRgt, nHuaDongKuaiWidth,
		nHuaDongKuaiHeight, nArcLength, bRot, EndPos);
	if (!bRev)
	{
		cout << "查找失败！" << endl;
		return 1;
	}

	cout << StartPos.x << "," << StartPos.y << ";" << EndPos.x << "," << EndPos.y << endl;
	imwrite("MatchRst.bmp", InputImg);

	imshow("MatchRst.bmp", InputImg);
	waitKey(0);


	system("pause");
	return 0;
}

int createModelFile()
{
	/*if (argc != 2)
	{
	cout << "请按可执行文件路径、模板图片路径" << endl;
	return 1;
	}*/

	CreateModel cm;

	char* TemplatePath = "C:\\Users\\White\\Desktop\\temp\\img2";
	vector<string> files;
	bool Res = false;

	Res = cm.listFiles(TemplatePath, files);
	if (!Res)
	{
		cout << "模板目录下的文件获取失败" << endl;
		return 1;
	}

	int filenum = files.size();
	if (filenum < 1)
	{
		cout << "File is empty" << endl;
		return 1;
	}

	//创建文件夹
	string strTmp = "-LocalModels";
	string TemplateMatchPath = TemplatePath + strTmp;

	string strDel = TemplateMatchPath + "\\*";
	vector<string> Txtfiles;

	if (!_access(TemplateMatchPath.c_str(), 0))//存在
	{
		Res = cm.listFiles(TemplateMatchPath.c_str(), Txtfiles);
		if (!Res)
		{
			cout << "保存目录下的文件获取失败" << endl;
			return 1;
		}

		if (Txtfiles.size() > 0)  // 文件夹下面有.txt文件先清空再创建
		{
			Res = cm.deletefile(strDel.c_str());
			if (!Res)
			{
				cout << "保存目录下的文件删除失败" << endl;
				return 1;
			}
		}
	}

	Res = cm.CreateDirectory(TemplateMatchPath);
	if (!Res)
	{
		cout << "Create document failed" << endl;
		return 1;
	}

	Mat TmpImg;
	int nofEdgePiex = 0;
	double maxThre = 100, minThre = 10;
	int nPos = 0;

	for (int i = 0; i < filenum; i++)
	{
		TmpImg = imread(files[i]);
		if (TmpImg.empty())
		{
			cout << files[i] << " is empty" << endl;
			return 1;
		}

		Mat TmpSobelDx, TmpSobelDy;
		Res = cm.GetSobelEdge(TmpImg, TmpSobelDx, TmpSobelDy);
		if (!Res)
		{
			cout << files[i] << "sobel edge failed" << endl;
			return 1;
		}

		nofEdgePiex = 0;
		CvPoint *pPositionEdge = new CvPoint[TmpImg.rows * TmpImg.cols];
		if (NULL == pPositionEdge)
		{
			cout << "CvPoint空间申请失败！" << endl;
			return 1;
		}
		double *pEdgeMagnitude = new double[3 * TmpImg.rows * TmpImg.cols];
		if (NULL == pEdgeMagnitude)
		{
			delete[] pPositionEdge;
			pPositionEdge = NULL;
			cout << "double * 3空间申请失败！" << endl;
			return 1;
		}
		double *pEdgeMX = pEdgeMagnitude + TmpImg.rows * TmpImg.cols;
		double *pEdgeMY = pEdgeMX + TmpImg.rows * TmpImg.cols;

		Res = cm.CreateMatchEdgeModel(TmpSobelDx, TmpSobelDy, maxThre, minThre, nofEdgePiex, pPositionEdge,
			pEdgeMagnitude, pEdgeMX, pEdgeMY);
		if (!Res)
		{
			cout << files[i] << " create match model failed" << endl;
			delete[] pPositionEdge;
			delete[] pEdgeMagnitude;
			pPositionEdge = NULL;
			pEdgeMagnitude = NULL;
			return 1;
		}

		nPos = 0;
		nPos = files[i].rfind('.');
		files[i].erase(nPos);
		nPos = files[i].rfind('\\');
		files[i] = files[i].erase(0, nPos + 1);

		//写入数据
		string strFileName = files[i] + ".mdl";
		int nSrcTmpWidth = TmpImg.cols;
		int nSrcTmpHeight = TmpImg.rows;
		int nPyrCount = cm.pyr(nSrcTmpHeight, nSrcTmpWidth);
		// 存储原模版图边缘数据及缩放次数
		Res = cm.Write(TemplateMatchPath, strFileName, nofEdgePiex, pPositionEdge, pEdgeMagnitude, pEdgeMX, pEdgeMY, 
			nSrcTmpWidth, nSrcTmpHeight, nPyrCount, true);
		if (!Res)
		{
			cout << files[i] << ".mdl write failure" << endl;
			delete[] pPositionEdge;
			delete[] pEdgeMagnitude;
			pPositionEdge = NULL;
			pEdgeMagnitude = NULL;
			return 1;
		}

		// 根据缩放次数对原图进行缩放
		if (nPyrCount > 0)
		{
			Mat src = TmpImg;
			Mat dst;
			for (int i = 1; i <= nPyrCount; i++)
			{
				pyrDown(src, dst, Size(src.cols / 2, src.rows / 2));
				blur(dst, dst, Size(2, 2));
				src = dst;
			}

			// 缩放后的边缘
			TmpImg = src;
			Mat TmpSobelDx, TmpSobelDy;
			Res = cm.GetSobelEdge(TmpImg, TmpSobelDx, TmpSobelDy);
			if (!Res)
			{
				cout << files[i] << " pyr sobel edge failed" << endl;
				delete[] pPositionEdge;
				delete[] pEdgeMagnitude;
				pPositionEdge = NULL;
				pEdgeMagnitude = NULL;
				return 1;
			}

			nofEdgePiex = 0;

			Res = cm.CreateMatchEdgeModel(TmpSobelDx, TmpSobelDy, maxThre, minThre, nofEdgePiex, pPositionEdge,
				pEdgeMagnitude, pEdgeMX, pEdgeMY);
			if (!Res)
			{
				cout << files[i] << " pyr create match model failed" << endl;
				delete[] pPositionEdge;
				delete[] pEdgeMagnitude;
				pPositionEdge = NULL;
				pEdgeMagnitude = NULL;
				return 1;
			}

			// 存储缩放后的边缘数据
			//缩放后的模板宽高没有用，所以传参为0
			Res = cm.Write(TemplateMatchPath, strFileName, nofEdgePiex, pPositionEdge, pEdgeMagnitude, pEdgeMX, pEdgeMY, 
				0, 0, nPyrCount, false);
			if (!Res)
			{
				cout << files[i] << ".mdl write failure" << endl;
				delete[] pPositionEdge;
				delete[] pEdgeMagnitude;
				pPositionEdge = NULL;
				pEdgeMagnitude = NULL;
				return 1;
			}
			
		}
		//写入end,防止读取到错误数据死循环。
		string strPath = TemplateMatchPath + "\\" + strFileName;
		ofstream fout(strPath, ios::app);
		fout << "end" << endl;
		fout.close();
		delete[] pPositionEdge;
		delete[] pEdgeMagnitude;
		pPositionEdge = NULL;
		pEdgeMagnitude = NULL;
	}

	return 0;
}

void testReadFile()
{
	ifstream infile("E:\\Images\\multi\\mobilePhone\\充值.mdl");
	if (!infile)
	{
		cout << "File Not Opened" << endl;
		infile.close();
		return ;
	}

	string  str1, str2;
	infile >> str1 ;
	cout << str1 << endl;
	infile >> str1;
	cout << str1 << endl;
	infile >> str1;
	cout << str1 << endl;
	double dNumber1;
	int a = 0;
	while (a < 50)
	{
		infile >> str2;
		if (str2 == "pyrTemplate")
		{
			cout << str2 << endl;
			infile >> str2;
			cout << str2 << endl;
			while (a < 50)
			{
				double d1, d2, d3, d4, d5;
				infile >> dNumber1;
				d1 = dNumber1;
				infile >> dNumber1;
				d2 = dNumber1;
				infile >> dNumber1;
				d3 = dNumber1;
				infile >> dNumber1;
				d4 = dNumber1;
				infile >> dNumber1;
				d5 = dNumber1;
				if (d1 != d3)
				{
					cout << d1 << endl;
					cout << d2 << endl;
					cout << d3 << endl;
					cout << d4 << endl;
					cout << d5 << endl;
				}
				else
				{
					return ;
				}
				a += 5;
				

			}
		}
		else if (str2 == "srcTemplate")
		{
			return ;
		}
		else
		{
			dNumber1 = atof(str2.c_str());
			cout << dNumber1 << endl;
			for (int i = 0; i < 4; ++i)
			{
				infile >> dNumber1;
				cout << dNumber1 << endl;
			}
			a += 5;
		}
	}

	infile.close();
}

int WeChatTest()
{
	WeChat a;
// 	Mat InputImg = imread("E:\\Images\\WeChatMatchTest\\222.png");//读取原图
// 	if (InputImg.empty())
// 	{
// 		cout << "1InputImg is empty!" << endl;
// 		return false;
// 	}
// 
// 	bool bHave = false;
// 
// 	WeChat a;
// 	bool bRes = a.NewsExist(InputImg, bHave);
// 	if (!bRes)
// 	{
// 		return 1;
// 	}
// 	if (bHave) {
// 		cout << "您有新的消息请及时处理！！！" << endl;
// 	}
// 	else {
// 		cout << "您还没有新消息呦" << endl;
// 	}

	/************************************识别消息气泡的位置****************************************/
	string InputImgPath = "E:\\Images\\WeChatMatchTest\\101.png";
	string TemplatePath = "E:\\Images\\WeChatMatchTest\\tmp.jpg";

	Mat InputSrcImg = imread(InputImgPath);
	if (InputSrcImg.empty())
	{
		cout << "InputSrcImg image is empty!" << endl;
		return 1;
	}
	Mat InputTmpImg = imread(TemplatePath);
	if (InputTmpImg.empty())
	{
		cout << "InputTmpImg image is empty!" << endl;
		return 1;
	}
	vector<MatchRst> Rst;
	bool Res = a.MatchNewsPosition(InputSrcImg, InputTmpImg, Rst);
	if (!Res)
	{
		return 1;
	}
	//输出最终的匹配中心
	if (Rst.size() < 1)
	{
		cout << "不存在尖号匹配结果!" << endl;
	}


	for (int i = 0; i < Rst.size(); i++)
	{
		Rect rect;
		rect.x = Rst[i].nCentX - (InputTmpImg.cols / 2);
		rect.y = Rst[i].nCentY - (InputTmpImg.rows / 2);
		rect.height = InputTmpImg.rows;
		rect.width = InputTmpImg.cols;
		rectangle(InputSrcImg, rect, Scalar(0, 0, 255), 2, 8, 0);//对外轮廓加矩形框 

	}
	imwrite("MatchRst.bmp", InputSrcImg);


	/********************************识别复制按钮的位置******************************************/
// 	string TemplatePath1 = "C:\\Users\\White\\Desktop\\WeChatMatchTest\\fuzhi.jpg";
// 	string InputImgPath1 = "C:\\Users\\White\\Desktop\\WeChatMatchTest\\6.png";
// 
// 	Mat InputSrcImg1 = imread(InputImgPath1);
// 	if (InputSrcImg.empty())
// 	{
// 		cout << "InputSrcImg image is empty!" << endl;
// 		return 1;
// 	}
// 
// 	Mat InputTmpImg1 = imread(TemplatePath1);
// 	if (InputTmpImg1.empty())
// 	{
// 		cout << "InputTmpImg1 image is empty!" << endl;
// 		return 1;
// 	}
// 
// 	MatchRst Rst1;
// 	Res = a.MatchButtonPos(InputSrcImg1, InputTmpImg1, Rst1);
// 	if (!Res)
// 	{
// 		cout << "复制按钮匹配出错" << endl;
// 		return 1;
// 	}
// 	cout << "坐标x:" << Rst1.nCentX << "坐标Y：" << Rst1.nCentY << endl;
// 
// 
// 
// 	Rect rect1;
// 	rect1.x = Rst1.nCentX - (InputTmpImg1.cols / 2);
// 	rect1.y = Rst1.nCentY - (InputTmpImg1.rows / 2);
// 	rect1.height = InputTmpImg1.rows;
// 	rect1.width = InputTmpImg1.cols;
// 	rectangle(InputSrcImg1, rect1, Scalar(0, 0, 0), 2, 8, 0);//对外轮廓加矩形框 
// 
// 
// 	imwrite("output_title.bmp", InputSrcImg1);

	//waitKey(0);

	return 0;

}

int main()
{
	//testReadFile();
	//multiMatch2();
	//multiMatch();
	//singleMatch();
	//sliding();
	//createModelFile();
	//WeChatTest();
	for (int i = 0; i < 100; i++)
	{
		cout << i << endl;
		multiMatch();
	}
	return 0;
}