
// DanmakuChickenDlg.h : 头文件
//

#pragma once
#include "OverlayDlg.h"
#undef min
#undef max
#include "..\Simple-Web-Server\server_http.hpp"
#include <thread>
#include "afxwin.h"
#include "afxcmn.h"
#include "afxdtctl.h"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;


// CDanmakuChickenDlg 对话框
class CDanmakuChickenDlg : public CDialogEx
{
// 构造
public:
	CDanmakuChickenDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DANMAKUCHICKEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;    
	CTabCtrl m_styleTabCtrl;
	CRect m_tabClientRect;

	afx_msg void OnTcnSelchangeStyleTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedStrokeEnable();
	void InitStyleTab();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButton2();

	// 处理添加弹幕请求
	void HandleAddDanmaku(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);


	COverlayDlg m_overlayDlg;
	HttpServer m_server;
	std::thread m_serverThread;

public:
	CSliderCtrl m_danmakuSizeSlider;
	CSliderCtrl m_danmakuSpeedSlider;
	CSliderCtrl m_danmakuOpacitySlider;
	
	// 样式设置控件
	CComboBox m_fontComboBox;
	CSliderCtrl m_fontSizeSlider;
	CButton m_textColorButton;
	CButton m_strokeEnableCheckbox;
	CButton m_strokeColorButton;
	CSliderCtrl m_opacitySlider;
	
	// 当前选中的标签页
	int m_currentTab;
	
	// 颜色变量
	COLORREF m_textColor;
	COLORREF m_strokeColor;
	
	//// 初始化样式设置界面
	//void InitStyleTab();
	// 更新预览区域
	void UpdatePreview();
	//// 显示颜色选择对话框
	//COLORREF ShowColorDialog(COLORREF initialColor);
	//// 处理标签页切换
	//afx_msg void OnTcnSelchangeStyleTab(NMHDR *pNMHDR, LRESULT *pResult);
	// 处理字体选择变化
	afx_msg void OnCbnSelchangeFontCombo();
	// 处理字体大小变化
	afx_msg void OnHScrollStyle(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 处理文字颜色按钮点击
	afx_msg void OnBnClickedTextColor();
	//// 处理描边开关变化
	//afx_msg void OnBnClickedStrokeEnable();
	// 处理描边颜色按钮点击
	afx_msg void OnBnClickedStrokeColor();
	// 处理透明度变化
	afx_msg void OnHScrollOpacity(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
