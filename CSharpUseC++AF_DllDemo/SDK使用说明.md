# ATF 主动对焦 SDK 使用说明（客户公开 API）

> 对应 `AF.dll`（x64）+ 客户公开头 `AF_Main.h`。本说明只覆盖**客户可使用**的接口；管理员/标定高级参数（MaxFocusStep/线性区/捕获区/稳定帧数/分段模式/跟踪上限/标定R²）不在客户头内，如需请联系管理员。

## 1. 通用约定
- **架构**：C# 与 `AF.dll` 必须同为 **x64**。
- **调用约定**：`__cdecl`（C# 用 `CallingConvention.Cdecl`）。
- **字符串**：统一 **UTF-8**。C# 建议用 `[MarshalAs(UnmanagedType.LPUTF8Str)]`；返回的 `char*`/`char**` 由 DLL 管理，**只读不释放**。
- **返回值**：多数函数返回 `Result { int code; char* message; }`，`code==0` 为成功，`message` 为 UTF-8 说明。
- **内存**：`Get_Objective_List` 返回的 `char**` **必须**用 `Free_Objective_List` 归还，否则内存泄漏。

## 2. 数据结构
```c
struct Result          { int code; char* message; };
struct Result_Array    { int code; char** strings; int count; };

// 物镜基础配置（含 μm 换算所需的两个步进参数）
struct ObjectiveSimpleConfig {
    int    Step;             // 步长(count)
    int    LightIntensity;   // 激光强度 0~1000（自动模式下限200）
    int    ExposureTime;     // 曝光
    int    FocusDoneRange;   // 对焦完成区间(count)
    double PValue;           // 对焦修正比例
    double FullStepsPerMm;   // 每毫米整步数，0=未配置（Move_Z_Um 需要）
    int    Microstepping;    // 每整步细分数（Move_Z_Um 需要）
};

struct AFRuntimeStatus { int State,Connected,CameraReady,LaserEnabled,InFocus,InRange,MotionBusy,SignalValid,SegmentMode,LastErrorCount; };

// 回调
typedef void (*ValueCallback)(int value);        // Z 轴移动值 / 传感器值（-512~+511）
```

## 3. 状态码（Get_AF_State / Set_AF_State_Ptr 推送）
| 值 | 含义 |
|---|---|
| 0 | 对焦成功 |
| 1 | 对焦中 |
| 2 | 标定中 |
| 3 | 标定完成 |
| -1 | 对焦失败 |
| -2 | 标定失败 |
| -3 | Z 轴移动超时 |

## 4. 接口清单（按职责）

### 物镜管理（增删改查）
| 函数 | 说明 |
|---|---|
| `Add_Objective(name, step, light, exposure, focusRange)` | 新增物镜 |
| `Delete_Objective(name)` | 删除物镜 |
| `Rename_Objective(old, new)` | 重命名 |
| `Update_Objective_Config(name, step, light, exposure, focusRange, pValue)` | 改基础参数 |
| `Get_Objective_Config(name, out ObjectiveSimpleConfig*)` | 查询基础配置（含 FullStepsPerMm/Microstepping）|
| `Set_Objective_StepParam(name, fullStepsPerMm, microstepping)` | **设置每毫米整步数与细分数**（Move_Z_Um 微米换算所需）|
| `Get_Working_Objective()` → `const char*` | 当前对焦物镜名 |
| `Get_Objective_List()` → `Result_Array` | 物镜列表（用后须 `Free_Objective_List`）|
| `Free_Objective_List(Result_Array)` | 释放列表内存 |
| `Set_Objective(name)` | 设置当前对焦物镜 |

### 连接 / 对焦
| 函数 | 说明 |
|---|---|
| `Connect_AF(port)` | 连接对焦模块（port=串口名，如 "COM3"；115200/8/N/1）|
| `DisConnect_AF()` | 断开 |
| `Auto_Focus()` | 执行主动对焦（0完成 -1超范围 -2取参失败）|
| `AF_Stop()` | 停止对焦/标定 |
| `Set_AF_Tracking_Mode(mode)` | 0=单次(合焦即停) 1=连续 |

### 状态读取
| 函数 | 说明 |
|---|---|
| `Get_AF_State()` → int | 当前状态（见第3节）|
| `Get_AF_Runtime_Status(out AFRuntimeStatus*)` | 读取综合运行状态 |
| `Set_AF_State_Ptr(int*)` | 注册状态指针（状态变化时由 DLL 写入，码同 Get_AF_State）|

### 标定 / 初始化（客户可独立完成全流程）
| 函数 | 说明 |
|---|---|
| `InIt(step)` | 物镜初始化（内部委托 Make0_Auto，step≥5）|
| `Make0()` | 保存当前重心为零点重心（Origin_Centroid_X）|
| `Set_Focus_Reference()` | 设置焦点参考位置（只存 Centroid_X，不动零点）|
| `Reset_Focus_Reference()` | 重置焦点参考位置（Centroid_X ← Origin_Centroid_X）|
| `Make0_Auto(step)` | **完整自动标定**（Z轴扫描+线性拟合，step≥5；须 R²≥0.98）|
| `Auto_Signal_Tune()` | 仅自动信号调整（遍历激光功率，不扫Z）|

### 移动（Z 轴）
| 函数 | 说明 |
|---|---|
| `Move_Z(value)` | 按脉冲值(count)移动 |
| `Move_Z_Um(micrometers)` | 按微米(μm)移动——需先用 `Set_Objective_StepParam` 配置 `FullStepsPerMm`+`Microstepping` |
| `Set_Z_Output_Mode(mode)` | Z 输出模式：**0=正常(脉冲SET_PUL,物理移动)**，**1=回调(经 MoveZValueCallback 输出Z，不移动)**，**2=模拟量(SET_AI,物理移动)** |

### 激光
| 函数 | 说明 |
|---|---|
| `Set_Lim(isReverse)` | 设置限位方向 |
| `Set_Laser_Power(power)` | 手动设置激光功率(0=关，0~1000)|
| `Set_Auto_Laser_Enable(enable)` | 开关自动激光亮度闭环 |
| `Get_Auto_Laser_Power()` → int | 当前激光功率 |

### 图像 / 回调
| 函数 | 说明 |
|---|---|
| `GetImageData(out data, out w, out h, out step)` | 获取一帧单通道灰度图（指针指向 DLL 内部内存，**必须立即复制**，不可释放/长期保存）|
| `MoveZValueCallback(cb)` | 注册 Z 轴移动值回调 |
| `RegisterSensorValueCallback(cb)` | 注册传感器值回调（对焦中逐帧推送，-512~+511）|

## 5. 典型使用流程（C# 示意）
```csharp
// 1) 连接
AFNative.Connect_AF("COM3");

// 2) 选物镜（须先由管理员/Add_Objective 配置好）
AFNative.Set_Objective("10X");

// 3) 配置 μm 换算（Move_Z_Um 需要）——按硬件实测填
AFNative.Set_Objective_StepParam("10X", fullStepsPerMm: 200.0, microstepping: 8);

// 4) 标定（首次或换物镜后；step≥5）
AFNative.Make0_Auto(20);   // 完整标定：Z扫描+拟合，成功后状态=3

// 5) 主动对焦
int state = AFNative.Get_AF_State();   // 轮询：0=成功
AFNative.Auto_Focus();

// 6) 微米级移动
AFNative.Move_Z_Um(5.0);   // +5μm（按 FullStepsPerMm×Microstepping 换算成脉冲）
```

## 6. μm 换算说明（Move_Z_Um）
`Move_Z_Um(μm)` 内部按**当前物镜**的 `FullStepsPerMm` 与 `Microstepping` 换算成脉冲：
```
count = μm / 1000 × FullStepsPerMm × Microstepping
```
- 1 个 count(脉冲) = `1000 / (FullStepsPerMm × Microstepping)` 微米。
- 这两个值由 `Set_Objective_StepParam` 设置、`Get_Objective_Config` 读取；未配置（≤0）时 `Move_Z_Um` 返回失败。
- `Auto_Focus`/标定使用 count 逻辑，不经过 μm 换算。

## 7. Z 输出模式（脉冲/模拟量/回调）
正常模式(`Set_Z_Output_Mode(0)`)：`Move_Z` 物理移动 Z 轴，下发脉冲指令 `SET_PUL`。
模拟量模式(`Set_Z_Output_Mode(2)`)：`Move_Z` 物理移动 Z 轴，下发模拟量指令 `SET_AI`（与正常模式的区别仅在下发指令不同）。
回调模式(`Set_Z_Output_Mode(1)`)：`Move_Z` 不驱动硬件，而是经 `MoveZValueCallback` 注册的回调把 Z 值送出（用于外部控制/仿真）。**注意**：回调模式是全局开关，会影响所有 `Move_Z` 调用（含 Auto_Focus/标定内部调用）——该模式下对焦/标定不会实际移动 Z 轴。

## 8. 注意事项
- **x64**：C# 与 AF.dll 必须同为 x64，否则 `BadImageFormatException`。
- **运行时依赖**：目标机需安装 **MVS 相机 SDK**（提供 `MVCameraControl.dll`）与 **VC++ 2015-2022 x64 运行时**；`opencv_world450.dll`、运动控制 DLL 等已随包提供。
- **物镜参数**：基础参数（步长/激光/曝光/完成区间/P值/每毫米整步数/细分）可由客户设置；高级对焦参数（MaxFocusStep/线性区/捕获区/稳定帧数/分段模式/跟踪上限/标定R²）由管理员配置。
