//
// GanttChart ���ò����ļ�
// ���й���������ʾ�����ܡ�������صĲ���
//
#ifndef GANTT_CONFIG_H
#define GANTT_CONFIG_H

#include <string>

namespace GanttConfig {

    // ========== ���ں���ʾ���� ==========
    struct WindowConfig {
        // ���ڲ���
        static const int DEFAULT_WINDOW_WIDTH = 1600;
        static const int DEFAULT_WINDOW_HEIGHT = 900;
        static const int FRAME_RATE_LIMIT = 60;
        static const bool ENABLE_VERTICAL_SYNC = false;
        static const bool ENABLE_ANTIALIASING = false;
        static const int OPENGL_MAJOR_VERSION = 2;
        static const int OPENGL_MINOR_VERSION = 1;

        // ֡����
        static const int TARGET_FRAME_TIME_MS = 16;  // ~60 FPS
        static const int MAX_UPDATE_FREQUENCY_MS = 33; // ������Ƶ�� ~30 FPS
    };

    // ========== ����ͼ�������� ==========
    struct LayoutConfig {
        // �߾�����
        static constexpr float LEFT_MARGIN = 80.0f;
        static constexpr float TOP_MARGIN = 40.0f;
        static constexpr float BOTTOM_MARGIN = 60.0f;
        static constexpr float RIGHT_MARGIN = 30.0f;

        // ����ͼ����
        static constexpr float MIN_BAR_HEIGHT = 12.0f;
        static constexpr float MAX_BAR_HEIGHT = 80.0f;
        static constexpr float MIN_BAR_SPACING = 1.0f;
        static constexpr float MAX_BAR_SPACING = 30.0f;
        static constexpr float DEFAULT_BAR_HEIGHT = 25.0f;
        static constexpr float DEFAULT_BAR_SPACING = 2.0f;

        // ������ֵ�����ڶ�̬������
        static const int LUXURY_MACHINE_COUNT = 3;      // �ݻ���ʾ��ֵ
        static const int COMFORTABLE_MACHINE_COUNT = 6; // ������ʾ��ֵ
        static const int BALANCED_MACHINE_COUNT = 10;   // ƽ����ʾ��ֵ
        static const int COMPACT_MACHINE_COUNT = 15;    // ������ʾ��ֵ

        // ���ֱ���
        static constexpr float LUXURY_BAR_RATIO = 0.7f;
        static constexpr float COMFORTABLE_BAR_RATIO = 0.75f;
        static constexpr float BALANCED_BAR_RATIO = 0.8f;
        static constexpr float COMPACT_BAR_RATIO = 0.85f;
        static constexpr float ULTRA_COMPACT_BAR_RATIO = 0.9f;
    };

    // ========== ���ź�ƽ������ ==========
    struct ViewConfig {
        // ���Ų���
        static constexpr float MIN_ZOOM_LEVEL = 0.1f;
        static constexpr float MAX_ZOOM_LEVEL = 5.0f;
        static constexpr float DEFAULT_ZOOM_LEVEL = 1.0f;
        static constexpr float ZOOM_STEP = 0.1f;

        // ƽ�Ʋ���
        static constexpr float PAN_SPEED = 20.0f;
        static constexpr float ZOOM_ADJUSTED_PAN_FACTOR = 1.0f; // ����ʱƽ���ٶȵ���

        // ��ͼ����
        static const bool ENABLE_VIEW_CLAMPING = true;
        static constexpr float VIEW_CLAMP_MARGIN = 50.0f; // ��ͼ�߽�����
    };

    // ========== ������ı����� ==========
    struct FontConfig {
        // ��������
        static const std::string DEFAULT_FONT_PATH;
        static const std::string FALLBACK_FONT_PATHS[];
        static const int FALLBACK_FONT_COUNT = 4;

        // �����С
        static const int DEFAULT_FONT_SIZE = 11;
        static const int MIN_FONT_SIZE = 8;
        static const int MAX_FONT_SIZE = 16;
        static const int TITLE_FONT_SIZE = 12;
        static const int STATUS_FONT_SIZE = 10;

        // �ı���ʾ��ֵ
        static constexpr float MIN_WIDTH_FOR_TEXT = 30.0f;  // ��ʾ�ı�����С���
        static constexpr float MIN_HEIGHT_FOR_TEXT = 15.0f; // ��ʾ�ı�����С�߶�
        static constexpr float TEXT_PADDING = 4.0f;         // �ı��ڱ߾�

        // �����С����Ӧ
        static constexpr float FONT_SIZE_THRESHOLD_1 = 80.0f;  // �е�������ֵ
        static constexpr float FONT_SIZE_THRESHOLD_2 = 120.0f; // ��������ֵ
    };

    // ========== ��ɫ����ʽ���� ==========
    struct ColorConfig {
        // HSV��ɫ���ɲ���
        static constexpr float JOB_COLOR_SATURATION = 0.7f;
        static constexpr float JOB_COLOR_VALUE = 0.9f;

        // �߿�����
        static const int NORMAL_BORDER_THICKNESS = 1;
        static const int CRITICAL_BORDER_THICKNESS = 2;

        // ����������ɫ��RGBֵ��
        static const int BACKGROUND_R = 255, BACKGROUND_G = 255, BACKGROUND_B = 255; // ��ɫ
        static const int AXIS_R = 0, AXIS_G = 0, AXIS_B = 0;                        // ��ɫ
        static const int TEXT_R = 0, TEXT_G = 0, TEXT_B = 0;                        // ��ɫ
        static const int CRITICAL_BORDER_R = 255, CRITICAL_BORDER_G = 0, CRITICAL_BORDER_B = 0; // ��ɫ
        static const int STATUS_TEXT_R = 0, STATUS_TEXT_G = 0, STATUS_TEXT_B = 255;  // ��ɫ

        // Ĭ����ҵ��ɫ��������ʧ��ʱ��
        static const int DEFAULT_JOB_R = 128, DEFAULT_JOB_G = 128, DEFAULT_JOB_B = 128; // ��ɫ
    };

    // ========== �������� ==========
    struct PerformanceConfig {
        // �Ӵ��ü�
        static const bool ENABLE_VIEWPORT_CULLING = true;
        static constexpr float CULLING_MARGIN = 20.0f; // �ü��߽�����

        // �����Ż�
        static const bool ENABLE_TEXT_OPTIMIZATION = true;  // �ı������Ż�
        static const bool ENABLE_OPERATION_LIMIT = false;   // �Ƿ�������ʾ�Ĳ�������
        static const int MAX_VISIBLE_OPERATIONS = 5000;     // ���ɼ�������

        // �����Ż�
        static const bool ENABLE_INCREMENTAL_UPDATE = false; // �������£�δʵ�֣�
        static const bool ENABLE_DIRTY_CHECKING = false;     // ���飨δʵ�֣�

        // �ڴ����
        static const bool ENABLE_MEMORY_OPTIMIZATION = true;
        static const int RESERVE_OPERATIONS_SIZE = 1000;    // Ԥ�����������
    };

    // ========== ʱ�������� ==========
    struct TimeAxisConfig {
        // ʱ���ǩ
        static const int LABEL_SPACING_PIXELS = 80;  // ʱ���ǩ��ࣨ���أ�
        static const int MIN_TIME_STEP = 1;          // ��Сʱ�䲽��
        static const int TIME_STEP_CANDIDATES[];     // ��ѡʱ�䲽��
        static const int TIME_STEP_CANDIDATE_COUNT = 6;

        // �̶���
        static constexpr float TICK_HEIGHT = 5.0f;
        static const int TICK_THICKNESS = 1;

        // �����ߣ���ѡ��
        static const bool ENABLE_GRID_LINES = false;
        static constexpr float GRID_LINE_ALPHA = 0.3f;
    };

    // ========== �������� ==========
    struct DebugConfig {
        // ���Կ��� - ����ͨ���޸���Щֵ�����Ƶ������
        static const bool ENABLE_CSV_DEBUG = false;
        static const bool ENABLE_DATA_ANALYSIS = false;
        static const bool ENABLE_DRAWING_DEBUG = false;
        static const bool ENABLE_RANGE_DEBUG = true;
        static const bool ENABLE_PERFORMANCE_DEBUG = false;
        static const bool ENABLE_OVERLAP_DEBUG = false;

        // �����������
        static const int MAX_DEBUG_OPERATIONS_SHOW = 10;  // �����ʾ�ĵ��Բ�����
        static const int MAX_DEBUG_LINES_PER_FRAME = 50;  // ÿ֡�������������

        // ���ܼ��
        static const bool ENABLE_FPS_COUNTER = false;
        static const bool ENABLE_FRAME_TIME_LOG = false;
        static const int PERFORMANCE_LOG_INTERVAL_FRAMES = 60; // ÿ60֡���һ��������Ϣ
    };

    // ========== �������� ==========
    struct InteractionConfig {
        // ������
        static constexpr float MOUSE_WHEEL_ZOOM_FACTOR = 0.1f;

        // ������Ӧ
        static const bool ENABLE_KEYBOARD_PAN = true;
        static const bool ENABLE_KEYBOARD_SHORTCUTS = true;

        // ����֧�֣�Ԥ����
        static const bool ENABLE_TOUCH_SUPPORT = false;
        static constexpr float TOUCH_ZOOM_SENSITIVITY = 1.0f;
    };

    // ========== �������ã�Ԥ����չ��==========
    struct AnimationConfig {
        // ��ͼ�л�����
        static const bool ENABLE_SMOOTH_TRANSITIONS = false;
        static const int TRANSITION_DURATION_MS = 300;

        // ���ݸ��¶���
        static const bool ENABLE_UPDATE_ANIMATION = false;
        static const int UPDATE_ANIMATION_DURATION_MS = 200;

        // ��������
        static const bool USE_EASING = false;
    };

    // ========== �������ã�Ԥ����չ��==========
    struct ExportConfig {
        // ͼƬ����
        static const bool ENABLE_PNG_EXPORT = false;
        static const bool ENABLE_SVG_EXPORT = false;

        // Ĭ�ϵ�������
        static const int EXPORT_WIDTH = 1920;
        static const int EXPORT_HEIGHT = 1080;
        static constexpr float EXPORT_SCALE_FACTOR = 2.0f;
    };

} // namespace GanttConfig

// ע�⣺��̬��Ա�Ķ���Ӧ���� .cpp �ļ���

#endif // GANTT_CONFIG_H