
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

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnSelchangeComboFont();
	afx_msg void OnBnClickedButtonTextColor();
	afx_msg void OnBnClickedButtonStrokeColor();
	afx_msg void OnBnClickedCheckStroke();

	// 设置保存和加载
	void SaveSettings();
	void LoadSettings();

	// 处理添加弹幕请求
	void HandleAddDanmaku(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);

	// 更新预览区域
	void UpdatePreview();

	COverlayDlg m_overlayDlg;
	HttpServer m_server;
	std::thread m_serverThread;

	CSliderCtrl m_danmakuSizeSlider;
	CSliderCtrl m_danmakuSpeedSlider;
	CSliderCtrl m_danmakuOpacitySlider;
	
	CTabCtrl m_tabControl;
	CComboBox m_fontCombo;
	CSliderCtrl m_fontSizeSlider;
	CButton m_textColorButton;
	CButton m_strokeCheck;
	CButton m_strokeColorButton;
	CSliderCtrl m_opacitySlider;
	CStatic m_previewStatic;
	
	// 样式设置对话框
	CDialog m_stylePage;
	
	// 系统字体列表
	std::vector<CString> m_systemFonts;

	// 字体枚举回调函数
	static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* lpelfe, NEWTEXTMETRICEX* lpntme, DWORD FontType, LPARAM lParam);
};
