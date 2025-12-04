#include "stdafx.h"
#include "ConfigManager.h"
#include <codecvt>
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

using namespace std;

// 加载配置
void ConfigManager::LoadConfig(const std::wstring& configPath)
{
    m_configPath = configPath;

    try
    {
        // 尝试从ini文件中读取配置
        wchar_t buf[256] = {0};
        if (GetPrivateProfileStringW(L"danmaku", L"fontSize", L"40.0", buf, 256, configPath.c_str()))
        {
            m_config.fontSize = static_cast<float>(_wtof(buf));
        }
        if (GetPrivateProfileStringW(L"danmaku", L"speed", L"4", buf, 256, configPath.c_str()))
        {
            m_config.speed = static_cast<int>(_wtoi(buf));
        }
        if (GetPrivateProfileStringW(L"danmaku", L"opacity", L"204", buf, 256, configPath.c_str()))
        {
            m_config.opacity = static_cast<int>(_wtoi(buf));
        }

        // 验证配置值的有效性
        if (m_config.fontSize < 10 || m_config.fontSize > 100)
            m_config.fontSize = 40.0f;
        if (m_config.speed < 1 || m_config.speed > 100)
            m_config.speed = 4;
        if (m_config.opacity < 25 || m_config.opacity > 255)
            m_config.opacity = 204;

        Log("Config loaded successfully");
    }
    catch (const exception& e)
    {
        Log(string("Failed to load config: ") + e.what() + ", using default configuration");
        // 使用默认配置
        m_config = DanmakuConfig();
    }
}

// 保存配置
void ConfigManager::SaveConfig(const std::wstring& configPath) const
{
    try
    {
        // 创建配置目录
        wstring dirPath = configPath.substr(0, configPath.find_last_of(L"\\/"));
        CreateDirectory(dirPath.c_str(), NULL);

        // 将配置写入ini文件
        wchar_t buf[256] = {0};

        // 写入弹幕大小
        swprintf_s(buf, 256, L"%.1f", m_config.fontSize);
        WritePrivateProfileStringW(L"danmaku", L"fontSize", buf, configPath.c_str());

        // 写入弹幕速度
        swprintf_s(buf, 256, L"%d", m_config.speed);
        WritePrivateProfileStringW(L"danmaku", L"speed", buf, configPath.c_str());

        // 写入弹幕不透明度
        swprintf_s(buf, 256, L"%d", m_config.opacity);
        WritePrivateProfileStringW(L"danmaku", L"opacity", buf, configPath.c_str());

        Log("Config saved successfully");
    }
    catch (const exception& e)
    {
        Log(string("Failed to save config: ") + e.what());
    }
}

// 记录日志
void ConfigManager::Log(const std::string& message) const
{
    // 输出到控制台
    cout << "[ConfigManager] " << message << endl;

    // 输出到文件
    try
    {
        wstring logPath = m_configPath;
        size_t pos = logPath.find_last_of(L"\\/");
        if (pos != wstring::npos)
        {
            logPath = logPath.substr(0, pos) + L"\\config.log";
        }
        else
        {
            logPath = L"config.log";
        }

        // 打开日志文件（追加模式）
        string logPathNarrow;
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        logPathNarrow = conv.to_bytes(logPath);
        ofstream logFile(logPathNarrow.c_str(), ios::app);
        if (logFile.is_open())
        {
            // 获取当前时间
            time_t now = time(NULL);
            tm localTime;
            localtime_s(&localTime, &now);

            // 格式化时间
            char timeBuf[64] = {0};
            strftime(timeBuf, 64, "%Y-%m-%d %H:%M:%S", &localTime);

            // 写入日志
            logFile << timeBuf << " [ConfigManager] " << message << endl;
            logFile.close();
        }
    }
    catch (const exception& e)
    {
        // 如果日志写入失败，只输出到控制台
        cout << "[ConfigManager] Failed to write log: " << e.what() << endl;
    }
}