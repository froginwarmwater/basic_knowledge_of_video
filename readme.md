# 音视频基础知识学习

1. 通过命令方式采集音频
ffmpeg -f avfoundation -i :0 out.wav

ffplay out.wav
wav pcm数据+wav的header (采样率,采样大小,通道数)

2. swift 基础知识
action: self
target: #selector(myfunc)
@objc func