#include "GanttChart.h"
#include <iostream>
#include <thread>
#include <chrono>

/**
 * 甘特图库简单使用示例
 * 演示两个核心接口：
 * 1. init() - 初始化空状态接口
 * 2. update() - 清空前一个显示，绘制当前显示的接口
 */

 // 创建一些示例调度数据
std::vector<qm::Operation> createSampleSchedule1() {
    std::vector<qm::Operation> operations;

    // 示例调度方案1：3个作业，简单安排
    qm::Operation op;

    // 作业0
    op.job_id = 0; op.stage = 0; op.machine_id = 0; op.start_time = 0; op.end_time = 25; op.is_critical = true;
    operations.push_back(op);
    op.job_id = 0; op.stage = 1; op.machine_id = 1; op.start_time = 30; op.end_time = 55; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 0; op.stage = 2; op.machine_id = 2; op.start_time = 60; op.end_time = 85; op.is_critical = true;
    operations.push_back(op);

    // 作业1  
    op.job_id = 1; op.stage = 0; op.machine_id = 1; op.start_time = 10; op.end_time = 35; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 1; op.stage = 1; op.machine_id = 2; op.start_time = 40; op.end_time = 65; op.is_critical = true;
    operations.push_back(op);
    op.job_id = 1; op.stage = 2; op.machine_id = 0; op.start_time = 90; op.end_time = 115; op.is_critical = false;
    operations.push_back(op);

    // 作业2
    op.job_id = 2; op.stage = 0; op.machine_id = 2; op.start_time = 20; op.end_time = 45; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 2; op.stage = 1; op.machine_id = 0; op.start_time = 120; op.end_time = 145; op.is_critical = true;
    operations.push_back(op);
    op.job_id = 2; op.stage = 2; op.machine_id = 1; op.start_time = 150; op.end_time = 175; op.is_critical = false;
    operations.push_back(op);

    return operations;
}

std::vector<qm::Operation> createSampleSchedule2() {
    std::vector<qm::Operation> operations;

    // 示例调度方案2：4个作业，更复杂的安排
    qm::Operation op;

    // 作业0
    op.job_id = 0; op.stage = 0; op.machine_id = 2; op.start_time = 0; op.end_time = 20; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 0; op.stage = 1; op.machine_id = 0; op.start_time = 25; op.end_time = 50; op.is_critical = true;
    operations.push_back(op);

    // 作业1
    op.job_id = 1; op.stage = 0; op.machine_id = 1; op.start_time = 5; op.end_time = 30; op.is_critical = true;
    operations.push_back(op);
    op.job_id = 1; op.stage = 1; op.machine_id = 3; op.start_time = 35; op.end_time = 60; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 1; op.stage = 2; op.machine_id = 2; op.start_time = 65; op.end_time = 90; op.is_critical = true;
    operations.push_back(op);

    // 作业2
    op.job_id = 2; op.stage = 0; op.machine_id = 0; op.start_time = 55; op.end_time = 80; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 2; op.stage = 1; op.machine_id = 1; op.start_time = 95; op.end_time = 120; op.is_critical = true;
    operations.push_back(op);

    // 作业3
    op.job_id = 3; op.stage = 0; op.machine_id = 3; op.start_time = 10; op.end_time = 35; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 3; op.stage = 1; op.machine_id = 2; op.start_time = 95; op.end_time = 120; op.is_critical = false;
    operations.push_back(op);
    op.job_id = 3; op.stage = 2; op.machine_id = 1; op.start_time = 125; op.end_time = 150; op.is_critical = true;
    operations.push_back(op);

    return operations;
}

std::vector<qm::Operation> createSampleSchedule3() {
    std::vector<qm::Operation> operations;

    // 示例调度方案3：更多机器，密集调度
    qm::Operation op;

    for (int job = 0; job < 5; ++job) {
        for (int stage = 0; stage < 3; ++stage) {
            op.job_id = job;
            op.stage = stage;
            op.machine_id = (job + stage) % 6;  // 使用6台机器
            op.start_time = job * 30 + stage * 20;
            op.end_time = op.start_time + 15 + (job * 5);
            op.is_critical = (job == 2 && stage == 1) || (job == 4 && stage == 0);
            operations.push_back(op);
        }
    }

    return operations;
}

// 创建示例CSV文件
void createSampleCSV() {
    std::ofstream file("output.csv");
    if (file.is_open()) {
        file << "ID,Job,Operation,Machine,StartTime,EndTime,IsCritical\n";
        file << "1,0,0,3,299,323,1\n";
        file << "2,0,1,7,780,843,0\n";
        file << "3,0,2,5,843,866,1\n";
        file << "4,1,0,2,150,175,0\n";
        file << "5,1,1,4,400,425,1\n";
        file << "6,1,2,6,600,625,0\n";
        file << "7,2,0,1,50,75,0\n";
        file << "8,2,1,3,323,348,1\n";
        file << "9,2,2,8,700,725,0\n";
        file << "10,3,0,0,0,25,1\n";
        file << "11,3,1,5,866,891,0\n";
        file << "12,3,2,9,950,975,1\n";
        file.close();
        std::cout << "[INFO] Sample CSV file 'output.csv' created" << std::endl;
    }
}

int main() {
    std::cout << "=== 甘特图库简单使用示例 ===" << std::endl;
    std::cout << "演示两个核心接口的使用方法" << std::endl;

    // 创建示例CSV文件（如果不存在）
    std::ifstream csvCheck("output.csv");
    if (!csvCheck.good()) {
        createSampleCSV();
    }
    csvCheck.close();

    // 设置SFML的错误处理
    sf::err().rdbuf(nullptr); // 禁用SFML错误输出

    // 使用配置文件创建SFML窗口
    sf::RenderWindow window;
    try {
        // 获取配置的上下文设置
        sf::ContextSettings settings = getWindowContextSettings();

        // 创建窗口
        window.create(
            sf::VideoMode(GanttConfig::WindowConfig::DEFAULT_WINDOW_WIDTH,
                GanttConfig::WindowConfig::DEFAULT_WINDOW_HEIGHT),
            "Gantt Chart - Configured Display",
            sf::Style::Default,
            settings
        );

        if (!window.isOpen()) {
            std::cerr << "Failed to create SFML window!" << std::endl;
            return -1;
        }

        // 应用配置设置
        configureWindow(window);

        std::cout << "✓ Window created with configuration: "
            << window.getSize().x << "x" << window.getSize().y
            << " @" << GanttConfig::WindowConfig::FRAME_RATE_LIMIT << "fps" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "SFML window creation failed: " << e.what() << std::endl;
        return -1;
    }

    // 创建甘特图对象
    GanttChart gantt(window);

    // === 接口1：初始化空状态 ===
    std::cout << "\n步骤1：调用 init() 初始化空状态" << std::endl;
    if (!gantt.init()) {
        std::cerr << "初始化失败!" << std::endl;
        return -1;
    }
    std::cout << "✓ 甘特图初始化成功" << std::endl;

    // 准备示例数据
    std::vector<std::vector<qm::Operation>> schedules = {
        createSampleSchedule1(),
        createSampleSchedule2(),
        createSampleSchedule3()
    };

    // 如果有CSV文件，也加入到示例中
    auto csvOps = loadOperationsFromCSV("output.csv");
    if (!csvOps.empty()) {
        schedules.push_back(csvOps);
    }

    std::cout << "\n步骤2：使用 update() 接口展示不同的调度方案" << std::endl;
    std::cout << "准备了 " << schedules.size() << " 个示例调度方案" << std::endl;
    std::cout << "\n控制说明：" << std::endl;
    std::cout << "  鼠标滚轮 - 缩放" << std::endl;
    std::cout << "  方向键/WASD - 平移" << std::endl;
    std::cout << "  空格键 - 重置视图" << std::endl;
    std::cout << "  ESC - 退出" << std::endl;
    std::cout << "\n程序将自动切换显示不同的调度方案..." << std::endl;

    // 主循环 - 演示update接口的使用
    sf::Clock frameClock;
    sf::Clock switchClock;
    int currentSchedule = 0;
    bool hasUpdated = false;

    while (window.isOpen()) {
        // 控制帧率（使用配置参数）
        if (frameClock.getElapsedTime().asMilliseconds() < GanttConfig::WindowConfig::TARGET_FRAME_TIME_MS) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        frameClock.restart();

        // 每3秒切换一次调度方案，演示update接口
        if (switchClock.getElapsedTime().asSeconds() >= 3.0f) {
            if (!schedules.empty()) {
                // === 接口2：清空前一个显示，绘制当前显示 ===
                std::cout << "\n调用 update() 接口，显示调度方案 " << (currentSchedule + 1)
                    << " (共" << schedules[currentSchedule].size() << "个操作)" << std::endl;

                bool success = gantt.update(schedules[currentSchedule]);
                if (success) {
                    std::cout << "✓ 更新成功，甘特图已刷新" << std::endl;

                    // 显示统计信息
                    std::cout << "  操作数量: " << gantt.getOperationCount() << std::endl;
                    std::cout << "  是否有重叠: " << (gantt.hasOverlaps() ? "是" : "否") << std::endl;

                    int minTime, maxTime, minMachine, maxMachine;
                    gantt.getTimeRange(minTime, maxTime);
                    gantt.getMachineRange(minMachine, maxMachine);
                    std::cout << "  时间范围: " << minTime << " - " << maxTime << std::endl;
                    std::cout << "  机器范围: " << minMachine << " - " << maxMachine << std::endl;
                }
                else {
                    std::cout << "✗ 更新失败" << std::endl;
                }

                currentSchedule = (currentSchedule + 1) % schedules.size();
                hasUpdated = true;
            }

            switchClock.restart();
        }

        // 处理事件
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                    break;
                }
            }

            // 将事件传递给甘特图处理
            try {
                gantt.handleEvent(event);
            }
            catch (const std::exception& e) {
                std::cerr << "Error handling event: " << e.what() << std::endl;
            }
        }

        if (!window.isOpen()) break;

        // 处理持续按键
        try {
            gantt.handleKeyboard();
        }
        catch (const std::exception& e) {
            std::cerr << "Error handling keyboard: " << e.what() << std::endl;
        }

        // 绘制当前状态
        try {
            gantt.draw();
        }
        catch (const std::exception& e) {
            std::cerr << "Error during drawing: " << e.what() << std::endl;
        }
    }

    std::cout << "\n=== 示例程序结束 ===" << std::endl;
    std::cout << "实际使用时，你只需要：" << std::endl;
    std::cout << "1. 调用 gantt.init() 一次进行初始化" << std::endl;
    std::cout << "2. 每当有新的调度结果时，调用 gantt.update(newData) 更新显示" << std::endl;
    std::cout << "3. 在主循环中调用 gantt.draw() 进行绘制" << std::endl;
    std::cout << "4. 可以修改 GanttConfig.h 文件来调整显示参数" << std::endl;

    return 0;
}

/**
 * 实际使用示例：
 *
 * // 在你的调度程序中
 * GanttChart gantt(window);
 * gantt.init();  // 初始化一次
 *
 * while (调度算法运行) {
 *     auto newSchedule = 你的调度算法();
 *     gantt.update(newSchedule);  // 清空旧的，显示新的
 *
 *     // 主循环中
 *     gantt.handleEvent(event);
 *     gantt.handleKeyboard();
 *     gantt.draw();  // 绘制当前状态
 * }
 */