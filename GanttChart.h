#ifndef GANTT_CHART_H
#define GANTT_CHART_H

#include "data.h"
#include "GanttConfig.h" // ʹ�������ļ�
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

// ========== ���Ժ궨�壨ʹ�������ļ����ƣ�==========
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

// ͨ�õ��Ժ�
#define INFO(x) do { std::cout << "[INFO] " << x << std::endl; } while(0)
#define ERROR(x) do { std::cerr << "[ERROR] " << x << std::endl; } while(0)
#define WARNING(x) do { std::cout << "[WARNING] " << x << std::endl; } while(0)

// ����ͼ������
class GanttChart {
private:
    sf::RenderWindow& window;
    sf::Font font;
    std::vector<qm::Operation> operations;
    std::map<int, sf::Color> jobColors;
    bool debugOutputShown = false;  // ��ӱ�־�������ظ����������Ϣ

    // ͼ����� - �������ļ���ȡĬ��ֵ
    float leftMargin;
    float topMargin;
    float bottomMargin;
    float rightMargin;
    float barHeight;
    float barSpacing;

    // ʱ��ͻ�����Χ
    int minTime, maxTime;
    int minMachine, maxMachine;
    float timeScale;

    // ���ź�ƽ�Ʋ��� - �������ļ���ȡĬ��ֵ
    float zoomLevel;
    float offsetX;
    float offsetY;

    // �̰߳�ȫ
    mutable std::mutex dataMutex;
    bool isInitialized = false;

public:
    explicit GanttChart(sf::RenderWindow& win);

    // === ��Ҫ�ӿ� ===

    /**
     * @brief ��ʼ����״̬�ĸ���ͼ
     * @param fontPath �����ļ�·������ѡ��ʹ�������ļ�Ĭ��ֵ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    bool init(const std::string& fontPath = "");

    /**
     * @brief ���ǰһ����ʾ�����Ƶ�ǰ����ʾ��update���ܣ�
     * @param newData �µĲ�������
     * @param immediate_draw �Ƿ��������Ƶ����ڣ�Ĭ��true��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    bool update(const qm::Data& newData, bool immediate_draw = true);

    /**
     * @brief ���ǰһ����ʾ�����Ƶ�ǰ����ʾ��update���ܣ�- ���ذ汾
     * @param newOperations �µĲ�����������
     * @param immediate_draw �Ƿ��������Ƶ����ڣ�Ĭ��true��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    bool update(const std::vector<qm::Operation>& newOperations, bool immediate_draw = true);

    // === ԭ�й��ܽӿڣ����ּ����ԣ�===

    /**
     * @brief ֱ����������
     */
    void setData(const std::vector<qm::Operation>& ops);

    /**
     * @brief ��CSV�ļ��������ݲ�������ʾ
     * @param filename CSV�ļ�·��
     * @return �ɹ�����true
     */
    bool loadFromCSV(const std::string& filename);

    /**
     * @brief ���Ƹ���ͼ
     */
    void draw();

    /**
     * @brief ��ӡ�Ѽ��ص����ݽ�����֤
     */
    void printLoadedData();

    /**
     * @brief ���õ��Ա�־
     */
    void resetDebugFlag();

    /**
     * @brief ��������������
     */
    void handleMouseWheel(float delta);

    /**
     * @brief �������ƽ��
     */
    void handleKeyboard();

    /**
     * @brief ������ͼ
     */
    void resetView();

    /**
     * @brief ����SFML�¼�
     */
    void handleEvent(const sf::Event& event);

    // === ��ѯ�ӿ� ===

    /**
     * @brief ��ȡ��ǰ��������
     */
    size_t getOperationCount() const;

    /**
     * @brief ����Ƿ����ص�����
     */
    bool hasOverlaps() const;

    /**
     * @brief ��ȡʱ�䷶Χ
     */
    void getTimeRange(int& minT, int& maxT) const;

    /**
     * @brief ��ȡ������Χ
     */
    void getMachineRange(int& minM, int& maxM) const;

    // === ������ؽӿ� ===

    /**
     * @brief Ӧ���Զ��岼������
     */
    void applyLayoutConfig(float leftM, float topM, float bottomM, float rightM);

    /**
     * @brief Ӧ���Զ�����ͼ����
     */
    void applyViewConfig(float zoom, float panSpeed);

    /**
     * @brief ��ȡ��ǰ������Ϣ
     */
    void printCurrentConfig() const;

private:
    // === ˽�з��� ===

    // �������ļ�����Ĭ�ϲ���
    void loadDefaultConfig();

    // ���Լ��������ļ�
    bool tryLoadFont(const std::string& fontPath);

    // ����ʱ��ͻ����ķ�Χ������̬��������
    void calculateRanges();

    // Ϊÿ��job_id������ɫ
    void generateJobColors();

    // HSVתRGB
    sf::Color hsvToRgb(float h, float s, float v);

    // ��ȡ��������Ļ�ϵ�Y����
    float getMachineY(int machineId);

    // ��ȡʱ������Ļ�ϵ�X����
    float getTimeX(int time);

    // ������ź�ƽ�ƿ���
    void handleZoom(float delta);
    void handlePan(float deltaX, float deltaY);

    // ������ط���
    void drawAxes();
    void drawOperations();
    void drawLabels();

    // ��������е��ص�����
    void checkOverlaps();
};

// === ���ߺ��� ===

/**
 * @brief ����SFML��������
 * @param window Ҫ���õĴ�������
 */
void configureWindow(sf::RenderWindow& window);

/**
 * @brief ��ȡ���ڴ�������
 * @return ���úõ�����������
 */
sf::ContextSettings getWindowContextSettings();

/**
 * @brief ��CSV�ļ����ز�������
 * @param filename CSV�ļ�·��
 * @return ������������
 */
std::vector<qm::Operation> loadOperationsFromCSV(const std::string& filename);

/**
 * @brief �������ص��Ķ�������
 * @param operations ��������
 */
void checkOperationsOverlap(const std::vector<qm::Operation>& operations);

/**
 * @brief ��ӡvector�еĲ�������
 * @param operations ��������
 */
void printOperationsVector(const std::vector<qm::Operation>& operations);

/**
 * @brief ������������
 * @param jobCount ��ҵ����
 * @param stagesPerJob ÿ����ҵ�Ľ׶���
 * @param machineCount ��������
 * @return ���Բ�������
 */
std::vector<qm::Operation> createTestData(int jobCount, int stagesPerJob, int machineCount);

#endif // GANTT_CHART_H