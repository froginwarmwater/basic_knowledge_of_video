# 从音频数据中读取音频数据
注意: ac 参数在ffmpeg 新版本中已经被淘汰了， 该用下面这个-ch_layout [mono \ stereo]

ffplay -ar 44100 -ac 2 -f f32le audio.pcm

ffplay -ar 44100 -f f32le audio.pcm


-ch_layout stereo

# 作业
描述：
Windows、Linux、Mac从设备上采集音频数据的命令有什么不同？
思路：

Windows采集音频数据使用的是 dshow 技术。
Linux采集音频数据使用的是 alsa/oss 技术。
Mac采集音频数据使用的是 avfoundation 技术。
点拨：
采集音频数据的命令格式如下：ffmpeg -f 采集技述 -i 设备（可以是设备名或设备索引） 输出媒体名称。

## 解答
在 **Windows**、**Linux** 和 **Mac** 系统上采集音频数据时，使用的工具、命令和设备接口有所不同，具体如下：

---

## **1. Windows 音频采集**
### **工具/接口**：DirectSound、WASAPI、FFmpeg
#### **常见命令：**
- 使用 **FFmpeg**：
  ```bash
  ffmpeg -f dshow -i audio="麦克风 (Realtek Audio)" output.wav
  ```
  - **`-f dshow`**：使用 DirectShow 作为输入格式。
  - **`audio="设备名称"`**：指定音频输入设备名称（可用设备通过 `ffmpeg -list_devices true -f dshow -i dummy` 查看）。
  - **`output.wav`**：录制的音频输出文件。

- 列出可用设备：
  ```bash
  ffmpeg -list_devices true -f dshow -i dummy
  ```

- 使用 **SoX**：
  ```bash
  rec -c 2 -r 44100 -b 16 output.wav
  ```
  - **`-c`**：通道数。
  - **`-r`**：采样率。
  - **`-b`**：比特深度。
  - 默认使用系统的默认麦克风设备。

#### **注意事项：**
- 在 Windows 中，录制设备通常使用 `DirectShow` 或 `WASAPI`，需要确保设备名称正确。
- 如果录制虚拟设备（例如 "Stereo Mix"），可能需要在系统中启用相关设备。

---

## **2. Linux 音频采集**
### **工具/接口**：ALSA、PulseAudio、FFmpeg、SoX
#### **常见命令：**
- 使用 **FFmpeg**：
  ```bash
  ffmpeg -f alsa -i hw:0,0 output.wav
  ```
  - **`-f alsa`**：指定 ALSA 作为音频输入。
  - **`hw:0,0`**：设备号，表示声卡 ID 和设备 ID（可通过 `arecord -l` 查看）。

- 列出可用设备：
  ```bash
  arecord -l
  ```

- 使用 **SoX**：
  ```bash
  rec -r 44100 -c 2 output.wav
  ```
  - 默认使用 PulseAudio 的默认设备。
  - 可以通过 `pacmd list-sources` 查看设备并指定。

- 使用 **ALSA 的 arecord**：
  ```bash
  arecord -D hw:0,0 -f cd -t wav output.wav
  ```
  - **`-D hw:0,0`**：指定设备。
  - **`-f cd`**：使用 CD 质量（16 位、44100 Hz、立体声）。

#### **注意事项：**
- Linux 中常用的音频框架是 **ALSA** 或 **PulseAudio**。
- 确保所需设备未被其他程序占用，否则可能需要释放设备或更改采样参数。

---

## **3. Mac 音频采集**
### **工具/接口**：CoreAudio、AVFoundation、FFmpeg
#### **常见命令：**
- 使用 **FFmpeg**：
  ```bash
  ffmpeg -f avfoundation -i ":0" output.wav
  ```
  - **`-f avfoundation`**：指定 AVFoundation 作为输入。
  - **`:0`**：音频设备 ID，可通过 `ffmpeg -f avfoundation -list_devices true -i ""` 查看。

- 列出可用设备：
  ```bash
  ffmpeg -f avfoundation -list_devices true -i ""
  ```

- 使用 **SoX**：
  ```bash
  rec -r 44100 -c 2 output.wav
  ```
  - 默认使用系统的默认麦克风。

#### **注意事项：**
- Mac 系统主要通过 **CoreAudio** 或 **AVFoundation** 接口采集音频。
- 如果有多个音频输入设备，需要通过设备 ID 指定。

---

## **系统命令对比总结**

| 系统      | 工具         | 列出设备命令                                      | 录制音频命令                              | 主要接口       |
|-----------|--------------|--------------------------------------------------|-------------------------------------------|----------------|
| Windows   | FFmpeg       | `ffmpeg -list_devices true -f dshow -i dummy`    | `ffmpeg -f dshow -i audio="设备名" output.wav` | DirectShow/WASAPI |
|           | SoX          | N/A                                              | `rec -r 44100 -c 2 output.wav`            | 默认设备        |
| Linux     | FFmpeg       | `arecord -l` 或 `pacmd list-sources`             | `ffmpeg -f alsa -i hw:0,0 output.wav`     | ALSA/PulseAudio |
|           | SoX          | `pacmd list-sources`                             | `rec -r 44100 -c 2 output.wav`            | PulseAudio      |
|           | arecord      | `arecord -l`                                     | `arecord -D hw:0,0 -f cd output.wav`      | ALSA           |
| Mac       | FFmpeg       | `ffmpeg -f avfoundation -list_devices true -i ""` | `ffmpeg -f avfoundation -i ":0" output.wav` | AVFoundation   |
|           | SoX          | N/A                                              | `rec -r 44100 -c 2 output.wav`            | CoreAudio      |

---

## **选择工具建议**
1. **FFmpeg**：
   - 功能强大，支持多种格式和采样配置。
   - 适合需要灵活控制采样率、通道数的场景。

2. **SoX**：
   - 简单易用，适合快速录制基本音频。

3. **平台特定工具**：
   - Windows：DirectShow 和 WASAPI 支持更广泛的设备。
   - Linux：ALSA 和 PulseAudio 是标准接口。
   - Mac：AVFoundation 和 CoreAudio 是系统级解决方案。


