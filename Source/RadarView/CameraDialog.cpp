// CameraDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "CameraDialog.h"
#include "afxdialogex.h"

#include "Resource.h"
#include "MainFrm.h"
#include "RadarViewView.h"
#include "FileDataReader.h"
// CameraDialog 对话框

IMPLEMENT_DYNAMIC(CameraDialog, CDialogBar)

CameraDialog::CameraDialog(CWnd* pParent /*=nullptr*/)
	
{
	cornerSize = cv::Size(0,0);
	paramatersFilePath = "";
	m_RadarPointsNode = NULL;
	
	
}

CameraDialog::~CameraDialog()
{
}

void CameraDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PIC, m_ListMain);
}


BEGIN_MESSAGE_MAP(CameraDialog, CDialogBar)
	ON_WM_CREATE()
	ON_MESSAGE(WM_INITDIALOG, &OnInitDialog)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_BUTTON1, &CameraDialog::OnBnClickedButton1)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PIC, &CameraDialog::OnNMClickListPic)
	ON_BN_CLICKED(IDC_BUTTON2, &CameraDialog::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CameraDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCALIBRATE, &CameraDialog::OnBnClickedCalibrate)
	ON_BN_CLICKED(IDC_BUTTON_GETPOINT, &CameraDialog::OnBnClickedButtonGetpoint)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, &CameraDialog::OnBnClickedButtonChoose)
	ON_BN_CLICKED(IDC_BUTTON_READ1, &CameraDialog::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_PROJ, &CameraDialog::OnBnClickedButtonProj)
END_MESSAGE_MAP()


// CameraDialog 消息处理程序


int CameraDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

LRESULT CameraDialog::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	
	BOOL bRet = HandleInitDialog(wParam, lParam);
	if (!UpdateData(FALSE))
	{
		TRACE0("Warning, Unalbe to init update.\n");
	}
	//获取图像显示窗口的指针和绘图DC
	cv::namedWindow("view", cv::WINDOW_AUTOSIZE);
	
	HWND hWnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	
	m_ListMain.InsertColumn(0, _T("No."), LVCFMT_LEFT, 50);
	m_ListMain.InsertColumn(1, _T("FileName"), LVCFMT_LEFT, 200);//插入列


	return bRet;
}


void CameraDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);
	


}


void CameraDialog::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogBar::OnWindowPosChanged(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}


void CameraDialog::OnBnClickedButton1()
{
	// TODO: 添加图片列表
	CFileDialog dlg(TRUE, _T("*.jpg"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Image Files(*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp|All Files (*.*)|*.*||"), NULL);
	dlg.m_ofn.lpstrTitle = _T("选择图片");
	CString filename;
	vector<CString> vecPicPaths;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		while (fileNamesPosition != NULL)
		{
			filename = dlg.GetNextPathName(fileNamesPosition);
			vecPicPaths.push_back(filename);
		}
	}

	int count = vecPicPaths.size();
	if (count == 0)
		MessageBox(_T("没有选择文件"), NULL, MB_OK | MB_ICONQUESTION);
	else {
		for (int i = 0; i < count; i++) {
			
			OnAddPicture(vecPicPaths[i]);
		}
	}
}

void CameraDialog::ShowMat(cv::Mat img, UINT ID, string viewname)
{
	

	cv::Mat imagedst;
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	
	float scalesw = (float)rect.Width() / img.cols;
	float scalesh = (float)rect.Height() / img.rows;
	float scale = min(scalesw, scalesh);
	cv::Size dsize = cv::Size(img.cols*scale,img.rows*scale);

	resize(img,imagedst,dsize);
 
	cv::imshow(viewname, imagedst);
}

cv::Mat CameraDialog::generateMat(vector<string> strlist, int index)
{
	if (index >= strlist.size() || index < 0) {
		return cv::Mat();
	}
	cv::Mat matrix(3,4,CV_32FC1);
	
	string data = strlist[index];

	istringstream ss(data);
	string str;
	ss >> str;
	float num[12];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			ss >> num[i*4+j];
			matrix.at<float>(i,j) = num[i * 4 + j];
		}
		
	}

	return matrix;
}

void CameraDialog::OnAddPicture(CString path)
{
	string strpath = CT2A(path.GetBuffer());
	cv::Mat pic = cv::imread(strpath);
	pics.push_back(pic);
	CString t_strOut;
	int t_nPos = m_ListMain.GetItemCount();		//插入位置
	t_strOut.Format(_T("%d"), t_nPos);
	m_ListMain.InsertItem(t_nPos, t_strOut);
	
//	wstring str = path;
	
	m_ListMain.SetItemText(t_nPos, 1, path);
}

void CameraDialog::GetCurrentPointCloud()
{
	CMainFrame * mainfram = (CMainFrame *)AfxGetMainWnd();
	CRadarViewView  * view = (CRadarViewView*)mainfram->GetActiveView();
	this->m_RadarPointsNode = view->Get3DWnd()->m_RadarPointsNode;
	//pointlistPtr = view->Get3DWnd()->m_RadarPointsNode->gerCurrentPointList();
}


void CameraDialog::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialogBar::OnUpdateCmdUI(pTarget, FALSE);
}


void CameraDialog::OnNMClickListPic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	for (int i = 0; i < m_ListMain.GetItemCount(); i++) {
		if (m_ListMain.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			ShowMat(pics[i],IDC_STATIC,"view");
			currPic = pics[i];
		}
	}
}


void CameraDialog::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	m_ListMain.DeleteAllItems();
	pics.clear();

}


void CameraDialog::OnBnClickedOk()
{
	// TODO: 寻找角点
	CString txt;
	GetDlgItemText(IDC_EDITHEIGHT, txt);
	CString txt1;
	GetDlgItemText(IDC_EDITWIDTH, txt1);
	cornerSize = cv::Size(_ttoi(txt1),_ttoi(txt));

	for (auto itr = pics.begin(); itr != pics.end(); itr++) {
		if (0 == cv::findChessboardCorners(*itr, cornerSize, image_points_buf)) {

		}
		else {
			cv::Mat view_gray;
			cv::cvtColor(*itr, view_gray, cv::COLOR_RGB2GRAY);  // 转灰度图
			/* 亚像素精确化 */
			// image_points_buf 初始的角点坐标向量，同时作为亚像素坐标位置的输出
			// Size(5,5) 搜索窗口大小
			// （-1，-1）表示没有死区
			// TermCriteria 角点的迭代过程的终止条件, 可以为迭代次数和角点精度两者的组合  CV_TERMCRIT_EPS + CV_TERMCRIT_ITER
			cv::cornerSubPix(view_gray, image_points_buf, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(0, 30, 0.1));

			image_points_seq.push_back(image_points_buf);  // 保存亚像素角点

			/* 在图像上显示角点位置 */
			drawChessboardCorners(view_gray, cornerSize, image_points_buf, false); // 用于在图片中标记角点

			cv::imshow("Camera Calibration", view_gray);       // 显示图片

			cv::waitKey(500); //暂停0.5S      
		}
	}
}


void CameraDialog::OnBnClickedCalibrate()
{
	// TODO: 标定

}


void CameraDialog::OnBnClickedButtonGetpoint()
{
	// TODO: 更新点云数据
	GetCurrentPointCloud();

}


void CameraDialog::OnBnClickedButtonChoose()
{
	// TODO: 打开文本文件
	// 导入文件，每个文件对应一个配置
	TCHAR szFilter[] = _T("Setting File(*.txt)|*.txt|All File(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter);


	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
		paramatersFilePath = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT_PATH, paramatersFilePath);
	}
}

void output(ofstream & of,cv::Mat  mat) {
	if (!mat.empty()&&!of) {
		for (int i = 0; i < mat.rows; i++) {
			for (int j = 0; j < mat.cols; j++) {
				of << mat.at<float>(i, j) << " ";
			}
			of << endl;
		}
	}
}

void CameraDialog::OnBnClickedButtonRead()
{
	// TODO: 读取参数
	if (paramatersFilePath != "") {
		string filepath = CT2A(paramatersFilePath.GetBuffer());
		vector<string> data = FileDataReader::Instance()->readFile(filepath);

		cameraParameters = generateMat(data, 0);

		cv::Mat velo2cam = generateMat(data, 5);
		cv::Mat tmp = cv::Mat::zeros(1,4,CV_32FC1);
		tmp.at<float>(0, 3) = 1;
		cv::Mat velo2cam4x4;
		cv::vconcat(velo2cam, tmp, velo2cam4x4);
		velo2cam = velo2cam4x4;

		//TRACE();
		this->velo_to_cam = velo2cam;
		//ofstream ofile("D:\\debug.txt");
		//ofile << "123" << endl;
		////output(ofile, cameraParameters);
		//for (int i = 0; i < velo_to_cam.rows; i++) {
		//	for (int j = 0; j < velo_to_cam.cols; j++) {
		//		ofile <<velo_to_cam.at<float>(i, j) << " ";
		//	}
		//	ofile << endl;
		//}
		//
		////ofile << cameraParameters << endl;
		////ofile <<endl<< this->velo_to_cam << endl;
		//ofile.close();
	}
}


void CameraDialog::OnBnClickedButtonProj()
{
	// TODO: 投影

	RadarPointList &pointlistPtr = m_RadarPointsNode->gerCurrentPointList();

	//m_RadarPointsNode->GetPointListData(PointList);
	if (!currPic.empty() && pointlistPtr.size() > 0&& !cameraParameters.empty()&&!velo_to_cam.empty()) {

		cv::Mat velo_to_img = cameraParameters *(velo_to_cam);
		ofstream ofile("D:\\debug.txt");
		
		//output(ofile, cameraParameters);
		for (int i = 0; i < velo_to_img.rows; i++) {
			for (int j = 0; j < velo_to_img.cols; j++) {
				ofile << velo_to_img.at<float>(i, j) << " ";
			}
			ofile << endl;
		}

		ofile.close();
		m_RadarPointsNode->SetColorCoding(COLORCODING::CODING_MERGE);

		RadarPointList &PointList = pointlistPtr;

		m_RadarPointsNode->GetPointListData(PointList);
		for (auto ele = PointList.begin(); ele != PointList.end();ele++) {
			if (ele->x >= 5) {
				cv::Mat point(4, 1, CV_32FC1);
				point.at<float>(0, 0) = ele->x;
				point.at<float>(1, 0) = ele->y;
				point.at<float>(2, 0) = ele->z;
				point.at<float>(3, 0) = 1;
				cv::Mat res = velo_to_img * (point);//3*1
			//	res.at<float>(0, 0) = res.at<float>(0, 0) / res.at<float>(2, 0);
			//	res.at<float>(1, 0) = res.at<float>(1, 0) / res.at<float>(2, 0);
				int x = res.at<float>(0, 0) / res.at<float>(2, 0);
				int y = res.at<float>(1, 0) / res.at<float>(2, 0);
				if (x > 0&&x<currPic.cols && y > 0&&y<currPic.rows) {
					cv::Scalar color = currPic.at<cv::Vec3b>(y, x);
					ele->R = color[0];
					ele->G = color[1];
					ele->B = color[2];
				}	
			}
		}
	}
}
