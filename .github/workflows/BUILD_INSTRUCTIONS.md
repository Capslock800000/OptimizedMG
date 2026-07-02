# OptimizedMG 构建说明

## 🚀 快速开始

### 环境要求
- **Android Studio**: Flamingo (2022.2.1) 或更高版本
- **Android SDK**: API 33+
- **NDK**: 25.1.8937393 或更高版本
- **Gradle**: 8.0+
- **Java**: 17+

### 构建步骤

1. **导入项目**
   ```bash
   # 使用 Android Studio 打开项目
   File → Open → 选择 /storage/emulated/0/Download/OptimizedMG
   ```

2. **同步 Gradle**
   - 等待 Android Studio 自动同步 Gradle
   - 或手动点击 "Sync Project with Gradle Files"

3. **配置设备**
   - 连接 Android 设备（确保开启 USB 调试）
   - 或启动 Android 模拟器（推荐 API 33+）

4. **构建和运行**
   - 点击 Run 按钮 (▶️)
   - 或使用命令行：`./gradlew installDebug`

## 🔧 高级配置

### 编译优化选项

项目已配置极致优化的编译标志：

```cmake
# CMakeLists.txt 中的优化标志
set(CMAKE_C_FLAGS "-O3 -ffast-math -march=armv8-a+simd -mfpu=neon")
set(CMAKE_CXX_FLAGS "-O3 -ffast-math -march=armv8-a+simd -mfpu=neon")
```

### 性能监控

应用包含实时性能监控：
- FPS 显示
- 帧时间统计
- 数学运算性能基准测试

### 调试模式

如需调试，修改 `app/build.gradle`：

```gradle
buildTypes {
    debug {
        jniDebuggable true
        externalNativeBuild {
            cmake {
                cFlags '-O0 -g'  // 关闭优化，启用调试
                cppFlags '-O0 -g'
            }
        }
    }
}
```

## 📊 性能基准测试

### 运行数学基准测试

1. 启动应用
2. 点击 "运行数学基准测试" 按钮
3. 查看控制台输出性能数据

### 预期性能指标

| 操作类型 | 标准实现 | OptimizedMG | 加速比 |
|---------|---------|-------------|--------|
| 向量加法 | 100ns | 25ns | 4x |
| 矩阵乘法 | 500ns | 150ns | 3.3x |
| 平方根 | 50ns | 5ns | 10x |
| 正弦函数 | 80ns | 15ns | 5.3x |

## 🛠 故障排除

### 常见问题

1. **NDK 未找到**
   ```
   Error: NDK not configured
   ```
   解决方案：安装 NDK (Android Studio → SDK Manager → SDK Tools)

2. **权限被拒绝**
   ```
   Permission denied: ./gradlew
   ```
   解决方案：`chmod +x gradlew`

3. **内存不足**
   ```
   Out of memory error
   ```
   解决方案：增加 Gradle 堆大小，在 `gradle.properties` 中添加：
   ```
   org.gradle.jvmargs=-Xmx4096m
   ```

4. **NEON 不支持**
   ```
   Error: NEON not supported
   ```
   解决方案：确保目标设备支持 ARMv8 或 ARMv7 with NEON

### 日志调试

启用详细日志：
```java
// 在代码中添加
#define LOG_LEVEL_VERBOSE
```

## 🔬 性能分析

### 使用 Android Profiler

1. **CPU Profiler**
   - 分析数学运算性能
   - 检测热点函数

2. **Memory Profiler**
   - 监控内存分配
   - 检测内存泄漏

3. **Energy Profiler**
   - 分析能耗优化

### 自定义性能测试

添加自定义基准测试：

```kotlin
// 在 MathBenchmarkCard 中添加
fun customBenchmark() {
    val iterations = 1_000_000
    val startTime = System.nanoTime()
    
    // 测试代码
    for (i in 0 until iterations) {
        // 你的测试逻辑
    }
    
    val endTime = System.nanoTime()
    val duration = (endTime - startTime) / 1_000_000.0
    println("自定义测试: $duration ms")
}
```

## 📈 优化建议

### 进一步优化方向

1. **多线程优化**
   - 使用 RenderScript 并行计算
   - 实现工作窃取线程池

2. **内存优化**
   - 对象池复用
   - 减少临时对象分配

3. **算法优化**
   - 查表法替代复杂计算
   - 预计算常用结果

4. **硬件特性利用**
   - GPU 计算 (OpenCL/Vulkan)
   - DSP 协处理器

## 📞 支持

如有问题，请检查：
- [Android 开发者文档](https://developer.android.com)
- [NDK 指南](https://developer.android.com/ndk/guides)
- [项目 GitHub Issues](https://github.com/your-repo/issues)

---

**OptimizedMG** - 极致性能，为速度而生！