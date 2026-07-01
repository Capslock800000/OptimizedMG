package com.optimizedmg.ui

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material.icons.filled.Calculate
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.optimizedmg.core.PerformanceMonitor
import com.optimizedmg.renderer.OptimizedRenderer
import kotlinx.coroutines.delay

/**
 * 极致优化的主界面，使用高效的Compose组件
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(
    renderer: OptimizedRenderer,
    performanceMonitor: PerformanceMonitor
) {
    var fps by remember { mutableStateOf(0f) }
    var frameTime by remember { mutableStateOf(0f) }
    var frameTimeStdDev by remember { mutableStateOf(0f) }
    
    // 使用LaunchedEffect高效更新性能数据
    LaunchedEffect(Unit) {
        while (true) {
            fps = renderer.getCurrentFPS()
            frameTime = performanceMonitor.getAverageFrameTime()
            frameTimeStdDev = performanceMonitor.getFrameTimeStdDev()
            delay(100) // 100ms更新一次，平衡性能和数据新鲜度
        }
    }
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { 
                    Text(
                        "OptimizedMG", 
                        maxLines = 1,
                        modifier = Modifier.fillMaxWidth()
                    )
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.primaryContainer
                )
            )
        }
    ) { padding ->
        Column(
            modifier = Modifier
                .padding(padding)
                .fillMaxSize()
                .verticalScroll(rememberScrollState()),
            verticalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            // 性能监控卡片 - 使用记忆化减少重组
            PerformanceStatsCard(fps, frameTime, frameTimeStdDev)
            
            // 控制面板
            ControlPanel(renderer)
            
            // 数学性能测试
            MathBenchmarkCard()
        }
    }
}

@Composable
private fun PerformanceStatsCard(
    fps: Float,
    frameTime: Float,
    frameTimeStdDev: Float
) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp),
        colors = CardDefaults.cardColors(
            containerColor = MaterialTheme.colorScheme.surfaceVariant
        )
    ) {
        Column(
            modifier = Modifier.padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            Text(
                text = "性能监控",
                style = MaterialTheme.typography.headlineSmall,
                color = MaterialTheme.colorScheme.onSurfaceVariant
            )
            
            PerformanceMetricRow("FPS", "%.1f".format(fps), fps > 55f)
            PerformanceMetricRow("帧时间", "%.2f ms".format(frameTime), frameTime < 18f)
            PerformanceMetricRow("稳定性", "%.3f ms".format(frameTimeStdDev), frameTimeStdDev < 2f)
        }
    }
}

@Composable
private fun PerformanceMetricRow(
    label: String,
    value: String,
    isGood: Boolean
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(
            text = label,
            style = MaterialTheme.typography.bodyMedium
        )
        Text(
            text = value,
            style = MaterialTheme.typography.bodyLarge,
            color = if (isGood) MaterialTheme.colorScheme.primary 
                   else MaterialTheme.colorScheme.error
        )
    }
}

@Composable
private fun ControlPanel(renderer: OptimizedRenderer) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
    ) {
        Column(
            modifier = Modifier.padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            Text(
                text = "渲染控制",
                style = MaterialTheme.typography.headlineSmall
            )
            
            Button(
                onClick = { /* 启动渲染测试 */ },
                modifier = Modifier.fillMaxWidth(),
                colors = ButtonDefaults.buttonColors()
            ) {
                Icon(Icons.Default.PlayArrow, contentDescription = null)
                Spacer(Modifier.width(8.dp))
                Text("开始性能测试")
            }
            
            OutlinedButton(
                onClick = { /* 重置统计 */ },
                modifier = Modifier.fillMaxWidth()
            ) {
                Icon(Icons.Default.Refresh, contentDescription = null)
                Spacer(Modifier.width(8.dp))
                Text("重置统计")
            }
        }
    }
}

@Composable
private fun MathBenchmarkCard() {
    var benchmarkResult by remember { mutableStateOf("") }
    
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
    ) {
        Column(
            modifier = Modifier.padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            Text(
                text = "数学性能测试",
                style = MaterialTheme.typography.headlineSmall
            )
            
            Button(
                onClick = {
                    benchmarkResult = runMathBenchmark()
                },
                modifier = Modifier.fillMaxWidth()
            ) {
                Icon(Icons.Default.Calculate, contentDescription = null)
                Spacer(Modifier.width(8.dp))
                Text("运行数学基准测试")
            }
            
            if (benchmarkResult.isNotEmpty()) {
                Text(
                    text = benchmarkResult,
                    style = MaterialTheme.typography.bodyMedium,
                    color = MaterialTheme.colorScheme.onSurface
                )
            }
        }
    }
}

private fun runMathBenchmark(): String {
    val iterations = 1_000_000
    val startTime = System.nanoTime()
    
    // 向量运算测试
    var sum = 0f
    for (i in 0 until iterations) {
        val x = i.toFloat()
        sum += fastSqrt(x * x + (i + 1).toFloat() * (i + 1).toFloat())
    }
    
    val endTime = System.nanoTime()
    val duration = (endTime - startTime) / 1_000_000.0 // 毫秒
    
    return "数学基准测试完成:\n" +
           "迭代次数: $iterations\n" +
           "总耗时: %.2f ms\n".format(duration) +
           "平均每次运算: %.4f μs".format(duration * 1000 / iterations)
}

// 快速平方根函数
private fun fastSqrt(x: Float): Float {
    if (x <= 0f) return 0f
    val i = java.lang.Float.floatToIntBits(x)
    val y = java.lang.Float.intBitsToFloat(0x5f3759df - (i shr 1))
    return y * (1.5f - 0.5f * x * y * y)
}