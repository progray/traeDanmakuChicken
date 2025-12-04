#include "stdafx.h"
#include "Danmaku.h"
using namespace std;
using namespace Gdiplus;


Danmaku::Danmaku(const CString& content, const FontFamily* font, REAL size, COLORREF textColor, BOOL strokeEnabled, COLORREF strokeColor)
{
	m_content = content;

	GraphicsPath path;
	path.AddString(m_content, -1, font, FontStyle::FontStyleBold, size, PointF(0.0f, 0.0f), StringFormat::GenericDefault());

	Pen strokePen(Color(strokeColor), 1.5F);
	RectF rect;
	path.GetBounds(&rect, NULL, &strokePen);
	m_size.Width = rect.Width;
	m_size.Height = rect.Height;

	m_img.Create(static_cast<int>(ceil(rect.Width)), static_cast<int>(ceil(rect.Height)), 32, CImage::createAlphaChannel);
	Graphics graph(m_img.GetDC());
	graph.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
	
	// 设置文字颜色
	Color gdiTextColor(GetRValue(textColor), GetGValue(textColor), GetBValue(textColor), 255);
	SolidBrush textBrush(gdiTextColor);
	
	// 先绘制描边（如果开启）
	if (strokeEnabled)
	{
		Color gdiStrokeColor(GetRValue(strokeColor), GetGValue(strokeColor), GetBValue(strokeColor), 255);
		Pen strokePen(gdiStrokeColor, 1.5F);
		graph.DrawPath(&strokePen, &path);
	}
	
	// 再绘制文字
	graph.FillPath(&textBrush, &path);
	
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

// 添加新弹幕
void DanmakuManager::AddDanmaku(const CString& content)
{
	Danmaku danmaku(content, m_danmakuFont.get(), m_danmakuSize, m_textColor, m_strokeEnabled, m_strokeColor);
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
	} while (hasCollision && danmaku.m_pos.Y + danmaku.m_size.Height < m_danmakuBoxSize.Height);

	lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	m_danmakuSet.push_back(danmaku);
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
