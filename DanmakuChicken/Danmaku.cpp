#include "stdafx.h" 
#include "Danmaku.h" 
#include <Afx.h> 
#include <fstream> 
#include <ShlObj.h> 
#include <Shlwapi.h> 
#pragma comment(lib, "Shlwapi.lib") 
#include <sstream> 
#include <Windows.h> 
using namespace std; 
using namespace Gdiplus; 


Danmaku::Danmaku(const CString& content, const FontFamily* font, REAL size)
{
	m_content = content;

	GraphicsPath path;
	path.AddString(m_content, -1, font, FontStyle::FontStyleBold, size, Point(0, 0), StringFormat::GenericDefault());

	Pen blackPen(Color::Black, 1.5F);
	Rect rect;
	path.GetBounds(&rect, NULL, &blackPen);
	m_size.Width = rect.Width;
	m_size.Height = rect.Height;

	m_img.Create(m_size.Width, m_size.Height, 32, CImage::createAlphaChannel);
	Graphics graph(m_img.GetDC());
	graph.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
	SolidBrush whiteBrush(Color::White);
	graph.FillPath(&whiteBrush, &path);
	graph.DrawPath(&blackPen, &path);
	m_img.ReleaseDC();
}

Danmaku::Danmaku(Danmaku&& other)
{
	*this = move(other);
}

Danmaku& Danmaku::operator= (Danmaku&& other)
{
	m_content = move(other.m_content);
	m_pos = move(other.m_pos);
	m_size = move(other.m_size);
	m_img.Destroy();
	m_img.Attach(other.m_img.Detach());
	return *this;
}


DanmakuManager::DanmakuManager() :
	m_danmakuFont(make_unique<FontFamily>(L"黑体"))
{
	// 更新弹幕线程
	m_updateThread = thread([this] {
		while (!m_stopThreads)
		{
			UpdateDanmaku();
			Sleep(1000 / 60); // 60fps
		}
	});
}

DanmakuManager::~DanmakuManager() 
{ 
	m_stopThreads = TRUE; 
	if (m_updateThread.joinable()) 
		m_updateThread.join(); 
} 

// 获取配置文件路径 
CString DanmakuManager::GetConfigFilePath() 
{ 
	TCHAR szPath[MAX_PATH]; 
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) 
	{ 
		CString strPath(szPath); 
		strPath += _T("\\DanmakuChicken"); 
		CreateDirectory(strPath, NULL); 
		strPath += _T("\\config.ini"); 
		return strPath; 
	} 
	return _T("config.ini"); 
} 

// 获取日志文件路径 
CString DanmakuManager::GetLogFilePath() 
{ 
	TCHAR szPath[MAX_PATH]; 
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) 
	{ 
		CString strPath(szPath); 
		strPath += _T("\\DanmakuChicken"); 
		CreateDirectory(strPath, NULL); 
		strPath += _T("\\danmaku.log"); 
		return strPath; 
	} 
	return _T("danmaku.log"); 
} 

// 写入日志（CString版本） 
void DanmakuManager::WriteLog(const CString& strLog) 
{ 
	try 
	{ 
		CString strFilePath = GetLogFilePath(); 
		CTime currentTime = CTime::GetCurrentTime(); 
		CString strTime = currentTime.Format(_T("%Y-%m-%d %H:%M:%S")); 
		
		CStdioFile file; 
		if (file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::shareDenyNone)) 
		{ 
			file.SeekToEnd(); 
			file.WriteString(strTime + _T(" ") + strLog + _T("\n")); 
			file.Close(); 
		} 
	} 
	catch (const CFileException& e) 
	{ 
		CString strError; 
		strError.Format(_T("日志写入失败: %d"), e.m_cause); 
		OutputDebugString(strError + _T("\n")); 
	} 
	catch (const exception& e) 
	{ 
		stringstream ss; 
		ss << "日志写入失败: " << e.what() << endl; 
		OutputDebugStringA(ss.str().c_str()); 
	} 
} 



// 加载配置文件 
BOOL DanmakuManager::LoadConfig() 
{ 
	try 
	{ 
		CString strFilePath = GetConfigFilePath(); 
		
		// 读取配置参数 
		m_danmakuSize = (REAL)GetPrivateProfileInt(_T("Danmaku"), _T("Size"), 40, strFilePath); 
		m_danmakuSpeed = GetPrivateProfileInt(_T("Danmaku"), _T("Speed"), 4, strFilePath); 
		m_danmakuAlpha = GetPrivateProfileInt(_T("Danmaku"), _T("Alpha"), 255 * 80 / 100, strFilePath); 
		
		CString strLog = _T("配置文件加载成功"); 
		WriteLog(strLog); 
		OutputDebugString(strLog + _T("\n")); 
		return TRUE; 
	} 
	catch (const exception& e) 
	{ 
		stringstream ss; 
		ss << "配置文件加载失败: " << e.what() << endl; 
		WriteLog(CString(ss.str().c_str())); 
		OutputDebugStringA(ss.str().c_str()); 
		return FALSE; 
	} 
} 

// 保存配置文件 
BOOL DanmakuManager::SaveConfig() 
{ 
	try 
	{ 
		CString strFilePath = GetConfigFilePath(); 
		CString strValue; 
		
		// 保存配置参数 
		strValue.Format(_T("%f"), m_danmakuSize); 
		WritePrivateProfileString(_T("Danmaku"), _T("Size"), strValue, strFilePath); 
		
		strValue.Format(_T("%d"), m_danmakuSpeed); 
		WritePrivateProfileString(_T("Danmaku"), _T("Speed"), strValue, strFilePath); 
		
		strValue.Format(_T("%d"), m_danmakuAlpha); 
		WritePrivateProfileString(_T("Danmaku"), _T("Alpha"), strValue, strFilePath); 
		
		CString strLog = _T("配置文件保存成功"); 
		WriteLog(strLog); 
		OutputDebugString(strLog + _T("\n")); 
		return TRUE; 
	} 
	catch (const exception& e) 
	{ 
		stringstream ss; 
		ss << "配置文件保存失败: " << e.what() << endl; 
		WriteLog(CString(ss.str().c_str())); 
		OutputDebugStringA(ss.str().c_str()); 
		return FALSE; 
	} 
}

// 添加新弹幕
void DanmakuManager::AddDanmaku(const CString& content)
{
	Danmaku danmaku(content, m_danmakuFont.get(), m_danmakuSize);
	danmaku.m_pos.X = m_danmakuBoxSize.Width;
	danmaku.m_pos.Y = 0;

	// 寻找不被遮挡的Y坐标
	BOOL hasCollision;
	do
	{
		hasCollision = FALSE;
		lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
		for (const auto& i : m_danmakuSet)
		{
			// 有碰撞
			if (i.m_pos.X + i.m_size.Width > danmaku.m_pos.X && i.m_pos.X < danmaku.m_pos.X + danmaku.m_size.Width
				&& i.m_pos.Y + i.m_size.Height > danmaku.m_pos.Y && i.m_pos.Y < danmaku.m_pos.Y + danmaku.m_size.Height)
			{
				danmaku.m_pos.Y = i.m_pos.Y + i.m_size.Height;
				hasCollision = TRUE;
				break;
			}
		}
	} while (hasCollision && danmaku.m_pos.Y + danmaku.m_size.Height <= m_danmakuBoxSize.Height);

	lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	m_danmakuSet.push_back(move(danmaku));
}

// 更新弹幕位置
void DanmakuManager::UpdateDanmaku()
{
	lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	for (auto it = m_danmakuSet.begin(); it != m_danmakuSet.end(); )
	{
		it->m_pos.X -= m_danmakuSpeed;
		if (it->m_pos.X + it->m_size.Width <= 0) // 删除边界外的弹幕
			it = m_danmakuSet.erase(it);
		else
			++it;
	}
}

// 渲染m_danmakuSet到hdc
void DanmakuManager::RenderDanmakuSet(HDC hdc)
{
	lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	for (const auto& i : m_danmakuSet)
		i.m_img.AlphaBlend(hdc, i.m_pos.X, i.m_pos.Y, m_danmakuAlpha, AC_SRC_OVER);
}
