# GanttChart Library

一个基于 SFML 的高性能甘特图可视化库，专为工厂调度、作业车间调度等场景设计。

## 📋 目录

- [功能特性](#功能特性)
- [系统要求](#系统要求)
- [安装说明](#安装说明)
- [快速开始](#快速开始)
- [API 文档](#api-文档)
- [配置说明](#配置说明)
- [交互控制](#交互控制)
- [数据格式](#数据格式)
- [故障排除](#故障排除)
- [性能优化](#性能优化)
- [贡献指南](#贡献指南)

## 🌟 功能特性

### 核心功能
- **实时更新**：支持动态更新调度数据并立即可视化
- **高性能渲染**：优化的绘制算法，支持大规模数据集
- **交互式视图**：支持缩放、平移、重置等操作
- **多数据源**：支持 CSV 文件、内存数据等多种输入方式
- **配置灵活**：丰富的配置选项，适应不同显示需求

### 显示特性
- **智能布局**：根据机器数量自动调整条形图大小和间距
- **彩色编码**：每个作业使用不同颜色，关键路径高亮显示
- **详细标签**：显示作业ID、阶段、机器、时间等信息
- **重叠检测**：自动检测并警告时间冲突
- **多种视图模式**：支持奢华、舒适、紧凑等不同显示密度

### 调试功能
- **数据验证**：自动检查数据完整性和一致性
- **性能监控**：可选的性能分析和调试输出
- **详细日志**：分级日志系统，便于问题诊断

## 🖥️ 系统要求

### 支持平台
- Windows 10/11
- macOS 10.15+
- Linux (Ubuntu 18.04+, CentOS 7+)

### 依赖库
- **SFML 2.5+**：图形渲染库
- **C++17**：编译器支持
- **CMake 3.16+**：构建系统（可选）

### 硬件要求
- **最低配置**：
  - 内存：4GB RAM
  - 显卡：支持 OpenGL 2.1
  - 处理器：双核 2.0GHz
- **推荐配置**：
  - 内存：8GB+ RAM
  - 显卡：独立显卡或集成显卡
  - 处理器：四核 2.5GHz+

## 🚀 安装说明

### 1. 安装 SFML

#### Windows (Visual Studio)
```bash
# 使用 vcpkg
vcpkg install sfml

# 或下载预编译二进制文件
# https://www.sfml-dev.org/download.php
```

#### macOS (Homebrew)
```bash
brew install sfml
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```

#### Linux (CentOS/RHEL)
```bash
sudo yum install SFML-devel
# 或
sudo dnf install SFML-devel
```

### 2. 编译项目

#### 使用 CMake (推荐)
```bash
mkdir build && cd build
cmake ..
make -j4
```

#### 使用 g++ (Linux/macOS)
```bash
g++ -std=c++17 -O3 -o example \
    main.cpp GanttChart.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system
```

#### 使用 Visual Studio (Windows)
1. 创建新的 C++ 项目
2. 添加 SFML 库路径到项目设置
3. 链接 SFML 库：`sfml-graphics.lib`, `sfml-window.lib`, `sfml-system.lib`
4. 添加源文件并编译

## 📖 快速开始

### 基本用法

```cpp
#include "GanttChart.h"
#include <SFML/Graphics.hpp>

int main() {
    // 1. 创建 SFML 窗口
    sf::RenderWindow window;
    
    // 获取配置的上下文设置
    sf::ContextSettings settings = getWindowContextSettings();
    
    // 创建窗口
    window.create(
        sf::VideoMode(1600, 900),
        "Gantt Chart Demo",
        sf::Style::Default,
        settings
    );
    
    // 应用窗口配置
    configureWindow(window);
    
    // 2. 创建甘特图对象
    GanttChart gantt(window);
    
    // 3. 初始化（只需一次）
    if (!gantt.init()) {
        std::cerr << "初始化失败!" << std::endl;
        return -1;
    }
    
    // 4. 准备调度数据
    std::vector<qm::Operation> operations = {
        {0, 0, 0, 0, 25, true},   // job_id, stage, machine_id, start_time, end_time, is_critical
        {0, 1, 1, 30, 55, false},
        {1, 0, 1, 10, 35, false},
        {1, 1, 2, 40, 65, true}
    };
    
    // 5. 更新显示
    gantt.update(operations);
    
    // 6. 主循环
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            gantt.handleEvent(event);  // 处理交互事件
        }
        
        gantt.handleKeyboard();  // 处理键盘输入
        gantt.draw();           // 绘制甘特图
    }
    
    return 0;
}
```

### 从 CSV 文件加载数据

```cpp
// 方法1：直接加载CSV文件
gantt.loadFromCSV("schedule_data.csv");

// 方法2：使用工具函数
auto operations = loadOperationsFromCSV("schedule_data.csv");
gantt.update(operations);
```

### 调度算法集成示例

```cpp
// 在你的调度算法中
GanttChart gantt(window);
gantt.init();

while (算法运行) {
    // 运行你的调度算法
    auto newSchedule = yourSchedulingAlgorithm();
    
    // 立即更新显示
    gantt.update(newSchedule);
    
    // 处理窗口事件
    sf::Event event;
    while (window.pollEvent(event)) {
        gantt.handleEvent(event);
    }
    
    gantt.handleKeyboard();
    gantt.draw();
}
```

## 📚 API 文档

### 核心接口

#### `bool init(const std::string& fontPath = "")`
初始化甘特图为空状态。
- **参数**：`fontPath` - 字体文件路径（可选）
- **返回值**：成功返回 true
- **说明**：必须在使用其他功能前调用一次

#### `bool update(const std::vector<qm::Operation>& operations, bool immediate_draw = true)`
清空前一个显示，绘制当前数据。
- **参数**：
  - `operations` - 新的操作数据
  - `immediate_draw` - 是否立即绘制
- **返回值**：成功返回 true
- **说明**：这是核心更新接口

#### `void draw()`
绘制甘特图到窗口。
- **说明**：通常在主循环中调用

### 交互接口

#### `void handleEvent(const sf::Event& event)`
处理 SFML 事件（鼠标、键盘等）。

#### `void handleKeyboard()`
处理持续按键输入（方向键平移等）。

#### `void resetView()`
重置视图到默认状态。

### 查询接口

#### `size_t getOperationCount() const`
获取当前操作数量。

#### `bool hasOverlaps() const`
检查是否存在时间重叠。

#### `void getTimeRange(int& minT, int& maxT) const`
获取时间范围。

#### `void getMachineRange(int& minM, int& maxM) const`
获取机器范围。

### 配置接口

#### `void applyLayoutConfig(float leftM, float topM, float bottomM, float rightM)`
应用自定义布局配置。

#### `void applyViewConfig(float zoom, float panSpeed)`
应用自定义视图配置。

## ⚙️ 配置说明

所有配置都在 `GanttConfig.h` 文件中定义，分为以下几个类别：

### 窗口配置 (`WindowConfig`)
```cpp
static const int DEFAULT_WINDOW_WIDTH = 1600;    // 默认窗口宽度
static const int DEFAULT_WINDOW_HEIGHT = 900;    // 默认窗口高度
static const int FRAME_RATE_LIMIT = 60;          // 帧率限制
```

### 布局配置 (`LayoutConfig`)
```cpp
static constexpr float LEFT_MARGIN = 80.0f;      // 左边距
static constexpr float TOP_MARGIN = 40.0f;       // 上边距
static constexpr float DEFAULT_BAR_HEIGHT = 25.0f; // 默认条形高度
```

### 视图配置 (`ViewConfig`)
```cpp
static constexpr float MIN_ZOOM_LEVEL = 0.1f;    // 最小缩放
static constexpr float MAX_ZOOM_LEVEL = 5.0f;    // 最大缩放
static constexpr float PAN_SPEED = 20.0f;        // 平移速度
```

### 调试配置 (`DebugConfig`)
```cpp
static const bool ENABLE_CSV_DEBUG = false;      // CSV调试
static const bool ENABLE_DATA_ANALYSIS = false;  // 数据分析
static const bool ENABLE_RANGE_DEBUG = true;     // 范围调试
```

### 自定义配置
修改 `GanttConfig.h` 文件中的相应值，然后重新编译即可。

## 🎮 交互控制

### 鼠标操作
- **滚轮**：缩放视图
  - 向上滚动：放大
  - 向下滚动：缩小

### 键盘操作
- **方向键 / WASD**：平移视图
  - ↑/W：向上平移
  - ↓/S：向下平移
  - ←/A：向左平移
  - →/D：向右平移
- **空格键**：重置视图到默认状态
- **R 键**：重置调试标志
- **D 键**：手动输出调试信息
- **ESC 键**：退出程序

### 视图控制
- **缩放范围**：0.1x - 5.0x
- **平移**：无限制（可配置边界）
- **重置**：一键恢复默认视图

## 📊 数据格式

### Operation 结构体
```cpp
struct Operation {
    int job_id;       // 作业ID
    int stage;        // 阶段/工序
    int machine_id;   // 机器ID
    int start_time;   // 开始时间
    int end_time;     // 结束时间
    bool is_critical; // 是否为关键路径
};
```

### CSV 文件格式
```csv
ID,Job,Operation,Machine,StartTime,EndTime,IsCritical
1,0,0,3,299,323,1
2,0,1,7,780,843,0
3,0,2,5,843,866,1
4,1,0,2,150,175,0
```

**列说明**：
- `ID`：操作唯一标识符
- `Job`：作业ID
- `Operation`：操作/阶段编号
- `Machine`：机器编号
- `StartTime`：开始时间
- `EndTime`：结束时间
- `IsCritical`：是否关键路径（1=是，0=否）

### 数据验证
库会自动进行以下检查：
- 时间范围有效性（start_time < end_time）
- 机器冲突检测
- 数据完整性验证

## 🔧 故障排除

### 常见编译错误

#### 1. SFML 库未找到
```bash
错误：fatal error: SFML/Graphics.hpp: No such file or directory
```
**解决方案**：
- 确保正确安装 SFML
- 检查头文件和库文件路径
- 验证编译命令中的链接参数

#### 2. 静态成员重定义
```bash
错误："DEFAULT_FONT_PATH": 重定义；多次初始化
```
**解决方案**：
- 检查 `GanttConfig.h` 文件，确保静态成员只有声明
- 静态成员定义应在 `GanttChart.cpp` 中

#### 3. 函数未声明
```bash
错误：找不到标识符 "configureWindow"
```
**解决方案**：
- 确保 `GanttChart.h` 中包含函数声明
- 检查头文件包含顺序

### 运行时问题

#### 1. 窗口创建失败
**症状**：程序启动但窗口不显示
**解决方案**：
- 检查 OpenGL 驱动支持
- 尝试降低抗锯齿设置
- 检查显示器配置

#### 2. 字体加载失败
**症状**：文本不显示或显示异常
**解决方案**：
- 确保字体文件存在
- 检查字体文件路径
- 使用系统默认字体

#### 3. 性能问题
**症状**：帧率低、响应慢
**解决方案**：
- 启用视窗裁剪（默认已启用）
- 减少同时显示的操作数量
- 调整调试输出设置

### 调试技巧

#### 启用调试输出
修改 `GanttConfig.h` 中的调试开关：
```cpp
static const bool ENABLE_DATA_ANALYSIS = true;  // 启用数据分析
static const bool ENABLE_RANGE_DEBUG = true;    // 启用范围调试
```

#### 性能分析
```cpp
static const bool ENABLE_PERFORMANCE_DEBUG = true;
```

#### 数据验证
```cpp
// 验证加载的数据
printOperationsVector(operations);
checkOperationsOverlap(operations);
```

## ⚡ 性能优化

### 大数据集优化
- **视窗裁剪**：只渲染可见区域
- **操作限制**：可选的最大操作数限制
- **内存预分配**：减少动态内存分配

### 配置建议
```cpp
// 高性能配置
static const bool ENABLE_VIEWPORT_CULLING = true;
static const bool ENABLE_TEXT_OPTIMIZATION = true;
static const int MAX_VISIBLE_OPERATIONS = 5000;
```

### 最佳实践
1. **批量更新**：避免频繁调用 `update()`
2. **合理缩放**：避免过度缩放导致性能问题
3. **关闭调试**：发布版本关闭所有调试输出
4. **字体优化**：使用系统字体以提高加载速度

## 🤝 贡献指南

### 开发环境设置
1. Fork 项目仓库
2. 创建功能分支：`git checkout -b feature/your-feature`
3. 遵循代码规范
4. 添加必要的测试
5. 提交 Pull Request

### 代码规范
- 使用 4 个空格缩进
- 类名使用 PascalCase
- 函数名使用 camelCase
- 常量使用 UPPER_SNAKE_CASE
- 添加必要的注释和文档

### 报告问题
请在 GitHub Issues 中报告问题，包含：
- 操作系统和版本
- 编译器和版本
- SFML 版本
- 重现步骤
- 错误日志

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。

<!-- ## 📞 联系方式

- **问题反馈**：[GitHub Issues](https://github.com/yourname/ganttchart/issues)
- **功能建议**：[GitHub Discussions](https://github.com/yourname/ganttchart/discussions)
- **邮件联系**：your.email@example.com -->

## 📝 更新日志

### v1.0.0 (当前版本)
- ✨ 初始版本发布
- ✨ 基础甘特图渲染功能
- ✨ CSV 文件支持
- ✨ 交互式视图控制
- ✨ 丰富的配置选项
- ✨ 调试和性能监控功能

### 计划功能
- 🚀 动画效果支持
- 🚀 图片导出功能
- 🚀 更多数据格式支持
- 🚀 主题系统
- 🚀 插件架构

---

**感谢使用 GanttChart Library！** 🎉