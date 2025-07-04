#include "GanttChart.h"

// ========== 配置文件静态成员定义 ==========
namespace GanttConfig {
    // 字体路径定义
    const std::string FontConfig::DEFAULT_FONT_PATH = "arial.ttf";
    const std::string FontConfig::FALLBACK_FONT_PATHS[] = {
        "C:\\Windows\\Fonts\\arial.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "/usr/share/fonts/truetype/arial.ttf",
        "/usr/share/fonts/TTF/arial.ttf"
    };

    // 时间步长候选
    const int TimeAxisConfig::TIME_STEP_CANDIDATES[] = {
        50, 100, 200, 500, 1000, 2000
    };
}

// === 构造函数 ===
GanttChart::GanttChart(sf::RenderWindow& win) : window(win) {
    // 加载默认配置
    loadDefaultConfig();
}

// === 主要接口实现 ===

bool GanttChart::init(const std::string& fontPath) {
    std::lock_guard<std::mutex> lock(dataMutex);

    // 尝试加载字体
    std::string actualFontPath = fontPath.empty() ?
        GanttConfig::FontConfig::DEFAULT_FONT_PATH : fontPath;

    if (!tryLoadFont(actualFontPath)) {
        std::cout << "[WARNING] Could not load any font file. Using default font." << std::endl;
    }

    // 清空所有数据
    operations.clear();
    jobColors.clear();

    // 重置视图参数到配置默认值
    zoomLevel = GanttConfig::ViewConfig::DEFAULT_ZOOM_LEVEL;
    offsetX = 0.0f;
    offsetY = 0.0f;

    // 重置范围
    minTime = maxTime = 0;
    minMachine = maxMachine = 0;
    timeScale = 1.0f;

    debugOutputShown = false;
    isInitialized = true;

    INFO("Gantt chart initialized successfully");
    if (GanttConfig::DebugConfig::ENABLE_RANGE_DEBUG) {
        printCurrentConfig();
    }

    return true;
}

bool GanttChart::update(const qm::Data& newData, bool immediate_draw) {
    return update(newData.data, immediate_draw);
}

bool GanttChart::update(const std::vector<qm::Operation>& newOperations, bool immediate_draw) {
    if (!isInitialized) {
        ERROR("GanttChart not initialized. Call init() first.");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(dataMutex);

        // 清空旧数据
        operations.clear();
        jobColors.clear();
        debugOutputShown = false;

        // 设置新数据
        operations = newOperations;

        if (!operations.empty()) {
            calculateRanges();
            generateJobColors();

            DATA_DEBUG("=== Data Updated ===");
            printLoadedData();
        }
    }

    // 立即绘制（如果请求的话）
    if (immediate_draw) {
        draw();
    }

    INFO("Updated gantt chart with " << newOperations.size() << " operations");
    return true;
}

void GanttChart::loadDefaultConfig() {
    // 从配置文件加载默认参数
    leftMargin = GanttConfig::LayoutConfig::LEFT_MARGIN;
    topMargin = GanttConfig::LayoutConfig::TOP_MARGIN;
    bottomMargin = GanttConfig::LayoutConfig::BOTTOM_MARGIN;
    rightMargin = GanttConfig::LayoutConfig::RIGHT_MARGIN;
    barHeight = GanttConfig::LayoutConfig::DEFAULT_BAR_HEIGHT;
    barSpacing = GanttConfig::LayoutConfig::DEFAULT_BAR_SPACING;

    zoomLevel = GanttConfig::ViewConfig::DEFAULT_ZOOM_LEVEL;
    offsetX = 0.0f;
    offsetY = 0.0f;

    INFO("Default configuration loaded from GanttConfig");
}

bool GanttChart::tryLoadFont(const std::string& fontPath) {
    // 首先尝试加载指定的字体
    if (font.loadFromFile(fontPath)) {
        INFO("Font loaded successfully: " << fontPath);
        return true;
    }

    // 如果失败，尝试备用字体路径
    for (int i = 0; i < GanttConfig::FontConfig::FALLBACK_FONT_COUNT; ++i) {
        if (font.loadFromFile(GanttConfig::FontConfig::FALLBACK_FONT_PATHS[i])) {
            INFO("Fallback font loaded: " << GanttConfig::FontConfig::FALLBACK_FONT_PATHS[i]);
            return true;
        }
    }

    WARNING("Could not load any font file, using default system font");
    return false;
}

// === 原有功能保持兼容 ===

void GanttChart::setData(const std::vector<qm::Operation>& ops) {
    std::lock_guard<std::mutex> lock(dataMutex);

    operations = ops;
    calculateRanges();
    generateJobColors();
    DATA_DEBUG("=== Test Data Loaded ===");
    printLoadedData();
}

bool GanttChart::loadFromCSV(const std::string& filename) {
    auto loadedOps = loadOperationsFromCSV(filename);
    if (loadedOps.empty()) {
        ERROR("Failed to load operations from CSV file: " << filename);
        return false;
    }

    return update(loadedOps);
}

// 打印已加载的数据进行验证
void GanttChart::printLoadedData() {
#if ENABLE_DATA_ANALYSIS
    std::cout << "[DATA] === Loaded Operations Data ===" << std::endl;
    std::cout << "[DATA] Format: [Index] Job-Stage | Machine | Time | Critical" << std::endl;
    std::cout << "[DATA] ---------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < operations.size(); ++i) {
        const auto& op = operations[i];
        std::cout << "[DATA] [" << std::setw(2) << i << "] "
            << "J" << op.job_id << "-" << op.stage << " | "
            << "M" << op.machine_id << " | "
            << "[" << std::setw(2) << op.start_time << "-" << std::setw(2) << op.end_time << "] | "
            << (op.is_critical ? "CRITICAL" : "normal  ") << std::endl;
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
void GanttChart::checkOverlaps() {
    OVERLAP_DEBUG("=== Overlap Analysis ===");

    // 按机器分组检查重叠
    std::map<int, std::vector<qm::Operation>> machineOps;
    for (const auto& op : operations) {
        machineOps[op.machine_id].push_back(op);
    }

    bool hasOverlaps = false;

#if ENABLE_DATA_ANALYSIS
    for (auto& pair : machineOps) {
        int machineId = pair.first;
        auto& ops = pair.second;

        // 按开始时间排序
        std::sort(ops.begin(), ops.end(), [](const qm::Operation& a, const qm::Operation& b) {
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

// 计算时间和机器的范围，并动态调整布局
void GanttChart::calculateRanges() {
    if (operations.empty()) return;

    minTime = maxTime = operations[0].start_time;
    minMachine = maxMachine = operations[0].machine_id;

    for (const auto& op : operations) {
        minTime = std::min(minTime, op.start_time);
        maxTime = std::max(maxTime, op.end_time);
        minMachine = std::min(minMachine, op.machine_id);
        maxMachine = std::max(maxMachine, op.machine_id);
    }

    // 根据机器数量动态调整布局参数（使用配置文件阈值）
    int machineCount = maxMachine - minMachine + 1;
    float availableHeight = window.getSize().y - topMargin - bottomMargin;

    RANGE_DEBUG("Machine count: " << machineCount << ", Available height: " << availableHeight);

    if (machineCount <= GanttConfig::LayoutConfig::LUXURY_MACHINE_COUNT) {
        // 很少机器时：大条形，宽间距
        barHeight = std::min(GanttConfig::LayoutConfig::MAX_BAR_HEIGHT,
            availableHeight / machineCount * GanttConfig::LayoutConfig::LUXURY_BAR_RATIO);
        barSpacing = std::min(GanttConfig::LayoutConfig::MAX_BAR_SPACING,
            availableHeight / machineCount * (1.0f - GanttConfig::LayoutConfig::LUXURY_BAR_RATIO));
        RANGE_DEBUG("Layout: Very few machines - luxury display");
    }
    else if (machineCount <= GanttConfig::LayoutConfig::COMFORTABLE_MACHINE_COUNT) {
        // 少量机器时：中等条形，适中间距
        barHeight = std::min(60.0f, availableHeight / machineCount * GanttConfig::LayoutConfig::COMFORTABLE_BAR_RATIO);
        barSpacing = std::min(15.0f, availableHeight / machineCount * (1.0f - GanttConfig::LayoutConfig::COMFORTABLE_BAR_RATIO));
        RANGE_DEBUG("Layout: Few machines - comfortable display");
    }
    else if (machineCount <= GanttConfig::LayoutConfig::BALANCED_MACHINE_COUNT) {
        // 中等数量机器：平衡显示
        barHeight = std::min(45.0f, availableHeight / machineCount * GanttConfig::LayoutConfig::BALANCED_BAR_RATIO);
        barSpacing = std::min(8.0f, availableHeight / machineCount * (1.0f - GanttConfig::LayoutConfig::BALANCED_BAR_RATIO));
        RANGE_DEBUG("Layout: Medium machines - balanced display");
    }
    else if (machineCount <= GanttConfig::LayoutConfig::COMPACT_MACHINE_COUNT) {
        // 较多机器：紧凑显示
        barHeight = std::min(30.0f, availableHeight / machineCount * GanttConfig::LayoutConfig::COMPACT_BAR_RATIO);
        barSpacing = std::min(5.0f, availableHeight / machineCount * (1.0f - GanttConfig::LayoutConfig::COMPACT_BAR_RATIO));
        RANGE_DEBUG("Layout: Many machines - compact display");
    }
    else {
        // 大量机器：超紧凑显示，按可用空间平均分配
        float totalSpace = availableHeight / machineCount;
        barHeight = std::max(GanttConfig::LayoutConfig::MIN_BAR_HEIGHT,
            totalSpace * GanttConfig::LayoutConfig::ULTRA_COMPACT_BAR_RATIO);
        barSpacing = std::max(GanttConfig::LayoutConfig::MIN_BAR_SPACING,
            totalSpace * (1.0f - GanttConfig::LayoutConfig::ULTRA_COMPACT_BAR_RATIO));
        RANGE_DEBUG("Layout: Very many machines - ultra-compact display");
    }

    // 确保布局参数在配置范围内
    barHeight = std::max(GanttConfig::LayoutConfig::MIN_BAR_HEIGHT,
        std::min(GanttConfig::LayoutConfig::MAX_BAR_HEIGHT, barHeight));
    barSpacing = std::max(GanttConfig::LayoutConfig::MIN_BAR_SPACING,
        std::min(GanttConfig::LayoutConfig::MAX_BAR_SPACING, barSpacing));

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
void GanttChart::generateJobColors() {
    jobColors.clear();
    std::vector<int> uniqueJobs;

    for (const auto& op : operations) {
        if (std::find(uniqueJobs.begin(), uniqueJobs.end(), op.job_id) == uniqueJobs.end()) {
            uniqueJobs.push_back(op.job_id);
        }
    }

    // 生成不同的颜色（使用配置文件参数）
    for (size_t i = 0; i < uniqueJobs.size(); ++i) {
        float hue = (360.0f * i) / uniqueJobs.size();
        sf::Color color = hsvToRgb(hue,
            GanttConfig::ColorConfig::JOB_COLOR_SATURATION,
            GanttConfig::ColorConfig::JOB_COLOR_VALUE);
        jobColors[uniqueJobs[i]] = color;
    }
}

// HSV转RGB（使用配置的默认颜色）
sf::Color GanttChart::hsvToRgb(float h, float s, float v) {
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
float GanttChart::getMachineY(int machineId) {
    int machineIndex = machineId - minMachine;
    return topMargin + offsetY + machineIndex * (barHeight + barSpacing);
}

// 获取时间在屏幕上的X坐标
float GanttChart::getTimeX(int time) {
    return leftMargin + offsetX + (time - minTime) * timeScale;
}

// 添加缩放和平移控制（使用配置参数）
void GanttChart::handleZoom(float delta) {
    float oldZoom = zoomLevel;
    zoomLevel = std::max(GanttConfig::ViewConfig::MIN_ZOOM_LEVEL,
        std::min(GanttConfig::ViewConfig::MAX_ZOOM_LEVEL,
            zoomLevel + delta * GanttConfig::ViewConfig::ZOOM_STEP));

    if (zoomLevel != oldZoom) {
        std::lock_guard<std::mutex> lock(dataMutex);
        calculateRanges();
        std::cout << "Zoom level: " << zoomLevel << std::endl;
    }
}

void GanttChart::handlePan(float deltaX, float deltaY) {
    std::lock_guard<std::mutex> lock(dataMutex);

    // 根据配置调整平移速度
    float adjustedDeltaX = deltaX * GanttConfig::ViewConfig::ZOOM_ADJUSTED_PAN_FACTOR / zoomLevel;
    float adjustedDeltaY = deltaY * GanttConfig::ViewConfig::ZOOM_ADJUSTED_PAN_FACTOR / zoomLevel;

    offsetX += adjustedDeltaX;
    offsetY += adjustedDeltaY;

    // 限制平移范围（如果启用）
    if (GanttConfig::ViewConfig::ENABLE_VIEW_CLAMPING && !operations.empty()) {
        float maxOffsetX = std::max(0.0f, (maxTime - minTime) * timeScale - (window.getSize().x - leftMargin - rightMargin));
        float maxOffsetY = std::max(0.0f, (maxMachine - minMachine + 1) * (barHeight + barSpacing) - (window.getSize().y - topMargin - bottomMargin));

        offsetX = std::max(-maxOffsetX - GanttConfig::ViewConfig::VIEW_CLAMP_MARGIN,
            std::min(GanttConfig::ViewConfig::VIEW_CLAMP_MARGIN, offsetX));
        offsetY = std::max(-maxOffsetY - GanttConfig::ViewConfig::VIEW_CLAMP_MARGIN,
            std::min(GanttConfig::ViewConfig::VIEW_CLAMP_MARGIN, offsetY));
    }
}

// 绘制甘特图
void GanttChart::draw() {
    std::lock_guard<std::mutex> lock(dataMutex);

    // 添加错误处理
    try {
        window.clear(sf::Color::White);

        if (operations.empty()) {
            // 绘制空状态提示
            sf::Text emptyText("No Data - Load operations to display gantt chart", font, 16);
            emptyText.setFillColor(sf::Color::Black);
            emptyText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2);
            window.draw(emptyText);

            // 绘制基本坐标轴
            drawAxes();
        }
        else {
            drawAxes();
            drawOperations();
            drawLabels();
        }

        window.display();
    }
    catch (const std::exception& e) {
        ERROR("Error during drawing: " << e.what());
        // 至少尝试清除窗口
        try {
            window.clear(sf::Color::White);
            window.display();
        }
        catch (...) {
            // 忽略二次错误
        }
    }
}

// 重置调试标志
void GanttChart::resetDebugFlag() {
    debugOutputShown = false;
}

// 处理鼠标滚轮缩放（使用配置参数）
void GanttChart::handleMouseWheel(float delta) {
    handleZoom(delta * GanttConfig::InteractionConfig::MOUSE_WHEEL_ZOOM_FACTOR);
}

// 处理键盘平移（使用配置参数）
void GanttChart::handleKeyboard() {
    if (!GanttConfig::InteractionConfig::ENABLE_KEYBOARD_PAN) return;

    float panSpeed = GanttConfig::ViewConfig::PAN_SPEED;

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

// 重置视图（使用配置默认值）
void GanttChart::resetView() {
    std::lock_guard<std::mutex> lock(dataMutex);
    zoomLevel = GanttConfig::ViewConfig::DEFAULT_ZOOM_LEVEL;
    offsetX = 0.0f;
    offsetY = 0.0f;
    calculateRanges();
    std::cout << "View reset to default" << std::endl;
}

// 处理SFML事件
void GanttChart::handleEvent(const sf::Event& event) {
    switch (event.type) {
    case sf::Event::MouseWheelScrolled:
        handleMouseWheel(event.mouseWheelScroll.delta);
        break;

    case sf::Event::KeyPressed:
        switch (event.key.code) {
        case sf::Keyboard::Space:
            resetView();
            break;
        case sf::Keyboard::R:
            resetDebugFlag();
            INFO("Debug output flag reset. Next draw will show debug info.");
            break;
        case sf::Keyboard::D:
            INFO("=== Manual Debug Output Requested ===");
            printLoadedData();
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

// === 查询接口 ===

size_t GanttChart::getOperationCount() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return operations.size();
}

bool GanttChart::hasOverlaps() const {
    std::lock_guard<std::mutex> lock(dataMutex);

    if (operations.empty()) return false;

    // 按机器分组检查重叠
    std::map<int, std::vector<qm::Operation>> machineOps;
    for (const auto& op : operations) {
        machineOps[op.machine_id].push_back(op);
    }

    for (auto& pair : machineOps) {
        auto& ops = pair.second;
        std::sort(ops.begin(), ops.end(), [](const qm::Operation& a, const qm::Operation& b) {
            return a.start_time < b.start_time;
            });

        for (size_t i = 0; i + 1 < ops.size(); ++i) {
            if (ops[i].end_time > ops[i + 1].start_time) {
                return true;
            }
        }
    }

    return false;
}

void GanttChart::getTimeRange(int& minT, int& maxT) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    minT = minTime;
    maxT = maxTime;
}

void GanttChart::getMachineRange(int& minM, int& maxM) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    minM = minMachine;
    maxM = maxMachine;
}

// === 配置相关接口实现 ===

void GanttChart::applyLayoutConfig(float leftM, float topM, float bottomM, float rightM) {
    std::lock_guard<std::mutex> lock(dataMutex);
    leftMargin = leftM;
    topMargin = topM;
    bottomMargin = bottomM;
    rightMargin = rightM;

    if (!operations.empty()) {
        calculateRanges();
    }

    INFO("Custom layout configuration applied");
}

void GanttChart::applyViewConfig(float zoom, float panSpeed) {
    std::lock_guard<std::mutex> lock(dataMutex);
    zoomLevel = std::max(GanttConfig::ViewConfig::MIN_ZOOM_LEVEL,
        std::min(GanttConfig::ViewConfig::MAX_ZOOM_LEVEL, zoom));

    if (!operations.empty()) {
        calculateRanges();
    }

    INFO("Custom view configuration applied");
}

void GanttChart::printCurrentConfig() const {
    std::cout << "[CONFIG] === Current Configuration ===" << std::endl;
    std::cout << "[CONFIG] Layout: margins(" << leftMargin << "," << topMargin
        << "," << bottomMargin << "," << rightMargin << ")" << std::endl;
    std::cout << "[CONFIG] Bars: height=" << barHeight << ", spacing=" << barSpacing << std::endl;
    std::cout << "[CONFIG] View: zoom=" << zoomLevel << ", offset(" << offsetX << "," << offsetY << ")" << std::endl;
    std::cout << "[CONFIG] Window: " << window.getSize().x << "x" << window.getSize().y << std::endl;
}

// === 绘制方法 ===

// 绘制坐标轴
void GanttChart::drawAxes() {
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
void GanttChart::drawOperations() {
    bool shouldShowDebug = !debugOutputShown;

    if (shouldShowDebug) {
        DRAW_DEBUG("=== Drawing Operations ===");
        DRAW_DEBUG("Zoom: " << zoomLevel << ", Offset: (" << offsetX << ", " << offsetY << ")");
        DRAW_DEBUG("Visible operations (showing first 5):");
    }

    int drawnCount = 0;
    int visibleCount = 0;

#if ENABLE_PERFORMANCE_DEBUG
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

#if ENABLE_DRAWING_DEBUG
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

#if ENABLE_PERFORMANCE_DEBUG
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        std::cout << "[PERF] Drawing time: " << duration.count() << " microseconds" << std::endl;
#endif
    }
}

// 绘制标签
void GanttChart::drawLabels() {
    if (operations.empty()) return;

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

    // 绘制操作数量信息
    std::string opInfo = "Operations: " + std::to_string(operations.size());
    sf::Text opText(opInfo, font, 10);
    opText.setFillColor(sf::Color::Blue);
    opText.setPosition(window.getSize().x - 100, 20);
    window.draw(opText);
}

// === 工具函数实现 ===

// 配置SFML窗口设置（修改为配置函数而不是创建函数）
void configureWindow(sf::RenderWindow& window) {
    window.setFramerateLimit(GanttConfig::WindowConfig::FRAME_RATE_LIMIT);
    window.setVerticalSyncEnabled(GanttConfig::WindowConfig::ENABLE_VERTICAL_SYNC);
}

// 获取窗口创建设置
sf::ContextSettings getWindowContextSettings() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = GanttConfig::WindowConfig::ENABLE_ANTIALIASING ? 4 : 0;
    settings.majorVersion = GanttConfig::WindowConfig::OPENGL_MAJOR_VERSION;
    settings.minorVersion = GanttConfig::WindowConfig::OPENGL_MINOR_VERSION;
    return settings;
}

// CSV读取函数
std::vector<qm::Operation> loadOperationsFromCSV(const std::string& filename) {
    std::vector<qm::Operation> operations;
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

#if ENABLE_CSV_DEBUG
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
        qm::Operation op;
        try {
            // CSV格式：ID,Job,Operation,Machine,StartTime,EndTime,IsCritical
            int id = std::stoi(row[0]);           // ID (用作索引)
            op.job_id = std::stoi(row[1]);        // Job -> job_id
            op.stage = std::stoi(row[2]);         // Operation -> stage  
            op.machine_id = std::stoi(row[3]);    // Machine -> machine_id
            op.start_time = std::stoi(row[4]);    // StartTime -> start_time
            op.end_time = std::stoi(row[5]);      // EndTime -> end_time
            op.is_critical = (std::stoi(row[6]) == 1);  // IsCritical -> is_critical (1表示true)

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
void checkOperationsOverlap(const std::vector<qm::Operation>& operations) {
    OVERLAP_DEBUG("=== Overlap Detection ===");

    // 按机器分组
    std::map<int, std::vector<qm::Operation>> machineOps;
    for (const auto& op : operations) {
        machineOps[op.machine_id].push_back(op);
    }

    bool hasOverlaps = false;
    for (auto& pair : machineOps) {
        int machineId = pair.first;
        auto& ops = pair.second;

        // 按开始时间排序
        std::sort(ops.begin(), ops.end(), [](const qm::Operation& a, const qm::Operation& b) {
            return a.start_time < b.start_time;
            });

        OVERLAP_DEBUG("Machine " << machineId << " (" << ops.size() << " operations):");

#if ENABLE_DATA_ANALYSIS
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
void printOperationsVector(const std::vector<qm::Operation>& operations) {
    DATA_DEBUG("=== Vector Content Analysis ===");
    DATA_DEBUG("Vector size: " << operations.size());

#if ENABLE_DATA_ANALYSIS
    std::cout << "[DATA] Format: [Index] Job-Stage | Machine | Time | Critical" << std::endl;
    std::cout << "[DATA] ---------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < operations.size(); ++i) {
        const auto& op = operations[i];
        std::cout << "[DATA] [" << std::setw(2) << i << "] "
            << "J" << op.job_id << "-" << op.stage << " | "
            << "M" << op.machine_id << " | "
            << "[" << std::setw(2) << op.start_time << "-" << std::setw(2) << op.end_time << "] | "
            << (op.is_critical ? "CRITICAL" : "normal  ") << std::endl;
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

// 创建测试数据
std::vector<qm::Operation> createTestData(int jobCount, int stagesPerJob, int machineCount) {
    std::vector<qm::Operation> operations;

    // 每台机器的当前时间
    std::vector<int> machineCurrentTime(machineCount, 0);

    int currentTime = 0;
    for (int job = 0; job < jobCount; ++job) {
        for (int stage = 0; stage < stagesPerJob; ++stage) {
            qm::Operation op;
            op.job_id = job;
            op.stage = stage;
            op.machine_id = (job * stagesPerJob + stage) % machineCount;

            // 确保不重叠
            op.start_time = std::max(currentTime, machineCurrentTime[op.machine_id]);
            op.end_time = op.start_time + 20 + (rand() % 30); // 20-50时间单位
            op.is_critical = (rand() % 4 == 0); // 25%概率为关键路径

            // 更新机器时间
            machineCurrentTime[op.machine_id] = op.end_time + 2; // 添加间隔

            operations.push_back(op);
            currentTime = op.end_time + (rand() % 5); // 0-4时间单位间隔
        }
    }

    return operations;
}