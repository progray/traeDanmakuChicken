
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
#include "afxtabctrl.h"

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

	// 字体枚举回调函数
	friend int CALLBACK EnumFontFamProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType, LPARAM lParam);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButton2();

	// 标签页和样式设置相关消息处理函数
	afx_msg void OnTcnSelchangeTabControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeFontCombobox();
	afx_msg void OnBnClickedTextColorButton();
	afx_msg void OnBnClickedStrokeEnableCheckbox();
	afx_msg void OnHScrollStrokeWidth(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedStrokeColorButton();

	// 辅助函数
	void UpdatePreview();

	// 处理添加弹幕请求
	void HandleAddDanmaku(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);


	COverlayDlg m_overlayDlg;
	HttpServer m_server;
	std::thread m_serverThread;

	// 标签页控件
	CTabCtrl m_tabControl;
	
	// 常规设置对话框
	CDialogEx m_generalDlg;
	// 样式设置对话框
	CDialogEx m_styleDlg;
	
	// 常规设置控件
	CSliderCtrl m_danmakuSizeSlider;
	CSliderCtrl m_danmakuSpeedSlider;
	CSliderCtrl m_danmakuOpacitySlider;
	
	// 样式设置控件
	CComboBox m_fontComboBox;
	CButton m_textColorButton;
	CButton m_strokeEnableCheckbox;
	CSliderCtrl m_strokeWidthSlider;
	CButton m_strokeColorButton;
	CStatic m_previewStatic;
};
