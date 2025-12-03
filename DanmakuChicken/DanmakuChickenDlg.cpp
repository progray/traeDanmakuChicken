
// DanmakuChickenDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DanmakuChicken.h"
#include "DanmakuChickenDlg.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;
#include <codecvt>
using namespace std;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CDanmakuChickenDlg �Ի���



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
}

BEGIN_MESSAGE_MAP(CDanmakuChickenDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BUTTON2, &CDanmakuChickenDlg::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON1, &CDanmakuChickenDlg::OnBnClickedButtonOpenConfig)
ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CDanmakuChickenDlg ��Ϣ��������

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDanmakuChickenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDanmakuChickenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDanmakuChickenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_danmakuSizeSlider.SetRange(20, 100);
	m_danmakuSpeedSlider.SetRange(1, 100);
	m_danmakuOpacitySlider.SetRange(255 * 10 / 100, 255);
	
	// ��������
	LoadConfig();
	
	// ���»���λ��
	m_danmakuSizeSlider.SetPos((int)m_overlayDlg.m_danmakuManager.m_danmakuSize);
	m_danmakuSpeedSlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuSpeed);
	m_danmakuOpacitySlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuAlpha);

	// ���뵯Ļ����
	m_overlayDlg.Create(m_overlayDlg.IDD, GetDesktopWindow());

	// ����������
	m_serverThread = thread([this] {
		m_server.config.address = "127.0.0.1";
		m_server.config.port = 12450;
		m_server.resource["^/danmaku"]["POST"] = bind(&CDanmakuChickenDlg::HandleAddDanmaku, 
			this, placeholders::_1, placeholders::_2);
		m_server.start();
	});

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDanmakuChickenDlg::OnDestroy()
{
	// ��������
	SaveConfig();
	
	CDialogEx::OnDestroy();

	// ֹͣ������
	m_server.stop();
	if (m_serverThread.joinable())
		m_serverThread.join();

	// �رյ�Ļ����
	m_overlayDlg.DestroyWindow();
}

// �޸ĵ�Ļ����
void CDanmakuChickenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_ENDSCROLL)
	{
		switch (pScrollBar->GetDlgCtrlID())
		{
		case IDC_SLIDER1:
			m_overlayDlg.m_danmakuManager.m_danmakuSize = (float)nPos;
			SaveConfig();
			break;
		case IDC_SLIDER2:
			m_overlayDlg.m_danmakuManager.m_danmakuSpeed = nPos;
			SaveConfig();
			break;
		case IDC_SLIDER3:
			m_overlayDlg.m_danmakuManager.m_danmakuAlpha = nPos;
			SaveConfig();
			break;
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

// ���Ե�Ļ
void CDanmakuChickenDlg::OnBnClickedButton2()
{
	m_overlayDlg.m_danmakuManager.AddDanmaku(_T("�������²�������������"));
}

// �������ӵ�Ļ����
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
		stream << "{ \"error\": \"" << e.what() << "\" }";
		//stream << R"({ "error": ")" << e.what() << R"(" })";
		string content = stream.str();
		*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.size() << "\r\n\r\n"
			<< content;
	}
}

// ��ȡ�����ļ�·��
CString CDanmakuChickenDlg::GetConfigFilePath()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	int nPos = strPath.ReverseFind(_T('\\'));
	if (nPos != -1)
	{
		strPath = strPath.Left(nPos + 1);
	}
	strPath += _T("danmaku_config.json");
	return strPath;
}

// 记录配置操作日志
void CDanmakuChickenDlg::LogConfigOperation(const CString& operation, bool success, const CString& errorMsg)
{
	CString strLog;
	CTime timeNow = CTime::GetCurrentTime();
	strLog.Format(_T("[%s] %s %s"), timeNow.Format(_T("%Y-%m-%d %H:%M:%S")), operation, success ? _T("成功") : _T("失败"));
	
	if (!success && !errorMsg.IsEmpty())
	{
		strLog += _T(": ") + errorMsg;
	}
	
	strLog += _T("\r\n");
	
	// 写入日志文件为UTF-8格式
	CString strLogPath = GetConfigFilePath();
	strLogPath.Replace(_T("danmaku_config.json"), _T("danmaku_log.txt"));
	
	bool bIsNewFile = !PathFileExists(strLogPath);
	CStdioFile file;
	if (file.Open(strLogPath, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::typeText | CFile::shareDenyWrite))
	{
		file.SeekToEnd();
		
		// 如果是新文件，写入UTF-8 BOM
		if (bIsNewFile)
		{
			BYTE utf8Bom[] = {0xEF, 0xBB, 0xBF};
			file.Write(utf8Bom, sizeof(utf8Bom));
		}
		
		// 将CString转换为UTF-8写入
		file.WriteString(strLog);
		file.Close();
	}
}

// ��������
void CDanmakuChickenDlg::LoadConfig()
{
	CString strConfigPath = GetConfigFilePath();
	
	try
	{
		if (PathFileExists(strConfigPath))
{
	ptree pt;
	std::string configPathStr = CT2A(strConfigPath.GetString());
	read_json(configPathStr, pt);
			
			// ��ȡ���ã�ʹ��Ĭ��ֵ��Ϊ��
			m_overlayDlg.m_danmakuManager.m_danmakuSize = pt.get<float>("danmaku_size", 40.0F);
			m_overlayDlg.m_danmakuManager.m_danmakuSpeed = pt.get<int>("danmaku_speed", 4);
			m_overlayDlg.m_danmakuManager.m_danmakuAlpha = pt.get<int>("danmaku_alpha", 255 * 80 / 100);
			
			// ȷ������ֵ����Ч��Χ��
			m_overlayDlg.m_danmakuManager.m_danmakuSize = max(20.0F, min(100.0F, m_overlayDlg.m_danmakuManager.m_danmakuSize));
			m_overlayDlg.m_danmakuManager.m_danmakuSpeed = max(1, min(100, m_overlayDlg.m_danmakuManager.m_danmakuSpeed));
			m_overlayDlg.m_danmakuManager.m_danmakuAlpha = max(255 * 10 / 100, min(255, m_overlayDlg.m_danmakuManager.m_danmakuAlpha));
			
			LogConfigOperation(_T("��������"), true);
		}
		else
		{
			// ʹ��Ĭ�����ò�����
			SaveConfig();
			LogConfigOperation(_T("配置文件不存在，创建默认配置"), true);
		}
	}
	catch(exception& e)
	{
		CString strError;
		CString errorMsg = CA2T(e.what());
		strError.Format(_T("加载配置文件失败: %s"), errorMsg.GetString());
		LogConfigOperation(_T("加载配置"), false, strError);
		
		// �ָ�Ĭ������
		m_overlayDlg.m_danmakuManager.m_danmakuSize = 40.0F;
		m_overlayDlg.m_danmakuManager.m_danmakuSpeed = 4;
		m_overlayDlg.m_danmakuManager.m_danmakuAlpha = 255 * 80 / 100;
	}
}

// ��������
void CDanmakuChickenDlg::SaveConfig()
{
	CString strConfigPath = GetConfigFilePath();
	
	try
	{
		ptree pt;
		pt.put("danmaku_size", m_overlayDlg.m_danmakuManager.m_danmakuSize);
		pt.put("danmaku_speed", m_overlayDlg.m_danmakuManager.m_danmakuSpeed);
		pt.put("danmaku_alpha", m_overlayDlg.m_danmakuManager.m_danmakuAlpha);
		
		std::string configPathStr = CT2A(strConfigPath.GetString());
	write_json(configPathStr, pt);
	LogConfigOperation(_T("保存配置"), true);
}
catch(exception& e)
{
	CString strError;
	CString errorMsg = CA2T(e.what());
	strError.Format(_T("保存配置失败: %s"), errorMsg.GetString());
		LogConfigOperation(_T("保存配置"), false, strError);
	}
}

// �������ļ�
void CDanmakuChickenDlg::OnBnClickedButtonOpenConfig()
{
	CString strConfigPath = GetConfigFilePath();
	
	if (PathFileExists(strConfigPath))
	{
		ShellExecute(NULL, _T("open"), strConfigPath, NULL, NULL, SW_SHOWNORMAL);
		LogConfigOperation(_T("�������ļ�"), true);
	}
	else
	{
		// ����Ĭ������
		SaveConfig();
		ShellExecute(NULL, _T("open"), strConfigPath, NULL, NULL, SW_SHOWNORMAL);
		LogConfigOperation(_T("�������������ļ�"), true);
	}
}
