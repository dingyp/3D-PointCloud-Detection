#pragma once


// CameraDialog 对话框
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/calib3d/calib3d.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <vector>

#include "StructDefine.h"
#include "RadarPointsNode.h"
#include <sstream>
using namespace std;

class CameraDialog : public CDialogBar
{
	DECLARE_DYNAMIC(CameraDialog)

public:
	CameraDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CameraDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedButton1();
	void ShowMat(cv::Mat img, UINT ID, string viewname);
	cv::Mat generateMat(vector<string> strlist,int index); //矩阵构建

	void OnAddPicture(CString path);
	void GetCurrentPointCloud();
	vector<cv::Mat> pics;
	cv::Mat currPic;


	cv::Size cornerSize;
	cv::Size image_size;      /* 图像的尺寸 */
	RadarPointsNode * m_RadarPointsNode;
	//RadarPointList & pointlistPtr;
	
	cv::Mat P_velo_to_img;
	cv::Mat cameraParameters;//3*4相机内参

	cv::Mat velo_to_cam;//4x4
	
	vector<cv::Point2f> image_points_buf;         /* 缓存每幅图像上检测到的角点 */
	vector<vector<cv::Point2f>> image_points_seq; /* 保存检测到的所有角点 */

	CString paramatersFilePath;//参数路径
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler);
	CListCtrl m_ListMain;
	afx_msg void OnNMClickListPic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCalibrate();
	afx_msg void OnBnClickedButtonGetpoint();
	afx_msg void OnBnClickedButtonChoose();
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonProj();
};
