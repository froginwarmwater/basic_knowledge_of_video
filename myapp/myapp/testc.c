//
//  testc.c
//  myapp
//
//  Created by 李昕阳 on 31/12/2024.
//

#include "testc.h"
#include <unistd.h>


void haha(void) {
    printf("haha");
}
    
void record_audio(void) {
    av_log_set_level(AV_LOG_DEBUG);
    
    char errors[1024] = {0, };
    AVFormatContext *fmt_ctx = NULL;
    //  [[videodevice]: [audiodevice]]
    char *devicename = ":0";
    
    avdevice_register_all();
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    
    AVDictionary *options = NULL;
    // add
    av_dict_set(&options, "sample_rate", "44100", 0); // 采样率 44.1 kHz
    av_dict_set(&options, "channels", "2", 0);       // 双声道
    av_dict_set(&options, "sample_format", "fltp", 0); // 浮点音频格式

    
    // open device
    int ret = 0;
    ret = avformat_open_input(&fmt_ctx, devicename, iformat, &options);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        fprintf(stderr, "Fail to open audio device[%d]%s\n", ret, errors);
        return;
    }
    
    //create file
    char *out = "/Users/froginwarmwater/Documents/audio_dev/audio.pcm";
    FILE* outfile = fopen(out, "wb+");
    
    
    // 读出来的数据放到AVPacket 里面
    AVPacket pkt;
    av_init_packet(&pkt);
    int count = 0;
    
    usleep(500000); // 500 ms

    ret = av_read_frame(fmt_ctx, &pkt);
    printf("%d\n", ret);
    
    while (count++ < 50000) {
        ret = av_read_frame(fmt_ctx, &pkt);

        if (ret == -35) {
            printf("ret == -35, waiting...\n");
            usleep(100); // 短暂等待
            continue;
        }

        if (ret < 0) {
            printf("Error reading frame: %d\n", ret);
            break;
        }

        if (pkt.size > 0 && pkt.data) {
            fwrite(pkt.data, pkt.size, 1, outfile);
            fflush(outfile);
        }

        av_packet_unref(&pkt);
    }

    
    fclose(outfile);
    
    avformat_close_input(&fmt_ctx);
    
    
    av_log(NULL, AV_LOG_DEBUG, "finish!");
    return;
}
