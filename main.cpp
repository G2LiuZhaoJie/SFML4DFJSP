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

// ========== 调试开关配置 ==========
// 修改这些宏的值来控制调试输出
// 1 = 开启, 0 = 关闭

#if 0
#define ENABLE_CSV_DEBUG        // CSV读取调试信息
#endif

#if 0
#define ENABLE_DATA_ANALYSIS    // 数据分析和重叠检测
#endif

#if 0
#define ENABLE_DRAWING_DEBUG    // 绘制过程调试信息
#endif

#if 1
#define ENABLE_RANGE_DEBUG      // 范围计算调试信息
#endif

#if 0
#define ENABLE_PERFORMANCE_DEBUG // 性能调试信息
#endif

// ========== 调试宏定义 ==========
#ifdef ENABLE_CSV_DEBUG
#define CSV_DEBUG(x) do { std::cout << "[CSV] " << x << std::endl; } while(0)
#define CSV_DEBUG_DETAILED(x) do { std::cout << "[CSV_DETAIL] " << x << std::endl; } while(0)
#else
#define CSV_DEBUG(x) do {} while(0)
#define CSV_DEBUG_DETAILED(x) do {} while(0)
#endif

#ifdef ENABLE_DATA_ANALYSIS
#define DATA_DEBUG(x) do { std::cout << "[DATA] " << x << std::endl; } while(0)
#define OVERLAP_DEBUG(x) do { std::cout << "[OVERLAP] " << x << std::endl; } while(0)
#else
#define DATA_DEBUG(x) do {} while(0)
#define OVERLAP_DEBUG(x) do {} while(0)
#endif

#ifdef ENABLE_DRAWING_DEBUG
#define DRAW_DEBUG(x) do { std::cout << "[DRAW] " << x << std::endl; } while(0)
#else
#define DRAW_DEBUG(x) do {} while(0)
#endif

#ifdef ENABLE_RANGE_DEBUG
#define RANGE_DEBUG(x) do { std::cout << "[RANGE] " << x << std::endl; } while(0)
#else
#define RANGE_DEBUG(x) do {} while(0)
#endif

#ifdef ENABLE_PERFORMANCE_DEBUG
#define PERF_DEBUG(x) do { std::cout << "[PERF] " << x << std::endl; } while(0)
#else
#define PERF_DEBUG(x) do {} while(0)
#endif

// 通用调试宏
#define INFO(x) do { std::cout << "[INFO] " << x << std::endl; } while(0)
#define ERROR(x) do { std::cerr << "[ERROR] " << x << std::endl; } while(0)
#define WARNING(x) do { std::cout << "[WARNING] " << x << std::endl; } while(0)

// 定义操作结构体
struct MyOp {
    int job_id;
    int stage;
    int machine_id;
    int start_time;
    int end_time;
    bool is_critical;
    double some_other_field;
};

// 甘特图绘制类
class GanttChart {
private:
    sf::RenderWindow& window;
    sf::Font font;
    std::vector<MyOp> operations;
    std::map<int, sf::Color> jobColors;
    bool debugOutputShown = false;  // 添加标志，避免重复输出调试信息

    // 图表参数 - 针对大数据集优化
    float leftMargin = 60.0f;
    float topMargin = 30.0f;
    float bottomMargin = 60.0f;
    float rightMargin = 30.0f;
    float barHeight = 25.0f;      // 减小条高度以容纳更多机器
    float barSpacing = 2.0f;      // 减小间距

    // 时间和机器范围
    int minTime, maxTime;
    int minMachine, maxMachine;
    float timeScale;

    // 缩放和平移参数
    float zoomLevel = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

public:
    GanttChart(sf::RenderWindow& win) : window(win) {
        // 尝试加载字体
        if (!font.loadFromFile("arial.ttf")) {
            // 如果加载失败，使用默认字体
            std::cout << "Warning: Could not load font file. Using default font." << std::endl;
        }
    }

    // 直接设置数据（用于测试）
    void setData(const std::vector<MyOp>& ops) {
        operations = ops;
        calculateRanges();
        generateJobColors();
        DATA_DEBUG("=== Test Data Loaded ===");
        printLoadedData();
    }

    // 打印已加载的数据进行验证
    void printLoadedData() {
#ifdef ENABLE_DATA_ANALYSIS
        std::cout << "[DATA] === Loaded Operations Data ===" << std::endl;
        std::cout << "[DATA] Format: [Index] Job-Stage | Machine | Time | Critical | Other" << std::endl;
        std::cout << "[DATA] ---------------------------------------------------------------" << std::endl;

        for (size_t i = 0; i < operations.size(); ++i) {
            const auto& op = operations[i];
            std::cout << "[DATA] [" << std::setw(2) << i << "] "
                << "J" << op.job_id << "-" << op.stage << " | "
                << "M" << op.machine_id << " | "
                << "[" << std::setw(2) << op.start_time << "-" << std::setw(2) << op.end_time << "] | "
                << (op.is_critical ? "CRITICAL" : "normal  ") << " | "
                << op.some_other_field << std::endl;
        }

        std::cout << "[DATA] === Range Analysis ===" << std::endl;
        std::cout << "[DATA] Time range: " << minTime << " - " << maxTime << " (duration: " << (maxTime - minTime) << ")" << std::endl;
        std::cout << "[DATA] Machine range: " << minMachine << " - " << maxMachine << " (count: " << (maxMachine - minMachine + 1) << ")" << std::endl;
        std::cout << "[DATA] Time scale: " << timeScale << " pixels per time unit" << std::endl;

        // 检查重叠问题
        checkOverlaps();

        std::cout << "[DATA] === Job Colors ===" << std::endl;
        for (const auto& pair : jobColors) {
            std::cout << "[DATA] Job " << pair.first << ": RGB("
                << static_cast<int>(pair.second.r) << ", "
                << static_cast<int>(pair.second.g) << ", "
                << static_cast<int>(pair.second.b) << ")" << std::endl;
        }
#endif
    }

    // 检查数据中的重叠问题
    void checkOverlaps() {
        OVERLAP_DEBUG("=== Overlap Analysis ===");

        // 按机器分组检查重叠
        std::map<int, std::vector<MyOp>> machineOps;
        for (const auto& op : operations) {
            machineOps[op.machine_id].push_back(op);
        }

        bool hasOverlaps = false;

#ifdef ENABLE_DATA_ANALYSIS
        for (auto& pair : machineOps) {
            int machineId = pair.first;
            auto& ops = pair.second;

            // 按开始时间排序
            std::sort(ops.begin(), ops.end(), [](const MyOp& a, const MyOp& b) {
                return a.start_time < b.start_time;
                });

            std::cout << "[OVERLAP] Machine " << machineId << " operations:" << std::endl;
            for (size_t i = 0; i < ops.size(); ++i) {
                const auto& op = ops[i];
                std::cout << "[OVERLAP]   J" << op.job_id << "-" << op.stage
                    << " [" << op.start_time << "-" << op.end_time << "]";

                // 检查与下一个操作的重叠
                if (i + 1 < ops.size()) {
                    const auto& nextOp = ops[i + 1];
                    if (op.end_time > nextOp.start_time) {
                        std::cout << " ！！  OVERLAP with J" << nextOp.job_id << "-" << nextOp.stage
                            << " [" << nextOp.start_time << "-" << nextOp.end_time << "]";
                        hasOverlaps = true;
                    }
                }
                std::cout << std::endl;
            }
        }
#endif

        if (!hasOverlaps) {
            INFO("√ No time overlaps detected on any machine.");
        }
        else {
            WARNING("！！  Time overlaps detected! This may cause visual issues.");
        }
    }

private:
    // 计算时间和机器的范围，并动态调整布局
    void calculateRanges() {
        if (operations.empty()) return;

        minTime = maxTime = operations[0].start_time;
        minMachine = maxMachine = operations[0].machine_id;

        for (const auto& op : operations) {
            minTime = std::min(minTime, op.start_time);
            maxTime = std::max(maxTime, op.end_time);
            minMachine = std::min(minMachine, op.machine_id);
            maxMachine = std::max(maxMachine, op.machine_id);
        }

        // 根据机器数量动态调整布局参数
        int machineCount = maxMachine - minMachine + 1;
        float availableHeight = window.getSize().y - topMargin - bottomMargin;

        RANGE_DEBUG("Machine count: " << machineCount << ", Available height: " << availableHeight);

        if (machineCount <= 3) {
            // 很少机器时：大条形，宽间距
            barHeight = std::min(80.0f, availableHeight / machineCount * 0.7f);
            barSpacing = std::min(20.0f, availableHeight / machineCount * 0.3f);
            RANGE_DEBUG("Layout: Very few machines - luxury display");
        }
        else if (machineCount <= 6) {
            // 少量机器时：中等条形，适中间距
            barHeight = std::min(60.0f, availableHeight / machineCount * 0.75f);
            barSpacing = std::min(15.0f, availableHeight / machineCount * 0.25f);
            RANGE_DEBUG("Layout: Few machines - comfortable display");
        }
        else if (machineCount <= 10) {
            // 中等数量机器：平衡显示
            barHeight = std::min(45.0f, availableHeight / machineCount * 0.8f);
            barSpacing = std::min(8.0f, availableHeight / machineCount * 0.2f);
            RANGE_DEBUG("Layout: Medium machines - balanced display");
        }
        else if (machineCount <= 15) {
            // 较多机器：紧凑显示
            barHeight = std::min(30.0f, availableHeight / machineCount * 0.85f);
            barSpacing = std::min(5.0f, availableHeight / machineCount * 0.15f);
            RANGE_DEBUG("Layout: Many machines - compact display");
        }
        else {
            // 大量机器：超紧凑显示，按可用空间平均分配
            float totalSpace = availableHeight / machineCount;
            barHeight = std::max(12.0f, totalSpace * 0.9f);  // 最小12像素高度
            barSpacing = std::max(1.0f, totalSpace * 0.1f);  // 最小1像素间距
            RANGE_DEBUG("Layout: Very many machines - ultra-compact display");
        }

        // 确保布局参数合理
        barHeight = std::max(10.0f, std::min(100.0f, barHeight));
        barSpacing = std::max(1.0f, std::min(30.0f, barSpacing));

        // 计算时间缩放比例 - 考虑缩放级别
        float chartWidth = (window.getSize().x - leftMargin - rightMargin) * zoomLevel;
        if (maxTime > minTime) {
            timeScale = chartWidth / static_cast<float>(maxTime - minTime);
        }
        else {
            timeScale = 1.0f;
        }

        RANGE_DEBUG("=== Range Calculation Results ===");
        RANGE_DEBUG("Time: " << minTime << " - " << maxTime << " (span: " << (maxTime - minTime) << ")");
        RANGE_DEBUG("Machines: " << minMachine << " - " << maxMachine << " (count: " << machineCount << ")");
        RANGE_DEBUG("Layout: barHeight=" << barHeight << ", barSpacing=" << barSpacing);
        RANGE_DEBUG("Chart width: " << chartWidth << ", Time scale: " << timeScale);

        // 验证总高度是否适合窗口
        float totalRequiredHeight = machineCount * barHeight + (machineCount - 1) * barSpacing;
        if (totalRequiredHeight > availableHeight) {
            WARNING("Total required height (" << totalRequiredHeight
                << ") exceeds available height (" << availableHeight << ")");
        }
        else {
            RANGE_DEBUG("Layout fits well: used " << totalRequiredHeight
                << "/" << availableHeight << " = "
                << (totalRequiredHeight / availableHeight * 100) << "%");
        }
    }

    // 为每个job_id生成颜色
    void generateJobColors() {
        jobColors.clear();
        std::vector<int> uniqueJobs;

        for (const auto& op : operations) {
            if (std::find(uniqueJobs.begin(), uniqueJobs.end(), op.job_id) == uniqueJobs.end()) {
                uniqueJobs.push_back(op.job_id);
            }
        }

        // 生成不同的颜色
        for (size_t i = 0; i < uniqueJobs.size(); ++i) {
            float hue = (360.0f * i) / uniqueJobs.size();
            sf::Color color = hsvToRgb(hue, 0.7f, 0.9f);
            jobColors[uniqueJobs[i]] = color;
        }
    }

    // HSV转RGB
    sf::Color hsvToRgb(float h, float s, float v) {
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h / 60.0f, 2) - 1));
        float m = v - c;

        float r, g, b;
        if (h >= 0 && h < 60) {
            r = c; g = x; b = 0;
        }
        else if (h >= 60 && h < 120) {
            r = x; g = c; b = 0;
        }
        else if (h >= 120 && h < 180) {
            r = 0; g = c; b = x;
        }
        else if (h >= 180 && h < 240) {
            r = 0; g = x; b = c;
        }
        else if (h >= 240 && h < 300) {
            r = x; g = 0; b = c;
        }
        else {
            r = c; g = 0; b = x;
        }

        return sf::Color(
            static_cast<sf::Uint8>((r + m) * 255),
            static_cast<sf::Uint8>((g + m) * 255),
            static_cast<sf::Uint8>((b + m) * 255)
        );
    }

    // 获取机器在屏幕上的Y坐标
    float getMachineY(int machineId) {
        int machineIndex = machineId - minMachine;
        return topMargin + offsetY + machineIndex * (barHeight + barSpacing);
    }

    // 获取时间在屏幕上的X坐标
    float getTimeX(int time) {
        return leftMargin + offsetX + (time - minTime) * timeScale;
    }

    // 添加缩放和平移控制
    void handleZoom(float delta) {
        float oldZoom = zoomLevel;
        zoomLevel = std::max(0.1f, std::min(5.0f, zoomLevel + delta));

        if (zoomLevel != oldZoom) {
            calculateRanges();
            std::cout << "Zoom level: " << zoomLevel << std::endl;
        }
    }

    void handlePan(float deltaX, float deltaY) {
        offsetX += deltaX;
        offsetY += deltaY;

        // 限制平移范围
        float maxOffsetX = std::max(0.0f, (maxTime - minTime) * timeScale - (window.getSize().x - leftMargin - rightMargin));
        float maxOffsetY = std::max(0.0f, (maxMachine - minMachine + 1) * (barHeight + barSpacing) - (window.getSize().y - topMargin - bottomMargin));

        offsetX = std::max(-maxOffsetX, std::min(0.0f, offsetX));
        offsetY = std::max(-maxOffsetY, std::min(0.0f, offsetY));
    }

public:
    // 绘制甘特图
    void draw() {
        window.clear(sf::Color::White);

        drawAxes();
        drawOperations();
        drawLabels();

        window.display();
    }

    // 重置调试标志
    void resetDebugFlag() {
        debugOutputShown = false;
    }

    // 处理鼠标滚轮缩放
    void handleMouseWheel(float delta) {
        handleZoom(delta * 0.1f);
    }

    // 处理键盘平移
    void handleKeyboard() {
        float panSpeed = 20.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            handlePan(panSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            handlePan(-panSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            handlePan(0, panSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            handlePan(0, -panSpeed);
        }
    }

    // 重置视图
    void resetView() {
        zoomLevel = 1.0f;
        offsetX = 0.0f;
        offsetY = 0.0f;
        calculateRanges();
        std::cout << "View reset to default" << std::endl;
    }

private:
    // 绘制坐标轴
    void drawAxes() {
        // 绘制Y轴（机器轴）
        sf::RectangleShape yAxis(sf::Vector2f(2, window.getSize().y - topMargin - bottomMargin));
        yAxis.setPosition(leftMargin - 1, topMargin);
        yAxis.setFillColor(sf::Color::Black);
        window.draw(yAxis);

        // 绘制X轴（时间轴）
        sf::RectangleShape xAxis(sf::Vector2f(window.getSize().x - leftMargin - rightMargin, 2));
        xAxis.setPosition(leftMargin, window.getSize().y - bottomMargin);
        xAxis.setFillColor(sf::Color::Black);
        window.draw(xAxis);
    }

    // 绘制操作块
    void drawOperations() {
        bool shouldShowDebug = !debugOutputShown;

        if (shouldShowDebug) {
            DRAW_DEBUG("=== Drawing Operations ===");
            DRAW_DEBUG("Zoom: " << zoomLevel << ", Offset: (" << offsetX << ", " << offsetY << ")");
            DRAW_DEBUG("Visible operations (showing first 5):");
        }

        int drawnCount = 0;
        int visibleCount = 0;

#ifdef ENABLE_PERFORMANCE_DEBUG
        auto startTime = std::chrono::high_resolution_clock::now();
#endif

        for (size_t i = 0; i < operations.size(); ++i) {
            const auto& op = operations[i];
            float x = getTimeX(op.start_time);
            float y = getMachineY(op.machine_id);
            float width = (op.end_time - op.start_time) * timeScale;

            // 检查是否在可见区域内
            if (x + width < 0 || x > window.getSize().x ||
                y + barHeight < 0 || y > window.getSize().y) {
                continue; // 跳过不可见的操作
            }

            visibleCount++;

#ifdef ENABLE_DRAWING_DEBUG
            if (shouldShowDebug && drawnCount < 5) {
                std::cout << "[DRAW] J" << op.job_id << "-" << op.stage << " | "
                    << "M" << op.machine_id << " | "
                    << "(" << std::setw(6) << std::fixed << std::setprecision(1) << x << ","
                    << std::setw(6) << std::fixed << std::setprecision(1) << y << ") | "
                    << "size(" << std::setw(6) << std::fixed << std::setprecision(1) << width << ","
                    << std::setw(4) << barHeight << ") | "
                    << "[" << op.start_time << "-" << op.end_time << "]" << std::endl;
            }
#endif

            // 绘制操作矩形
            sf::RectangleShape rect(sf::Vector2f(std::max(1.0f, width), barHeight));
            rect.setPosition(x, y);
            rect.setFillColor(jobColors[op.job_id]);

            // 设置边框
            if (op.is_critical) {
                rect.setOutlineThickness(2);
                rect.setOutlineColor(sf::Color::Red);
            }
            else {
                rect.setOutlineThickness(1);
                rect.setOutlineColor(sf::Color::Black);
            }

            window.draw(rect);
            drawnCount++;

            // 绘制标签文本 - 只有当矩形足够大时
            if (width > 30 && barHeight > 15) {
                std::string label = "J" + std::to_string(op.job_id) + "-" + std::to_string(op.stage);

                // 根据矩形大小选择字体大小
                int fontSize = 10;
                if (width > 80) fontSize = 12;
                if (width > 120) fontSize = 14;

                sf::Text text(label, font, fontSize);
                text.setFillColor(sf::Color::Black);

                // 计算文本居中位置
                sf::FloatRect textBounds = text.getLocalBounds();
                if (textBounds.width < width - 4) {
                    float textX = x + (width - textBounds.width) / 2;
                    float textY = y + (barHeight - textBounds.height) / 2 - 2;
                    text.setPosition(textX, textY);
                    window.draw(text);
                }
            }
        }

        if (shouldShowDebug) {
            PERF_DEBUG("Visible operations: " << visibleCount << "/" << operations.size());
            PERF_DEBUG("Actually drawn: " << drawnCount);
            debugOutputShown = true;

#ifdef ENABLE_PERFORMANCE_DEBUG
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            std::cout << "[PERF] Drawing time: " << duration.count() << " microseconds" << std::endl;
#endif
        }
    }

    // 绘制标签
    void drawLabels() {
        // 绘制机器标签
        for (int machine = minMachine; machine <= maxMachine; ++machine) {
            float y = getMachineY(machine);

            // 只绘制可见的机器标签
            if (y < -20 || y > window.getSize().y + 20) continue;

            std::string label = "M" + std::to_string(machine);
            sf::Text text(label, font, 10);
            text.setFillColor(sf::Color::Black);

            text.setPosition(5, y + barHeight / 2 - 6);
            window.draw(text);
        }

        // 绘制时间标签 - 动态调整步长
        int totalTimeSpan = maxTime - minTime;
        int labelCount = (window.getSize().x - leftMargin - rightMargin) / 80; // 每80像素一个标签
        int timeStep = std::max(1, totalTimeSpan / labelCount);

        // 确保步长是合理的数值
        if (timeStep < 50) timeStep = 50;
        else if (timeStep < 100) timeStep = 100;
        else if (timeStep < 200) timeStep = 200;
        else if (timeStep < 500) timeStep = 500;
        else timeStep = ((timeStep + 499) / 500) * 500; // 向上取整到500的倍数

        for (int time = minTime; time <= maxTime; time += timeStep) {
            float x = getTimeX(time);

            // 只绘制可见的时间标签
            if (x < leftMargin - 50 || x > window.getSize().x) continue;

            std::string label = std::to_string(time);
            sf::Text text(label, font, 10);
            text.setFillColor(sf::Color::Black);

            sf::FloatRect textBounds = text.getLocalBounds();
            text.setPosition(x - textBounds.width / 2, window.getSize().y - bottomMargin + 5);
            window.draw(text);

            // 绘制时间刻度线
            sf::RectangleShape tick(sf::Vector2f(1, 5));
            tick.setPosition(x, window.getSize().y - bottomMargin);
            tick.setFillColor(sf::Color::Black);
            window.draw(tick);
        }

        // 绘制轴标题
        sf::Text yAxisTitle("Machine", font, 12);
        yAxisTitle.setFillColor(sf::Color::Black);
        yAxisTitle.setPosition(5, 5);
        window.draw(yAxisTitle);

        sf::Text xAxisTitle("Time", font, 12);
        xAxisTitle.setFillColor(sf::Color::Black);
        xAxisTitle.setPosition(window.getSize().x / 2 - 20, window.getSize().y - 20);
        window.draw(xAxisTitle);

        // 绘制缩放信息
        std::string zoomInfo = "Zoom: " + std::to_string(static_cast<int>(zoomLevel * 100)) + "%";
        sf::Text zoomText(zoomInfo, font, 10);
        zoomText.setFillColor(sf::Color::Blue);
        zoomText.setPosition(window.getSize().x - 100, 5);
        window.draw(zoomText);
    }
};

// CSV读取函数
std::vector<MyOp> loadOperationsFromCSV(const std::string& filename) {
    std::vector<MyOp> operations;
    std::ifstream file(filename);

    if (!file.is_open()) {
        ERROR("Could not open file " << filename);
        return operations;
    }

    std::string line;
    bool isFirstLine = true;
    int lineNumber = 0;

    CSV_DEBUG("=== CSV Reading Started ===");
    CSV_DEBUG("Reading from file: " << filename);

    while (std::getline(file, line)) {
        lineNumber++;

        // 去除行末的回车符
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // 跳过标题行，但要检查格式
        if (isFirstLine) {
            CSV_DEBUG("Header line: " << line);

            // 检查是否是预期的格式
            if (line.find("ID") != std::string::npos && line.find("Job") != std::string::npos) {
                CSV_DEBUG("√ Detected your CSV format (ID,Job,Operation,Machine,StartTime,EndTime,IsCritical)");
            }
            else if (line.find("job_id") != std::string::npos) {
                CSV_DEBUG("√ Detected standard format (job_id,stage,machine_id,...)");
            }
            else {
                WARNING("Unknown header format, will attempt to parse...");
            }

            isFirstLine = false;
            continue;
        }

        // 跳过空行
        if (line.empty()) {
            continue;
        }

        CSV_DEBUG_DETAILED("Line " << lineNumber << ": " << line);

        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        // 解析CSV行
        while (std::getline(ss, cell, ',')) {
            // 去除前后空格和引号
            cell.erase(0, cell.find_first_not_of(" \t\""));
            cell.erase(cell.find_last_not_of(" \t\"") + 1);
            row.push_back(cell);
        }

#ifdef ENABLE_CSV_DEBUG
        if (lineNumber <= 6) { // 只显示前几行的详细信息
            std::cout << "[CSV_DETAIL] Parsed " << row.size() << " cells: ";
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << "[" << i << "]=\"" << row[i] << "\" ";
            }
            std::cout << std::endl;
        }
#endif

        // 检查列数
        if (row.size() < 6) {
            if (lineNumber <= 10) {
                ERROR("Line " << lineNumber << " has insufficient columns (" << row.size() << " < 6)");
            }
            continue;
        }

        // 解析数据
        MyOp op;
        try {
            // CSV格式：ID,Job,Operation,Machine,StartTime,EndTime,IsCritical
            int id = std::stoi(row[0]);           // ID (用作索引)
            op.job_id = std::stoi(row[1]);        // Job -> job_id
            op.stage = std::stoi(row[2]);         // Operation -> stage  
            op.machine_id = std::stoi(row[3]);    // Machine -> machine_id
            op.start_time = std::stoi(row[4]);    // StartTime -> start_time
            op.end_time = std::stoi(row[5]);      // EndTime -> end_time
            op.is_critical = (std::stoi(row[6]) == 1);  // IsCritical -> is_critical (1表示true)
            op.some_other_field = static_cast<double>(id);  // 使用ID作为other字段

            operations.push_back(op);

            CSV_DEBUG_DETAILED("√ Operation added: Job=" << op.job_id
                << ", Stage=" << op.stage
                << ", Machine=" << op.machine_id
                << ", Time=[" << op.start_time << "-" << op.end_time << "]"
                << ", Critical=" << (op.is_critical ? "true" : "false")
                << ", ID=" << id);

        }
        catch (const std::exception& e) {
            if (lineNumber <= 10) {
                ERROR("Error parsing line " << lineNumber << ": " << line);
                ERROR("Exception: " << e.what());
            }
        }
    }

    file.close();

    INFO("=== CSV Reading Summary ===");
    INFO("Total lines processed: " << lineNumber);
    INFO("Valid operations loaded: " << operations.size());

    return operations;
}

// 检查操作重叠的独立函数
void checkOperationsOverlap(const std::vector<MyOp>& operations) {
    OVERLAP_DEBUG("=== Overlap Detection ===");

    // 按机器分组
    std::map<int, std::vector<MyOp>> machineOps;
    for (const auto& op : operations) {
        machineOps[op.machine_id].push_back(op);
    }

    bool hasOverlaps = false;
    for (auto& pair : machineOps) {
        int machineId = pair.first;
        auto& ops = pair.second;

        // 按开始时间排序
        std::sort(ops.begin(), ops.end(), [](const MyOp& a, const MyOp& b) {
            return a.start_time < b.start_time;
            });

        OVERLAP_DEBUG("Machine " << machineId << " (" << ops.size() << " operations):");

#ifdef ENABLE_DATA_ANALYSIS
        for (size_t i = 0; i < ops.size(); ++i) {
            const auto& op = ops[i];
            std::cout << "[OVERLAP]   J" << op.job_id << "-" << op.stage
                << " [" << op.start_time << "-" << op.end_time << "]";

            // 检查与下一个操作的重叠
            if (i + 1 < ops.size()) {
                const auto& nextOp = ops[i + 1];
                if (op.end_time > nextOp.start_time) {
                    std::cout << " ！！  OVERLAP with J" << nextOp.job_id << "-" << nextOp.stage
                        << " [" << nextOp.start_time << "-" << nextOp.end_time << "]"
                        << " (overlap time: " << (op.end_time - nextOp.start_time) << ")";
                    hasOverlaps = true;
                }
            }
            std::cout << std::endl;
        }
#endif
    }

    if (!hasOverlaps) {
        INFO("√ No time overlaps detected on any machine.");
    }
    else {
        WARNING("！！  WARNING: Time overlaps detected! This will cause visual overlapping in the Gantt chart.");
    }
}

// 打印vector中的操作数据
void printOperationsVector(const std::vector<MyOp>& operations) {
    DATA_DEBUG("=== Vector Content Analysis ===");
    DATA_DEBUG("Vector size: " << operations.size());

#ifdef ENABLE_DATA_ANALYSIS
    std::cout << "[DATA] Format: [Index] Job-Stage | Machine | Time | Critical | Other" << std::endl;
    std::cout << "[DATA] ---------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < operations.size(); ++i) {
        const auto& op = operations[i];
        std::cout << "[DATA] [" << std::setw(2) << i << "] "
            << "J" << op.job_id << "-" << op.stage << " | "
            << "M" << op.machine_id << " | "
            << "[" << std::setw(2) << op.start_time << "-" << std::setw(2) << op.end_time << "] | "
            << (op.is_critical ? "CRITICAL" : "normal  ") << " | "
            << op.some_other_field << std::endl;
    }
#endif

    if (operations.empty()) {
        WARNING("Vector is empty");
        return;
    }

    // 分析时间和机器范围
    int minTime = operations[0].start_time;
    int maxTime = operations[0].end_time;
    int minMachine = operations[0].machine_id;
    int maxMachine = operations[0].machine_id;

    for (const auto& op : operations) {
        minTime = std::min(minTime, op.start_time);
        maxTime = std::max(maxTime, op.end_time);
        minMachine = std::min(minMachine, op.machine_id);
        maxMachine = std::max(maxMachine, op.machine_id);
    }

    DATA_DEBUG("=== Data Range Analysis ===");
    DATA_DEBUG("Time range: " << minTime << " - " << maxTime << " (duration: " << (maxTime - minTime) << ")");
    DATA_DEBUG("Machine range: " << minMachine << " - " << maxMachine << " (count: " << (maxMachine - minMachine + 1) << ")");

    // 检查重叠
    checkOperationsOverlap(operations);
}

int main() {
    // 检查CSV文件是否存在
    std::string csvFilename = "result.csv";
    std::ifstream testFile(csvFilename);
    if (!testFile.good()) {
        ERROR("CSV file '" << csvFilename << "' not found!");
        std::cerr << "Please create a CSV file with the following format:" << std::endl;
        std::cerr << "ID,Job,Operation,Machine,StartTime,EndTime,IsCritical" << std::endl;
        std::cerr << "1,0,0,3,299,323,1" << std::endl;
        std::cerr << "2,0,1,7,780,843,0" << std::endl;
        std::cerr << "..." << std::endl;
        return -1;
    }
    testFile.close();

    // 读取CSV数据到vector
    std::vector<MyOp> testOps = loadOperationsFromCSV(csvFilename);

    if (testOps.empty()) {
        ERROR("No valid operations loaded from CSV file!");
        return -1;
    }

    // 打印读取到的数据进行验证
    printOperationsVector(testOps);

    // 创建更大的SFML窗口来容纳大数据集
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Gantt Chart Viewer - Large Dataset");
    window.setFramerateLimit(60);

    // 创建甘特图对象
    GanttChart gantt(window);

    // 设置数据到甘特图
    gantt.setData(testOps);

    INFO("=== Gantt Chart Window Opened ===");
    INFO("Controls:");
    INFO("- ESC: Exit");
    INFO("- Mouse Wheel: Zoom in/out");
    INFO("- Arrow Keys or WASD: Pan view");
    INFO("- SPACE: Reset view");
    INFO("- D: Show debug info again");
    INFO("- R: Reset debug output flag");

    // 主循环
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }

                // 重置视图
                if (event.key.code == sf::Keyboard::Space) {
                    gantt.resetView();
                }

                // 重新显示调试信息
                if (event.key.code == sf::Keyboard::D) {
                    INFO("=== Manual Debug Output Requested ===");
                    printOperationsVector(testOps);
                    gantt.printLoadedData();
                }

                // 重置调试输出标志
                if (event.key.code == sf::Keyboard::R) {
                    gantt.resetDebugFlag();
                    INFO("Debug output flag reset. Next draw will show debug info.");
                }
            }

            // 处理鼠标滚轮缩放
            if (event.type == sf::Event::MouseWheelScrolled) {
                gantt.handleMouseWheel(event.mouseWheelScroll.delta);
            }
        }

        // 处理持续按键（平移）
        gantt.handleKeyboard();

        // 绘制甘特图
        gantt.draw();
    }

    return 0;
}