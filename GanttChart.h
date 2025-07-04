#ifndef GANTT_CHART_H
#define GANTT_CHART_H

#include "data.h"
#include "GanttConfig.h" // 使用配置文件
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cctype>
#include <chrono>
#include <mutex>

// ========== 调试宏定义（使用配置文件控制）==========
#if GanttConfig::DebugConfig::ENABLE_CSV_DEBUG
#define CSV_DEBUG(x) do { std::cout << "[CSV] " << x << std::endl; } while(0)
#define CSV_DEBUG_DETAILED(x) do { std::cout << "[CSV_DETAIL] " << x << std::endl; } while(0)
#else
#define CSV_DEBUG(x) do {} while(0)
#define CSV_DEBUG_DETAILED(x) do {} while(0)
#endif

#if GanttConfig::DebugConfig::ENABLE_DATA_ANALYSIS
#define DATA_DEBUG(x) do { std::cout << "[DATA] " << x << std::endl; } while(0)
#define OVERLAP_DEBUG(x) do { std::cout << "[OVERLAP] " << x << std::endl; } while(0)
#else
#define DATA_DEBUG(x) do {} while(0)
#define OVERLAP_DEBUG(x) do {} while(0)
#endif

#if GanttConfig::DebugConfig::ENABLE_DRAWING_DEBUG
#define DRAW_DEBUG(x) do { std::cout << "[DRAW] " << x << std::endl; } while(0)
#else
#define DRAW_DEBUG(x) do {} while(0)
#endif

#if GanttConfig::DebugConfig::ENABLE_RANGE_DEBUG
#define RANGE_DEBUG(x) do { std::cout << "[RANGE] " << x << std::endl; } while(0)
#else
#define RANGE_DEBUG(x) do {} while(0)
#endif

#if GanttConfig::DebugConfig::ENABLE_PERFORMANCE_DEBUG
#define PERF_DEBUG(x) do { std::cout << "[PERF] " << x << std::endl; } while(0)
#else
#define PERF_DEBUG(x) do {} while(0)
#endif

// 通用调试宏
#define INFO(x) do { std::cout << "[INFO] " << x << std::endl; } while(0)
#define ERROR(x) do { std::cerr << "[ERROR] " << x << std::endl; } while(0)
#define WARNING(x) do { std::cout << "[WARNING] " << x << std::endl; } while(0)

// 甘特图绘制类
class GanttChart {
private:
    sf::RenderWindow& window;
    sf::Font font;
    std::vector<qm::Operation> operations;
    std::map<int, sf::Color> jobColors;
    bool debugOutputShown = false;  // 添加标志，避免重复输出调试信息

    // 图表参数 - 从配置文件读取默认值
    float leftMargin;
    float topMargin;
    float bottomMargin;
    float rightMargin;
    float barHeight;
    float barSpacing;

    // 时间和机器范围
    int minTime, maxTime;
    int minMachine, maxMachine;
    float timeScale;

    // 缩放和平移参数 - 从配置文件读取默认值
    float zoomLevel;
    float offsetX;
    float offsetY;

    // 线程安全
    mutable std::mutex dataMutex;
    bool isInitialized = false;

public:
    explicit GanttChart(sf::RenderWindow& win);

    // === 主要接口 ===

    /**
     * @brief 初始化空状态的甘特图
     * @param fontPath 字体文件路径（可选，使用配置文件默认值）
     * @return 成功返回true，失败返回false
     */
    bool init(const std::string& fontPath = "");

    /**
     * @brief 清空前一个显示，绘制当前的显示（update功能）
     * @param newData 新的操作数据
     * @param immediate_draw 是否立即绘制到窗口（默认true）
     * @return 成功返回true，失败返回false
     */
    bool update(const qm::Data& newData, bool immediate_draw = true);

    /**
     * @brief 清空前一个显示，绘制当前的显示（update功能）- 重载版本
     * @param newOperations 新的操作数据向量
     * @param immediate_draw 是否立即绘制到窗口（默认true）
     * @return 成功返回true，失败返回false
     */
    bool update(const std::vector<qm::Operation>& newOperations, bool immediate_draw = true);

    // === 原有功能接口（保持兼容性）===

    /**
     * @brief 直接设置数据
     */
    void setData(const std::vector<qm::Operation>& ops);

    /**
     * @brief 从CSV文件加载数据并更新显示
     * @param filename CSV文件路径
     * @return 成功返回true
     */
    bool loadFromCSV(const std::string& filename);

    /**
     * @brief 绘制甘特图
     */
    void draw();

    /**
     * @brief 打印已加载的数据进行验证
     */
    void printLoadedData();

    /**
     * @brief 重置调试标志
     */
    void resetDebugFlag();

    /**
     * @brief 处理鼠标滚轮缩放
     */
    void handleMouseWheel(float delta);

    /**
     * @brief 处理键盘平移
     */
    void handleKeyboard();

    /**
     * @brief 重置视图
     */
    void resetView();

    /**
     * @brief 处理SFML事件
     */
    void handleEvent(const sf::Event& event);

    // === 查询接口 ===

    /**
     * @brief 获取当前操作数量
     */
    size_t getOperationCount() const;

    /**
     * @brief 检查是否有重叠操作
     */
    bool hasOverlaps() const;

    /**
     * @brief 获取时间范围
     */
    void getTimeRange(int& minT, int& maxT) const;

    /**
     * @brief 获取机器范围
     */
    void getMachineRange(int& minM, int& maxM) const;

    // === 配置相关接口 ===

    /**
     * @brief 应用自定义布局配置
     */
    void applyLayoutConfig(float leftM, float topM, float bottomM, float rightM);

    /**
     * @brief 应用自定义视图配置
     */
    void applyViewConfig(float zoom, float panSpeed);

    /**
     * @brief 获取当前配置信息
     */
    void printCurrentConfig() const;

private:
    // === 私有方法 ===

    // 从配置文件加载默认参数
    void loadDefaultConfig();

    // 尝试加载字体文件
    bool tryLoadFont(const std::string& fontPath);

    // 计算时间和机器的范围，并动态调整布局
    void calculateRanges();

    // 为每个job_id生成颜色
    void generateJobColors();

    // HSV转RGB
    sf::Color hsvToRgb(float h, float s, float v);

    // 获取机器在屏幕上的Y坐标
    float getMachineY(int machineId);

    // 获取时间在屏幕上的X坐标
    float getTimeX(int time);

    // 添加缩放和平移控制
    void handleZoom(float delta);
    void handlePan(float deltaX, float deltaY);

    // 绘制相关方法
    void drawAxes();
    void drawOperations();
    void drawLabels();

    // 检查数据中的重叠问题
    void checkOverlaps();
};

// === 工具函数 ===

/**
 * @brief 配置SFML窗口设置
 * @param window 要配置的窗口引用
 */
void configureWindow(sf::RenderWindow& window);

/**
 * @brief 获取窗口创建设置
 * @return 配置好的上下文设置
 */
sf::ContextSettings getWindowContextSettings();

/**
 * @brief 从CSV文件加载操作数据
 * @param filename CSV文件路径
 * @return 操作数据向量
 */
std::vector<qm::Operation> loadOperationsFromCSV(const std::string& filename);

/**
 * @brief 检查操作重叠的独立函数
 * @param operations 操作数据
 */
void checkOperationsOverlap(const std::vector<qm::Operation>& operations);

/**
 * @brief 打印vector中的操作数据
 * @param operations 操作数据
 */
void printOperationsVector(const std::vector<qm::Operation>& operations);

/**
 * @brief 创建测试数据
 * @param jobCount 作业数量
 * @param stagesPerJob 每个作业的阶段数
 * @param machineCount 机器数量
 * @return 测试操作数据
 */
std::vector<qm::Operation> createTestData(int jobCount, int stagesPerJob, int machineCount);

#endif // GANTT_CHART_H