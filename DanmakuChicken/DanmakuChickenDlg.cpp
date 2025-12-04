
// DanmakuChickenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DanmakuChicken.h"
#include "DanmakuChickenDlg.h"
#include "Danmaku.h"
#include "OverlayDlg.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <shlwapi.h>
#include <codecvt>
#pragma comment(lib, "shlwapi.lib")
using namespace boost::property_tree;
using namespace std;

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
	DDX_Control(pDX, IDC_SLIDER1, m_danmakuSizeSlider);
	DDX_Control(pDX, IDC_SLIDER2, m_danmakuSpeedSlider);
	DDX_Control(pDX, IDC_SLIDER3, m_danmakuOpacitySlider);
	DDX_Control(pDX, IDC_TAB1, m_tabControl);
	DDX_Control(pDX, IDC_COMBO_FONT, m_fontCombo);
	DDX_Control(pDX, IDC_SLIDER_FONT_SIZE, m_fontSizeSlider);
	DDX_Control(pDX, IDC_BUTTON_TEXT_COLOR, m_textColorButton);
	DDX_Control(pDX, IDC_CHECK_STROKE, m_strokeCheck);
	DDX_Control(pDX, IDC_BUTTON_STROKE_COLOR, m_strokeColorButton);
	DDX_Control(pDX, IDC_SLIDER_OPACITY, m_opacitySlider);
	DDX_Control(pDX, IDC_PREVIEW_STATIC, m_previewStatic);
}

BEGIN_MESSAGE_MAP(CDanmakuChickenDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &CDanmakuChickenDlg::OnBnClickedButton2)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_FONT, &CDanmakuChickenDlg::OnSelchangeComboFont)
	ON_BN_CLICKED(IDC_BUTTON_TEXT_COLOR, &CDanmakuChickenDlg::OnBnClickedButtonTextColor)
	ON_BN_CLICKED(IDC_BUTTON_STROKE_COLOR, &CDanmakuChickenDlg::OnBnClickedButtonStrokeColor)
	ON_BN_CLICKED(IDC_CHECK_STROKE, &CDanmakuChickenDlg::OnBnClickedCheckStroke)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);		// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 初始化标签页
	m_tabControl.InsertItem(0, _T("样式设置"));
	
	// 创建样式设置对话框
	m_stylePage.Create(IDD_STYLE_PAGE, &m_tabControl);
	
	// 获取标签页的客户区域
	CRect tabRect;
	m_tabControl.GetClientRect(&tabRect);
	tabRect.top += 20; // 标签页标题高度
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.bottom -= 1;
	m_stylePage.MoveWindow(&tabRect);
	m_stylePage.ShowWindow(SW_SHOW);

	// 初始化字体下拉框
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	
	HDC hdc = GetDC()->GetSafeHdc();
	EnumFontFamiliesEx(hdc, &logFont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)this, 0);
	ReleaseDC(GetDC());
	
	// 初始化滑块控件
	m_fontSizeSlider.SetRange(10, 50);
	m_fontSizeSlider.SetPos((int)m_overlayDlg.m_danmakuManager.m_danmakuSize);
	m_opacitySlider.SetRange(0, 100);
	m_opacitySlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuAlpha * 100 / 255);
	
	// 初始化描边复选框
	m_strokeCheck.SetCheck(m_overlayDlg.m_danmakuManager.m_strokeEnabled ? BST_CHECKED : BST_UNCHECKED);
	
	// 加载设置
	LoadSettings();

	// 更新预览区域
	UpdatePreview();
	SaveSettings();

	// 载入弹幕窗口
	m_overlayDlg.Create(m_overlayDlg.IDD, GetDesktopWindow());

	// 启动服务器
	m_serverThread = thread([this] {
		m_server.config.address = "127.0.0.1";
		m_server.config.port = 12450;
		m_server.resource["^/danmaku"]["POST"] = bind(&CDanmakuChickenDlg::HandleAddDanmaku, 
			this, placeholders::_1, placeholders::_2);
		m_server.start();
	});

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
}

// 修改弹幕设置
void CDanmakuChickenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBTRACK)
	{
		switch (pScrollBar->GetDlgCtrlID())
		{
		case IDC_SLIDER1:
			m_overlayDlg.m_danmakuManager.m_danmakuSize = (float)nPos;
			break;
		case IDC_SLIDER2:
			m_overlayDlg.m_danmakuManager.m_danmakuSpeed = nPos;
			break;
		case IDC_SLIDER3:
			m_overlayDlg.m_danmakuManager.m_danmakuAlpha = nPos;
			break;
		case IDC_SLIDER_FONT_SIZE:
			m_overlayDlg.m_danmakuManager.m_danmakuSize = (float)nPos;
			UpdatePreview();
			break;
		case IDC_SLIDER_OPACITY:
			m_overlayDlg.m_danmakuManager.m_danmakuAlpha = nPos * 255 / 100;
			UpdatePreview();
			break;
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

// 字体选择改变
void CDanmakuChickenDlg::OnSelchangeComboFont()
{
	int selectedIndex = m_fontCombo.GetCurSel();
	if (selectedIndex != CB_ERR)
	{
		CString fontName;
		m_fontCombo.GetLBText(selectedIndex, fontName);
		m_overlayDlg.m_danmakuManager.m_danmakuFontName = fontName;
		m_overlayDlg.m_danmakuManager.m_danmakuFont.reset(new Gdiplus::FontFamily(fontName.GetString()));
		UpdatePreview();
	}
}

// 选择文字颜色
void CDanmakuChickenDlg::OnBnClickedButtonTextColor()
{
	CColorDialog colorDialog(m_overlayDlg.m_danmakuManager.m_textColor, CC_FULLOPEN | CC_RGBINIT);
	if (colorDialog.DoModal() == IDOK)
	{
		m_overlayDlg.m_danmakuManager.m_textColor = colorDialog.GetColor();
		UpdatePreview();
		SaveSettings();
	}
}

// 选择描边颜色
void CDanmakuChickenDlg::OnBnClickedButtonStrokeColor()
{
	CColorDialog colorDialog(m_overlayDlg.m_danmakuManager.m_strokeColor, CC_FULLOPEN | CC_RGBINIT);
	if (colorDialog.DoModal() == IDOK)
	{
		m_overlayDlg.m_danmakuManager.m_strokeColor = colorDialog.GetColor();
		UpdatePreview();
		SaveSettings();
	}
}

// 描边开关改变
void CDanmakuChickenDlg::OnBnClickedCheckStroke()
{
	m_overlayDlg.m_danmakuManager.m_strokeEnabled = (m_strokeCheck.GetCheck() == BST_CHECKED);
	UpdatePreview();
}

// 更新预览区域
void CDanmakuChickenDlg::UpdatePreview()
{
	CRect previewRect;
	m_previewStatic.GetClientRect(&previewRect);
	CDC* pDC = m_previewStatic.GetDC();
	
	// 清除预览区域
	pDC->FillSolidRect(&previewRect, RGB(255, 255, 255));
	
	// 创建临时弹幕对象用于预览
	// 因为Danmaku类的构造函数是protected的，所以我们需要通过DanmakuManager来创建
	m_overlayDlg.m_danmakuManager.AddDanmaku(_T("预览弹幕效果"));
	Danmaku& previewDanmaku = m_overlayDlg.m_danmakuManager.m_danmakuSet.back();
	// 移除临时弹幕对象
	m_overlayDlg.m_danmakuManager.m_danmakuSet.pop_back();
	
	// 计算居中位置
	int x = (previewRect.Width() - previewDanmaku.m_size.Width) / 2;
	int y = (previewRect.Height() - previewDanmaku.m_size.Height) / 2;
	
	// 绘制预览弹幕
	previewDanmaku.m_img.AlphaBlend(pDC->GetSafeHdc(), x, y, m_overlayDlg.m_danmakuManager.m_danmakuAlpha, AC_SRC_OVER);
	
	m_previewStatic.ReleaseDC(pDC);
}

// 枚举字体回调函数
int CALLBACK CDanmakuChickenDlg::EnumFontFamExProc(ENUMLOGFONTEX* lpelfe, NEWTEXTMETRICEX* lpntme, DWORD FontType, LPARAM lParam)
{
	CDanmakuChickenDlg* pDlg = (CDanmakuChickenDlg*)lParam;
	CString fontName = lpelfe->elfLogFont.lfFaceName;
	
	// 避免重复添加字体
	if (pDlg->m_fontCombo.FindStringExact(-1, fontName) == CB_ERR)
	{
		pDlg->m_fontCombo.AddString(fontName);
		pDlg->m_systemFonts.push_back(fontName);
	}
	
	return 1; // 继续枚举
}

// 保存设置到文件
void CDanmakuChickenDlg::SaveSettings()
{
	try
	{
		ptree pt;
		
		// 保存弹幕设置
		pt.put("danmaku.speed", m_overlayDlg.m_danmakuManager.m_danmakuSpeed);
		
		// 保存样式设置
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::string fontNameStr = converter.to_bytes(m_overlayDlg.m_danmakuManager.m_danmakuFontName.GetString());
		pt.put("style.font_name", fontNameStr);
		pt.put("style.font_size", m_overlayDlg.m_danmakuManager.m_danmakuSize);
		pt.put("style.text_color", GetRValue(m_overlayDlg.m_danmakuManager.m_textColor) << 16 | GetGValue(m_overlayDlg.m_danmakuManager.m_textColor) << 8 | GetBValue(m_overlayDlg.m_danmakuManager.m_textColor));
		pt.put("style.stroke_enabled", m_overlayDlg.m_danmakuManager.m_strokeEnabled);
		pt.put("style.stroke_color", GetRValue(m_overlayDlg.m_danmakuManager.m_strokeColor) << 16 | GetGValue(m_overlayDlg.m_danmakuManager.m_strokeColor) << 8 | GetBValue(m_overlayDlg.m_danmakuManager.m_strokeColor));
		pt.put("style.opacity", m_overlayDlg.m_danmakuManager.m_danmakuAlpha);
		
		// 获取配置文件路径
		TCHAR szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		CString strPath(szPath);
		int nPos = strPath.ReverseFind(_T('\\'));
		if (nPos != -1)
		{
			strPath = strPath.Left(nPos + 1);
		}
		strPath += _T("settings.json");
		
		// 保存到文件
		std::string path = CT2A(strPath.GetString());
		write_json(path, pt);
	}
	catch (exception& e)
	{
		// 保存失败，忽略错误
	}
}

// 从文件加载设置
void CDanmakuChickenDlg::LoadSettings()
{
	try
	{
		// 获取配置文件路径
		TCHAR szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		CString strPath(szPath);
		int nPos = strPath.ReverseFind(_T('\\'));
		if (nPos != -1)
		{
			strPath = strPath.Left(nPos + 1);
		}
		strPath += _T("settings.json");
		
		// 检查文件是否存在
		if (!PathFileExists(strPath))
		{
			return; // 文件不存在，使用默认设置
		}
		
		// 从文件加载设置
		ptree pt;
		std::string path = CT2A(strPath.GetString());
		read_json(path, pt);
		
		// 加载弹幕设置
		m_overlayDlg.m_danmakuManager.m_danmakuSpeed = pt.get<int>("danmaku.speed", 4);
		
		// 加载样式设置
		string fontNameStr = pt.get<string>("style.font_name", "黑体");
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter2;
		std::wstring fontNameWStr = converter2.from_bytes(fontNameStr);
		CString fontName(fontNameWStr.c_str());
		m_overlayDlg.m_danmakuManager.m_danmakuFontName = fontName;
		m_overlayDlg.m_danmakuManager.m_danmakuFont.reset(new Gdiplus::FontFamily(fontName.GetString()));
		
		m_overlayDlg.m_danmakuManager.m_danmakuSize = pt.get<REAL>("style.font_size", 40.0F);
		DWORD textColorVal = pt.get<DWORD>("style.text_color", 0xFFFFFF);
		m_overlayDlg.m_danmakuManager.m_textColor = RGB((textColorVal >> 16) & 0xFF, (textColorVal >> 8) & 0xFF, textColorVal & 0xFF);
		m_overlayDlg.m_danmakuManager.m_strokeEnabled = pt.get<BOOL>("style.stroke_enabled", TRUE);
		DWORD strokeColorVal = pt.get<DWORD>("style.stroke_color", 0x000000);
		m_overlayDlg.m_danmakuManager.m_strokeColor = RGB((strokeColorVal >> 16) & 0xFF, (strokeColorVal >> 8) & 0xFF, strokeColorVal & 0xFF);
		m_overlayDlg.m_danmakuManager.m_danmakuAlpha = pt.get<int>("style.opacity", 255 * 80 / 100);
		
		// 更新控件状态
		m_danmakuSizeSlider.SetPos((int)m_overlayDlg.m_danmakuManager.m_danmakuSize);
		m_danmakuSpeedSlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuSpeed);
		m_danmakuOpacitySlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuAlpha);
		
		m_fontSizeSlider.SetPos((int)m_overlayDlg.m_danmakuManager.m_danmakuSize);
		m_opacitySlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuAlpha * 100 / 255);
		m_strokeCheck.SetCheck(m_overlayDlg.m_danmakuManager.m_strokeEnabled ? BST_CHECKED : BST_UNCHECKED);
		
		// 设置字体选择
		int fontIndex = m_fontCombo.FindStringExact(-1, fontName);
		if (fontIndex != CB_ERR)
		{
			m_fontCombo.SetCurSel(fontIndex);
		}
		
		// 更新预览区域
		UpdatePreview();
	}
	catch (exception& e)
	{
		// 加载失败，使用默认设置
	}
}

// 测试弹幕
void CDanmakuChickenDlg::OnBnClickedButton2()
{
	m_overlayDlg.m_danmakuManager.AddDanmaku(_T("我能吞下玻璃而不伤身体"));
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
