//
// GanttChart 配置参数文件
// 集中管理所有显示、性能、调试相关的参数
//
#ifndef GANTT_CONFIG_H
#define GANTT_CONFIG_H

#include <string>

namespace GanttConfig {

    // ========== 窗口和显示配置 ==========
    struct WindowConfig {
        // 窗口参数
        static const int DEFAULT_WINDOW_WIDTH = 1600;
        static const int DEFAULT_WINDOW_HEIGHT = 900;
        static const int FRAME_RATE_LIMIT = 60;
        static const bool ENABLE_VERTICAL_SYNC = false;
        static const bool ENABLE_ANTIALIASING = false;
        static const int OPENGL_MAJOR_VERSION = 2;
        static const int OPENGL_MINOR_VERSION = 1;

        // 帧控制
        static const int TARGET_FRAME_TIME_MS = 16;  // ~60 FPS
        static const int MAX_UPDATE_FREQUENCY_MS = 33; // 最大更新频率 ~30 FPS
    };

    // ========== 甘特图布局配置 ==========
    struct LayoutConfig {
        // 边距设置
        static constexpr float LEFT_MARGIN = 80.0f;
        static constexpr float TOP_MARGIN = 40.0f;
        static constexpr float BOTTOM_MARGIN = 60.0f;
        static constexpr float RIGHT_MARGIN = 30.0f;

        // 条形图参数
        static constexpr float MIN_BAR_HEIGHT = 12.0f;
        static constexpr float MAX_BAR_HEIGHT = 80.0f;
        static constexpr float MIN_BAR_SPACING = 1.0f;
        static constexpr float MAX_BAR_SPACING = 30.0f;
        static constexpr float DEFAULT_BAR_HEIGHT = 25.0f;
        static constexpr float DEFAULT_BAR_SPACING = 2.0f;

        // 布局阈值（用于动态调整）
        static const int LUXURY_MACHINE_COUNT = 3;      // 奢华显示阈值
        static const int COMFORTABLE_MACHINE_COUNT = 6; // 舒适显示阈值
        static const int BALANCED_MACHINE_COUNT = 10;   // 平衡显示阈值
        static const int COMPACT_MACHINE_COUNT = 15;    // 紧凑显示阈值

        // 布局比例
        static constexpr float LUXURY_BAR_RATIO = 0.7f;
        static constexpr float COMFORTABLE_BAR_RATIO = 0.75f;
        static constexpr float BALANCED_BAR_RATIO = 0.8f;
        static constexpr float COMPACT_BAR_RATIO = 0.85f;
        static constexpr float ULTRA_COMPACT_BAR_RATIO = 0.9f;
    };

    // ========== 缩放和平移配置 ==========
    struct ViewConfig {
        // 缩放参数
        static constexpr float MIN_ZOOM_LEVEL = 0.1f;
        static constexpr float MAX_ZOOM_LEVEL = 5.0f;
        static constexpr float DEFAULT_ZOOM_LEVEL = 1.0f;
        static constexpr float ZOOM_STEP = 0.1f;

        // 平移参数
        static constexpr float PAN_SPEED = 20.0f;
        static constexpr float ZOOM_ADJUSTED_PAN_FACTOR = 1.0f; // 缩放时平移速度调整

        // 视图限制
        static const bool ENABLE_VIEW_CLAMPING = true;
        static constexpr float VIEW_CLAMP_MARGIN = 50.0f; // 视图边界余量
    };

    // ========== 字体和文本配置 ==========
    struct FontConfig {
        // 字体设置
        static const std::string DEFAULT_FONT_PATH;
        static const std::string FALLBACK_FONT_PATHS[];
        static const int FALLBACK_FONT_COUNT = 4;

        // 字体大小
        static const int DEFAULT_FONT_SIZE = 11;
        static const int MIN_FONT_SIZE = 8;
        static const int MAX_FONT_SIZE = 16;
        static const int TITLE_FONT_SIZE = 12;
        static const int STATUS_FONT_SIZE = 10;

        // 文本显示阈值
        static constexpr float MIN_WIDTH_FOR_TEXT = 30.0f;  // 显示文本的最小宽度
        static constexpr float MIN_HEIGHT_FOR_TEXT = 15.0f; // 显示文本的最小高度
        static constexpr float TEXT_PADDING = 4.0f;         // 文本内边距

        // 字体大小自适应
        static constexpr float FONT_SIZE_THRESHOLD_1 = 80.0f;  // 中等字体阈值
        static constexpr float FONT_SIZE_THRESHOLD_2 = 120.0f; // 大字体阈值
    };

    // ========== 颜色和样式配置 ==========
    struct ColorConfig {
        // HSV颜色生成参数
        static constexpr float JOB_COLOR_SATURATION = 0.7f;
        static constexpr float JOB_COLOR_VALUE = 0.9f;

        // 边框设置
        static const int NORMAL_BORDER_THICKNESS = 1;
        static const int CRITICAL_BORDER_THICKNESS = 2;

        // 背景和轴颜色（RGB值）
        static const int BACKGROUND_R = 255, BACKGROUND_G = 255, BACKGROUND_B = 255; // 白色
        static const int AXIS_R = 0, AXIS_G = 0, AXIS_B = 0;                        // 黑色
        static const int TEXT_R = 0, TEXT_G = 0, TEXT_B = 0;                        // 黑色
        static const int CRITICAL_BORDER_R = 255, CRITICAL_BORDER_G = 0, CRITICAL_BORDER_B = 0; // 红色
        static const int STATUS_TEXT_R = 0, STATUS_TEXT_G = 0, STATUS_TEXT_B = 255;  // 蓝色

        // 默认作业颜色（当生成失败时）
        static const int DEFAULT_JOB_R = 128, DEFAULT_JOB_G = 128, DEFAULT_JOB_B = 128; // 灰色
    };

    // ========== 性能配置 ==========
    struct PerformanceConfig {
        // 视窗裁剪
        static const bool ENABLE_VIEWPORT_CULLING = true;
        static constexpr float CULLING_MARGIN = 20.0f; // 裁剪边界余量

        // 绘制优化
        static const bool ENABLE_TEXT_OPTIMIZATION = true;  // 文本绘制优化
        static const bool ENABLE_OPERATION_LIMIT = false;   // 是否限制显示的操作数量
        static const int MAX_VISIBLE_OPERATIONS = 5000;     // 最大可见操作数

        // 更新优化
        static const bool ENABLE_INCREMENTAL_UPDATE = false; // 增量更新（未实现）
        static const bool ENABLE_DIRTY_CHECKING = false;     // 脏检查（未实现）

        // 内存管理
        static const bool ENABLE_MEMORY_OPTIMIZATION = true;
        static const int RESERVE_OPERATIONS_SIZE = 1000;    // 预分配操作数量
    };

    // ========== 时间轴配置 ==========
    struct TimeAxisConfig {
        // 时间标签
        static const int LABEL_SPACING_PIXELS = 80;  // 时间标签间距（像素）
        static const int MIN_TIME_STEP = 1;          // 最小时间步长
        static const int TIME_STEP_CANDIDATES[];     // 候选时间步长
        static const int TIME_STEP_CANDIDATE_COUNT = 6;

        // 刻度线
        static constexpr float TICK_HEIGHT = 5.0f;
        static const int TICK_THICKNESS = 1;

        // 网格线（可选）
        static const bool ENABLE_GRID_LINES = false;
        static constexpr float GRID_LINE_ALPHA = 0.3f;
    };

    // ========== 调试配置 ==========
    struct DebugConfig {
        // 调试开关 - 可以通过修改这些值来控制调试输出
        static const bool ENABLE_CSV_DEBUG = false;
        static const bool ENABLE_DATA_ANALYSIS = false;
        static const bool ENABLE_DRAWING_DEBUG = false;
        static const bool ENABLE_RANGE_DEBUG = true;
        static const bool ENABLE_PERFORMANCE_DEBUG = false;
        static const bool ENABLE_OVERLAP_DEBUG = false;

        // 调试输出限制
        static const int MAX_DEBUG_OPERATIONS_SHOW = 10;  // 最多显示的调试操作数
        static const int MAX_DEBUG_LINES_PER_FRAME = 50;  // 每帧最大调试输出行数

        // 性能监控
        static const bool ENABLE_FPS_COUNTER = false;
        static const bool ENABLE_FRAME_TIME_LOG = false;
        static const int PERFORMANCE_LOG_INTERVAL_FRAMES = 60; // 每60帧输出一次性能信息
    };

    // ========== 交互配置 ==========
    struct InteractionConfig {
        // 鼠标滚轮
        static constexpr float MOUSE_WHEEL_ZOOM_FACTOR = 0.1f;

        // 键盘响应
        static const bool ENABLE_KEYBOARD_PAN = true;
        static const bool ENABLE_KEYBOARD_SHORTCUTS = true;

        // 触摸支持（预留）
        static const bool ENABLE_TOUCH_SUPPORT = false;
        static constexpr float TOUCH_ZOOM_SENSITIVITY = 1.0f;
    };

    // ========== 动画配置（预留扩展）==========
    struct AnimationConfig {
        // 视图切换动画
        static const bool ENABLE_SMOOTH_TRANSITIONS = false;
        static const int TRANSITION_DURATION_MS = 300;

        // 数据更新动画
        static const bool ENABLE_UPDATE_ANIMATION = false;
        static const int UPDATE_ANIMATION_DURATION_MS = 200;

        // 缓动函数
        static const bool USE_EASING = false;
    };

    // ========== 导出配置（预留扩展）==========
    struct ExportConfig {
        // 图片导出
        static const bool ENABLE_PNG_EXPORT = false;
        static const bool ENABLE_SVG_EXPORT = false;

        // 默认导出设置
        static const int EXPORT_WIDTH = 1920;
        static const int EXPORT_HEIGHT = 1080;
        static constexpr float EXPORT_SCALE_FACTOR = 2.0f;
    };

} // namespace GanttConfig

// 注意：静态成员的定义应该在 .cpp 文件中

#endif // GANTT_CONFIG_H