# 调试宏系统使用说明

## 概述

程序已实现了基于预编译器宏的调试系统，您可以通过修改文件顶部的宏定义来控制不同类型的调试输出。

## 调试开关配置

### 在代码顶部找到这个区域：

```cpp
// ========== 调试开关配置 ==========
// 修改这些宏的值来控制调试输出
// 1 = 开启, 0 = 关闭

#if 1
#define ENABLE_CSV_DEBUG        // CSV读取调试信息
#endif

#if 1  
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
```

## 调试类型说明

| 调试类型 | 宏名称 | 默认状态 | 作用 |
|----------|--------|----------|------|
| **CSV调试** | `ENABLE_CSV_DEBUG` | ✅ 开启 | 显示CSV文件读取过程 |
| **数据分析** | `ENABLE_DATA_ANALYSIS` | ✅ 开启 | 显示数据统计和重叠检测 |
| **绘制调试** | `ENABLE_DRAWING_DEBUG` | ❌ 关闭 | 显示每个操作块的绘制坐标 |
| **范围调试** | `ENABLE_RANGE_DEBUG` | ✅ 开启 | 显示时间和机器范围计算 |
| **性能调试** | `ENABLE_PERFORMANCE_DEBUG` | ❌ 关闭 | 显示渲染性能统计 |

## 使用方法

### 1. 开启特定调试

要开启某个调试类型，将对应的 `#if 0` 改为 `#if 1`：

```cpp
// 开启绘制调试
#if 1
#define ENABLE_DRAWING_DEBUG
#endif
```

### 2. 关闭特定调试

要关闭某个调试类型，将对应的 `#if 1` 改为 `#if 0`：

```cpp
// 关闭CSV调试
#if 0
#define ENABLE_CSV_DEBUG
#endif
```

### 3. 完全关闭所有调试

```cpp
#if 0
#define ENABLE_CSV_DEBUG
#endif

#if 0  
#define ENABLE_DATA_ANALYSIS
#endif

#if 0
#define ENABLE_DRAWING_DEBUG
#endif

#if 0
#define ENABLE_RANGE_DEBUG
#endif

#if 0
#define ENABLE_PERFORMANCE_DEBUG
#endif
```

## 调试输出示例

### CSV调试（ENABLE_CSV_DEBUG）
```
[CSV] === CSV Reading Started ===
[CSV] Reading from file: result.csv
[CSV] Header line: ID,Job,Operation,Machine,StartTime,EndTime,IsCritical
[CSV] ✓ Detected your CSV format
[CSV_DETAIL] Line 2: 1,0,0,3,299,323,1
[CSV_DETAIL] Parsed 7 cells: [0]="1" [1]="0" [2]="0" [3]="3" [4]="299" [5]="323" [6]="1"
```

### 数据分析（ENABLE_DATA_ANALYSIS）
```
[DATA] === Vector Content Analysis ===
[DATA] Vector size: 293
[DATA] [0] J0-0 | M3 | [299-323] | CRITICAL | 1
[OVERLAP] === Overlap Detection ===
[OVERLAP] Machine 0 operations:
[OVERLAP]   J0-2 [994-1023]
```

### 绘制调试（ENABLE_DRAWING_DEBUG）
```
[DRAW] === Drawing Operations ===
[DRAW] Zoom: 1, Offset: (0, 0)
[DRAW] J0-0 | M3 | (234.5, 105.0) | size(15.2, 25) | [299-323]
```

### 范围调试（ENABLE_RANGE_DEBUG）
```
[RANGE] Range calculation:
[RANGE] Time: 299 - 2110 (span: 1811)
[RANGE] Machines: 0 - 14 (count: 15)
[RANGE] Chart width: 1520, Time scale: 0.839
```

### 性能调试（ENABLE_PERFORMANCE_DEBUG）
```
[PERF] Visible operations: 156/293
[PERF] Actually drawn: 156
[PERF] Drawing time: 2847 microseconds
```

## 常用配置组合

### 🔍 **开发调试**（找问题时）
```cpp
#if 1
#define ENABLE_CSV_DEBUG
#endif
#if 1
#define ENABLE_DATA_ANALYSIS
#endif
#if 1
#define ENABLE_DRAWING_DEBUG
#endif
#if 1
#define ENABLE_RANGE_DEBUG
#endif
#if 0
#define ENABLE_PERFORMANCE_DEBUG
#endif
```

### 📊 **数据验证**（检查CSV数据时）
```cpp
#if 1
#define ENABLE_CSV_DEBUG
#endif
#if 1
#define ENABLE_DATA_ANALYSIS
#endif
#if 0
#define ENABLE_DRAWING_DEBUG
#endif
#if 0
#define ENABLE_RANGE_DEBUG
#endif
#if 0
#define ENABLE_PERFORMANCE_DEBUG
#endif
```

### 🚀 **发布版本**（无调试信息）
```cpp
#if 0
#define ENABLE_CSV_DEBUG
#endif
#if 0
#define ENABLE_DATA_ANALYSIS
#endif
#if 0
#define ENABLE_DRAWING_DEBUG
#endif
#if 0
#define ENABLE_RANGE_DEBUG
#endif
#if 0
#define ENABLE_PERFORMANCE_DEBUG
#endif
```

### ⚡ **性能分析**（优化性能时）
```cpp
#if 0
#define ENABLE_CSV_DEBUG
#endif
#if 0
#define ENABLE_DATA_ANALYSIS
#endif
#if 0
#define ENABLE_DRAWING_DEBUG
#endif
#if 0
#define ENABLE_RANGE_DEBUG
#endif
#if 1
#define ENABLE_PERFORMANCE_DEBUG
#endif
```

## 优势

✅ **编译时决定**：关闭的调试代码完全不会编译进程序  
✅ **零性能影响**：发布版本中没有调试相关的运行时开销  
✅ **分类控制**：可以只开启需要的调试类型  
✅ **易于管理**：通过修改几个宏就能控制所有调试输出  

## 通用调试宏

这些宏总是可用，不受开关控制：

- `INFO(x)`：一般信息输出
- `WARNING(x)`：警告信息输出  
- `ERROR(x)`：错误信息输出

## 注意事项

1. **重新编译**：修改调试宏后需要重新编译程序
2. **文件大小**：开启所有调试会增加可执行文件大小
3. **输出量**：开启过多调试可能产生大量输出信息
4. **性能影响**：开启调试会轻微影响程序性能

## 使用建议

1. **开发时**：开启 CSV_DEBUG 和 DATA_ANALYSIS
2. **调试绘制问题**：开启 DRAWING_DEBUG 和 RANGE_DEBUG  
3. **性能优化**：只开启 PERFORMANCE_DEBUG
4. **最终发布**：关闭所有调试宏