#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <Windows.h>

// 配置管理类
class ConfigManager
{
public:
    // 单例模式
    static ConfigManager& GetInstance()
    {
        static ConfigManager instance;
        return instance;
    }

    // 弹幕配置结构
    struct DanmakuConfig
    {
        float fontSize = 40.0f;
        int speed = 4;
        int opacity = 255 * 80 / 100; // 80%
    };

    // 获取当前配置
    const DanmakuConfig& GetConfig() const { return m_config; }

    // 设置配置
    void SetConfig(const DanmakuConfig& config) { m_config = config; }

    // 加载配置
    void LoadConfig(const std::wstring& configPath);

    // 保存配置
    void SaveConfig(const std::wstring& configPath) const;

    // 获取配置文件路径
    std::wstring GetConfigPath() const { return m_configPath; }

private:
    ConfigManager() {}
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    DanmakuConfig m_config;
    std::wstring m_configPath;

    // 记录日志
    void Log(const std::string& message) const;
};