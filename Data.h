//
// Created by qiming on 25-7-3.
//
#ifndef FJSP_GANTT_DATA_H
#define FJSP_GANTT_DATA_H

#include <vector>

namespace qm {
    struct Operation {
        int job_id;
        int stage;
        int machine_id;
        int start_time;
        int end_time;
        bool is_critical;

        Operation() = default;

        // 简单的模板构造函数，运行时检查
        template<typename T>
        explicit Operation(const T& t)
            : job_id(static_cast<int>(t.job_id)),
            stage(static_cast<int>(t.stage)),
            machine_id(static_cast<int>(t.machine_id)),
            start_time(static_cast<int>(t.start_time)),
            end_time(static_cast<int>(t.end_time)),
            is_critical(static_cast<bool>(t.is_critical)) {
        }
    };

    struct Data {
        std::vector<Operation> data;

        // 简单的模板构造函数
        template<typename Container>
        explicit Data(const Container& c) {
            data.reserve(c.size());
            for (const auto& item : c) {
                data.emplace_back(Operation(item));
            }
        }
    };
}

#endif //FJSP_GANTT_DATA_H