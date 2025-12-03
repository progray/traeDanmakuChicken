
// DanmakuChickenDlg.cpp : 实现文件
//
#include "stdafx.h" 
#include "DanmakuChicken.h" 
#include "DanmakuChickenDlg.h" 
#include "../include/json.hpp" 
#include <codecvt> 
#include <ShellAPI.h> 
#include <Shlwapi.h> 
#include <sstream> 
#pragma comment(lib, "Shlwapi.lib") 
using namespace std; 
using json = nlohmann::json;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CDanmakuChickenDlg 对话框



CDanmakuChickenDlg::CDanmakuChickenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDanmakuChickenDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_overlayDlg = new COverlayDlg();
}

void CDanmakuChickenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_danmakuSizeSlider);
	DDX_Control(pDX, IDC_SLIDER2, m_danmakuSpeedSlider);
	DDX_Control(pDX, IDC_SLIDER3, m_danmakuOpacitySlider);
}

BEGIN_MESSAGE_MAP(CDanmakuChickenDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &CDanmakuChickenDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CONFIG, &CDanmakuChickenDlg::OnBnClickedOpenConfigFile)
	ON_WM_HSCROLL()
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
	SetIcon(m_hIcon, FALSE);	// 设置小图标 

	// 加载配置文件 
	m_overlayDlg->m_danmakuManager.LoadConfig(); 

	m_danmakuSizeSlider.SetRange(20, 100); 
	m_danmakuSizeSlider.SetPos((int)m_overlayDlg->m_danmakuManager.m_danmakuSize); 
	m_danmakuSpeedSlider.SetRange(1, 100); 
	m_danmakuSpeedSlider.SetPos(m_overlayDlg->m_danmakuManager.m_danmakuSpeed); 
	m_danmakuOpacitySlider.SetRange(255 * 10 / 100, 255); 
	m_danmakuOpacitySlider.SetPos(m_overlayDlg->m_danmakuManager.m_danmakuAlpha); 

	// 载入弹幕窗口 
	m_overlayDlg->Create(m_overlayDlg->IDD, GetDesktopWindow()); 

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

	// 保存配置文件 
	m_overlayDlg->m_danmakuManager.SaveConfig(); 

	// 停止服务器 
	m_server.stop(); 
	if (m_serverThread.joinable()) 
		m_serverThread.join(); 

	// 关闭弹幕窗口 
	m_overlayDlg->DestroyWindow(); 
	delete m_overlayDlg;
	m_overlayDlg = NULL;
}

// 修改弹幕设置 
void CDanmakuChickenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (nSBCode == SB_THUMBTRACK) 
	{
		switch (pScrollBar->GetDlgCtrlID()) 
		{
		case IDC_SLIDER1: 
			m_overlayDlg->m_danmakuManager.m_danmakuSize = (float)nPos; 
			break; 
		case IDC_SLIDER2: 
			m_overlayDlg->m_danmakuManager.m_danmakuSpeed = nPos; 
			break; 
		case IDC_SLIDER3: 
			m_overlayDlg->m_danmakuManager.m_danmakuAlpha = nPos; 
			break; 
		}
		
		// 弹幕设置修改时，保存到配置文件 
		m_overlayDlg->m_danmakuManager.SaveConfig(); 
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar); 
}

// 测试弹幕
void CDanmakuChickenDlg::OnBnClickedButton2()
{
	m_overlayDlg->m_danmakuManager.AddDanmaku(_T("测试弹幕"));
}

// 打开配置文件
void CDanmakuChickenDlg::OnBnClickedOpenConfigFile()
{
	CString strConfigPath = m_overlayDlg->m_danmakuManager.GetConfigFilePath();
	
	// 检查配置文件是否存在，如果不存在则创建
	if (!PathFileExists(strConfigPath))
	{
		m_overlayDlg->m_danmakuManager.SaveConfig();
	}
	
	// 使用默认程序打开配置文件
	ShellExecute(NULL, _T("open"), strConfigPath, NULL, NULL, SW_SHOW);
}

// 处理添加弹幕请求
void CDanmakuChickenDlg::HandleAddDanmaku(shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request)
{
	try
	{
		// 使用nlohmann/json库解析JSON数据
		string content = request->content.string();
		json j = json::parse(content);
		string contentA = j["content"];


		// 转换为宽字符
		wstring_convert<codecvt<wchar_t, char, mbstate_t> > cv(new codecvt_utf8_utf16<wchar_t>);
		wstring contentW = cv.from_bytes(contentA);

		m_overlayDlg->m_danmakuManager.AddDanmaku(contentW.c_str());

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
