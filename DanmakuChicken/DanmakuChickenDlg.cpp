
// DanmakuChickenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DanmakuChicken.h"
#include "DanmakuChickenDlg.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;
#include <codecvt>
#include <vector>
#include <algorithm>
//#include <gdiplus/gdiplus.h>  // Potential conflict
using namespace std;
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// Gdiplus初始化变量
ULONG_PTR gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CDanmakuChickenDlg 对话框



CDanmakuChickenDlg::CDanmakuChickenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDanmakuChickenDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDanmakuChickenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_STYLE, m_styleTabCtrl);
	DDX_Control(pDX, IDC_SIZE_SLIDER, m_danmakuSizeSlider);
	DDX_Control(pDX, IDC_SPEED_SLIDER, m_danmakuSpeedSlider);
	DDX_Control(pDX, IDC_OPACITY_SLIDER, m_danmakuOpacitySlider);
	DDX_Control(pDX, IDC_FONT_COMBO, m_fontComboBox);
	DDX_Control(pDX, IDC_FONT_SIZE_SLIDER, m_fontSizeSlider);
	DDX_Control(pDX, IDC_TEXT_COLOR_BUTTON, m_textColorButton);
	DDX_Control(pDX, IDC_STROKE_ENABLE_CHECK, m_strokeEnableCheckbox);
	DDX_Control(pDX, IDC_STROKE_COLOR_BUTTON, m_strokeColorButton);
}

BEGIN_MESSAGE_MAP(CDanmakuChickenDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &CDanmakuChickenDlg::OnBnClickedButton2)
	ON_WM_HSCROLL()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_STYLE, &CDanmakuChickenDlg::OnTcnSelchangeStyleTab)
	ON_CBN_SELCHANGE(IDC_FONT_COMBO, &CDanmakuChickenDlg::OnCbnSelchangeFontCombo)
	ON_BN_CLICKED(IDC_TEXT_COLOR_BUTTON, &CDanmakuChickenDlg::OnBnClickedTextColor)
	ON_BN_CLICKED(IDC_STROKE_ENABLE_CHECK, &CDanmakuChickenDlg::OnBnClickedStrokeEnable)
	ON_BN_CLICKED(IDC_STROKE_COLOR_BUTTON, &CDanmakuChickenDlg::OnBnClickedStrokeColor)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_STYLE, &CDanmakuChickenDlg::OnTcnSelchangeStyleTab)
	//ON_BN_CLICKED(IDC_STROKE_ENABLE, &CDanmakuChickenDlg::OnBnClickedStrokeEnable)
END_MESSAGE_MAP()


// CDanmakuChickenDlg 消息处理程序

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDanmakuChickenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDanmakuChickenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDanmakuChickenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化Gdiplus
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);		// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 初始化基本设置
	m_danmakuSizeSlider.SetRange(20, 100);
	m_danmakuSizeSlider.SetPos((int)m_overlayDlg.m_danmakuManager.m_danmakuSize);
	m_danmakuSpeedSlider.SetRange(1, 100);
	m_danmakuSpeedSlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuSpeed);
	m_danmakuOpacitySlider.SetRange(255 * 10 / 100, 255);
	m_danmakuOpacitySlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuAlpha);

	// 初始化样式设置
	InitStyleTab();

	// 默认显示基本设置标签页，隐藏样式设置相关控件
	m_currentTab = 0;
	GetDlgItem(IDC_STATIC_FONT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FONT_COMBO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_FONT_SIZE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FONT_SIZE_SLIDER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_TEXT_COLOR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TEXT_COLOR_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STROKE_ENABLE_CHECK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_STROKE_COLOR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STROKE_COLOR_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_OPACITY_STYLE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_OPACITY_SLIDER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PREVIEW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PREVIEW_AREA)->ShowWindow(SW_HIDE);

	// 载入弹幕窗口
	m_overlayDlg.Create(m_overlayDlg.IDD, GetDesktopWindow());

	//// 启动服务器
	//m_serverThread = thread([this] {
	//	//m_server.config.address = "127.0.0.1";
	//	//m_server.config.port = 12450;
	//	//m_server.resource["^/danmaku"]["POST"] = bind(&CDanmakuChickenDlg::HandleAddDanmaku, 
	//	//	this, placeholders::_1, placeholders::_2);
	//	//m_server.start();
	//});

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDanmakuChickenDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// 停止服务器
	m_server.stop();
	if (m_serverThread.joinable())
		m_serverThread.join();

	// 关闭弹幕窗口
	m_overlayDlg.DestroyWindow();

	// 清理Gdiplus
	GdiplusShutdown(gdiplusToken);
}

// 修改弹幕设置
void CDanmakuChickenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBTRACK)
	{
		switch (pScrollBar->GetDlgCtrlID())
		{
		case IDC_SIZE_SLIDER:
			m_overlayDlg.m_danmakuManager.m_danmakuSize = (float)nPos;
			break;
		case IDC_SPEED_SLIDER:
			m_overlayDlg.m_danmakuManager.m_danmakuSpeed = nPos;
			break;
		case IDC_OPACITY_SLIDER:
			m_overlayDlg.m_danmakuManager.m_danmakuAlpha = nPos;
			break;
		case IDC_FONT_SIZE_SLIDER:
			m_overlayDlg.m_danmakuManager.m_danmakuSize = (float)nPos;
			UpdatePreview();
			break;
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

// 初始化样式设置界面
void CDanmakuChickenDlg::InitStyleTab()
{
	// 添加标签页
	TCITEM tabItem;
	tabItem.mask = TCIF_TEXT;
	tabItem.pszText = L"基本设置";
	m_styleTabCtrl.InsertItem(0, &tabItem);
	tabItem.pszText = L"样式设置";
	m_styleTabCtrl.InsertItem(1, &tabItem);

	// 获取标签页客户区域
	m_styleTabCtrl.GetClientRect(&m_tabClientRect);
	m_styleTabCtrl.AdjustRect(FALSE, &m_tabClientRect);

	// Gdiplus初始化检查
	Gdiplus::Status gdiplusStatus = Gdiplus::Ok;

	// 初始化字体下拉框
	vector<CString> fontNames;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfCharSet = DEFAULT_CHARSET;
	
	EnumFontFamiliesEx(GetDC()->m_hDC, &logFont, [](const LOGFONT* lf, const TEXTMETRIC* tm, DWORD type, LPARAM data) {
		vector<CString>* fontList = reinterpret_cast<vector<CString>*>(data);
		if (type == TRUETYPE_FONTTYPE && !(lf->lfPitchAndFamily & FIXED_PITCH)) {
			CString fontName(lf->lfFaceName);
			if (find(fontList->begin(), fontList->end(), fontName) == fontList->end()) {
				fontList->push_back(fontName);
			}
		}
		return 1;
	}, reinterpret_cast<LPARAM>(&fontNames), 0);

	// 排序并添加到下拉框
	sort(fontNames.begin(), fontNames.end());
	for (const auto& fontName : fontNames) {
		m_fontComboBox.AddString(fontName);
	}

	// 选择当前字体
	int fontIndex = m_fontComboBox.FindStringExact(-1, m_overlayDlg.m_danmakuManager.m_danmakuFontName);
	if (fontIndex != CB_ERR) {
		m_fontComboBox.SetCurSel(fontIndex);
	}

	// 初始化字体大小滑块
	m_fontSizeSlider.SetRange(10, 50);
	m_fontSizeSlider.SetPos((int)m_overlayDlg.m_danmakuManager.m_danmakuSize);

	// 初始化颜色
	m_textColor = RGB(
		m_overlayDlg.m_danmakuManager.m_textColor.GetR(),
		m_overlayDlg.m_danmakuManager.m_textColor.GetG(),
		m_overlayDlg.m_danmakuManager.m_textColor.GetB());
	m_strokeColor = RGB(
		m_overlayDlg.m_danmakuManager.m_strokeColor.GetR(),
		m_overlayDlg.m_danmakuManager.m_strokeColor.GetG(),
		m_overlayDlg.m_danmakuManager.m_strokeColor.GetB());

	// 初始化描边开关
	m_strokeEnableCheckbox.SetCheck(m_overlayDlg.m_danmakuManager.m_strokeEnabled ? BST_CHECKED : BST_UNCHECKED);
	m_strokeColorButton.EnableWindow(m_overlayDlg.m_danmakuManager.m_strokeEnabled);

	// 初始化透明度滑块
	m_opacitySlider.SetRange(0, 100);
	m_opacitySlider.SetPos((int)((float)m_overlayDlg.m_danmakuManager.m_danmakuAlpha / 255.0f * 100.0f));

	// 初始化预览区域
	UpdatePreview();

	// 默认显示基本设置标签页
	m_currentTab = 0;
	// 隐藏样式设置相关控件
	GetDlgItem(IDC_STATIC_FONT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FONT_COMBO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_FONT_SIZE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FONT_SIZE_SLIDER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_TEXT_COLOR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TEXT_COLOR_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STROKE_ENABLE_CHECK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_STROKE_COLOR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STROKE_COLOR_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_OPACITY_STYLE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_OPACITY_SLIDER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PREVIEW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PREVIEW_AREA)->ShowWindow(SW_HIDE);
}

// 更新预览区域
void CDanmakuChickenDlg::UpdatePreview()
{
	CWnd* pPreviewWnd = GetDlgItem(IDC_PREVIEW_AREA);
	if (!pPreviewWnd) return;

	CDC* pDC = pPreviewWnd->GetDC();
	if (!pDC) return;

	CRect rect;
	pPreviewWnd->GetClientRect(&rect);

	// 清空预览区域
	pDC->FillSolidRect(&rect, RGB(255, 255, 255));

	// 使用Gdiplus绘制预览文本
	Graphics graphics(pDC->m_hDC);
	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

	// 创建字体
	Gdiplus::Font font(m_overlayDlg.m_danmakuManager.m_danmakuFont.get(), m_overlayDlg.m_danmakuManager.m_danmakuSize, FontStyle::FontStyleBold);

	// 创建文本画刷
	SolidBrush textBrush(m_overlayDlg.m_danmakuManager.m_textColor);

	// 绘制文本
	CString previewText(_T("预览弹幕样式"));
	RectF textRect(0, 0, rect.Width(), rect.Height());
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignment::StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignment::StringAlignmentCenter);

	if (m_overlayDlg.m_danmakuManager.m_strokeEnabled) {
		// 绘制描边
		Pen strokePen(m_overlayDlg.m_danmakuManager.m_strokeColor, m_overlayDlg.m_danmakuManager.m_strokeWidth);
		GraphicsPath path;
		path.AddString(previewText, -1, m_overlayDlg.m_danmakuManager.m_danmakuFont.get(), 
			FontStyle::FontStyleBold, m_overlayDlg.m_danmakuManager.m_danmakuSize, 
			PointF(rect.Width() / 2.0f, rect.Height() / 2.0f), &stringFormat);
		graphics.DrawPath(&strokePen, &path);
	}

	// 绘制填充文本
	graphics.DrawString(previewText, -1, &font, textRect, &stringFormat, &textBrush);

	pPreviewWnd->ReleaseDC(pDC);
}

//// 显示颜色选择对话框
//COLORREF CDanmakuChickenDlg::ShowColorDialog(COLORREF initialColor)
//{
//	CColorDialog colorDlg(initialColor, CC_FULLOPEN);
//	if (colorDlg.DoModal() == IDOK) {
//		return colorDlg.GetColor();
//	}
//	return initialColor;
//}

// 处理标签页切换
void CDanmakuChickenDlg::OnTcnSelchangeStyleTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int newTab = m_styleTabCtrl.GetCurSel();
	if (newTab != m_currentTab) {
		m_currentTab = newTab;
		
		// 隐藏所有控件
		GetDlgItem(IDC_STATIC_SIZE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SIZE_SLIDER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPEED_SLIDER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_OPACITY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OPACITY_SLIDER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FONT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FONT_COMBO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FONT_SIZE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FONT_SIZE_SLIDER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_TEXT_COLOR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TEXT_COLOR_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STROKE_ENABLE_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STROKE_COLOR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STROKE_COLOR_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_OPACITY_STYLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OPACITY_SLIDER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PREVIEW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PREVIEW_AREA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_TEST)->ShowWindow(SW_HIDE);
		
		// 根据选中的标签页显示对应的控件
		if (newTab == 0) {
			// 显示基本设置控件
			GetDlgItem(IDC_STATIC_SIZE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SIZE_SLIDER)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SPEED_SLIDER)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_OPACITY)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_OPACITY_SLIDER)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_TEST)->ShowWindow(SW_SHOW);
		} else if (newTab == 1) {
			// 显示样式设置控件
			GetDlgItem(IDC_STATIC_FONT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FONT_COMBO)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_FONT_SIZE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_FONT_SIZE_SLIDER)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_TEXT_COLOR)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXT_COLOR_BUTTON)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STROKE_ENABLE_CHECK)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_STROKE_COLOR)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STROKE_COLOR_BUTTON)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_OPACITY_STYLE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_OPACITY_SLIDER)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_PREVIEW)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_PREVIEW_AREA)->ShowWindow(SW_SHOW);
		}
	}
	*pResult = 0;
}

// 处理字体选择变化
void CDanmakuChickenDlg::OnCbnSelchangeFontCombo()
{
	int selectedIndex = m_fontComboBox.GetCurSel();
	if (selectedIndex != CB_ERR) {
		CString fontName;
		m_fontComboBox.GetLBText(selectedIndex, fontName);
		m_overlayDlg.m_danmakuManager.m_danmakuFontName = fontName;
		
		// 更新字体
		try {
			m_overlayDlg.m_danmakuManager.m_danmakuFont = make_unique<FontFamily>(fontName.GetBuffer());
		} catch (...) {
			// 字体创建失败，使用默认字体
			m_overlayDlg.m_danmakuManager.m_danmakuFontName = L"黑体";
			m_overlayDlg.m_danmakuManager.m_danmakuFont = make_unique<FontFamily>(L"黑体");
			m_fontComboBox.SetCurSel(m_fontComboBox.FindStringExact(-1, L"黑体"));
		}
		
		UpdatePreview();
	}
}

// 处理文字颜色按钮点击
void CDanmakuChickenDlg::OnBnClickedTextColor()
{
	//COLORREF newColor = ShowColorDialog(m_textColor);
	//if (newColor != m_textColor) {
	//	m_textColor = newColor;
	//	m_overlayDlg.m_danmakuManager.m_textColor = Color(GetRValue(newColor), GetGValue(newColor), GetBValue(newColor));
	//	UpdatePreview();
	//}
}

// 处理描边开关变化
void CDanmakuChickenDlg::OnBnClickedStrokeEnable()
{
	BOOL strokeEnabled = (m_strokeEnableCheckbox.GetCheck() == BST_CHECKED);
	m_overlayDlg.m_danmakuManager.m_strokeEnabled = strokeEnabled;
	m_strokeColorButton.EnableWindow(strokeEnabled);
	UpdatePreview();
}

// 处理描边颜色按钮点击
void CDanmakuChickenDlg::OnBnClickedStrokeColor()
{
	//if (m_overlayDlg.m_danmakuManager.m_strokeEnabled) {
	//	COLORREF newColor = ShowColorDialog(m_strokeColor);
	//	if (newColor != m_strokeColor) {
	//		m_strokeColor = newColor;
	//		m_overlayDlg.m_danmakuManager.m_strokeColor = Color(GetRValue(newColor), GetGValue(newColor), GetBValue(newColor));
	//		UpdatePreview();
	//	}
	//}
}

// 测试弹幕
void CDanmakuChickenDlg::OnBnClickedButton2()
{
	m_overlayDlg.m_danmakuManager.AddDanmaku(_T("测试弹幕"));
}

// 处理添加弹幕请求
void CDanmakuChickenDlg::HandleAddDanmaku(shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request)
{
	try
	{
		ptree pt;
		read_json(request->content, pt);

		string contentA = pt.get<string>("content");
		wstring_convert<codecvt<wchar_t, char, mbstate_t> > cv(new codecvt_utf8_utf16<wchar_t>);
		wstring contentW = cv.from_bytes(contentA);

		m_overlayDlg.m_danmakuManager.AddDanmaku(contentW.c_str());

		*response << "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
	}
	catch(exception& e)
	{
		stringstream stream;
		stream << R"({ "error": ")" << e.what() << R"(" })";
		string content = stream.str();
		*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.size() << "\r\n\r\n"
			<< content;
	}
}
