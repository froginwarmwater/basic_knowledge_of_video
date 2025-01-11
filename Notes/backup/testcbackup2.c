//
//  testc.c
//  myapp
//
//  Created by 李昕阳 on 31/12/2024.
//

#include "testc.h"
#include <unistd.h>
#include <string.h>
static int rec_status = 0;

void haha(void) {
    printf("haha");
}

void set_rec_status(int status) {
    rec_status = status;
}
    
SwrContext* init_swr(void) {
    SwrContext *swr_ctx = NULL;
    // channel number
    
    
    //AV_CH_LAYOUT_STEREO 定义左前方和右前方
    swr_alloc_set_opts(NULL, // <#struct SwrContext *s#>   ctx
                       AV_CH_LAYOUT_STEREO, // <#int64_t out_ch_layout#> 输出channel布局
                       AV_SAMPLE_FMT_S16,   // <#enum AVSampleFormat out_sample_fmt#> 输出的采样格式
                       44100,               // <#int out_sample_rate#> 采样率
                       AV_CH_LAYOUT_STEREO, // <#int64_t in_ch_layout#> 输入的channel 布局
                       AV_SAMPLE_FMT_FLT,   // <#enum AVSampleFormat in_sample_fmt#> 输入的采样格式
                       44100,       // <#int in_sample_rate#> 输入的采样率
                       0,           //
                       NULL);
    
    if (!swr_ctx) {
        
    }
    
    if(swr_init(swr_ctx) < 0) {
        
    }
    return swr_ctx;
}

void record_audio(void) {
    av_log_set_level(AV_LOG_DEBUG);
    
    char errors[1024] = {0, };
    
    // 重采样缓冲区
    uint8_t **src_data = NULL;
    int src_linesize = 0;
    
    uint8_t **dst_data = NULL;
    int dst_linesize = 0;
    
    AVFormatContext *fmt_ctx = NULL;
    //  [[videodevice]: [audiodevice]]
    char *devicename = ":0";
    rec_status = 1;
    avdevice_register_all();
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    
    AVDictionary *options = NULL;
    // add
    av_dict_set(&options, "sample_rate", "48000", 0); // 采样率 48 kHz
    av_dict_set(&options, "channels", "1", 0);       // 双声道
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
    
    
    // 我们之前采集的数据是4096  4096 / 4 = 1024
    // 双通道 所以1024 / 2 = 512 则为单通道 
    //创建输入缓冲区
    av_samples_alloc_array_and_samples(&src_data,  // 输出参数 缓冲区地址
                                       &src_linesize, // 缓冲区大小
                                       2,           // 通道个数
                                       512,         // 单通道采样个数
                                       AV_SAMPLE_FMT_FLT,   // 采样格式
                                       0);
    //创建输出缓冲区
    av_samples_alloc_array_and_samples(&dst_data,  // 输出参数 缓冲区地址
                                       &dst_linesize, // 缓冲区大小
                                       2,           // 通道个数
                                       512,         // 单通道采样个数
                                       AV_SAMPLE_FMT_S16,   // 采样格式
                                       0);
    
    
    SwrContext *swr_ctx = NULL;
    // channel number
    
    
    //AV_CH_LAYOUT_STEREO 定义左前方和右前方
    swr_ctx = init_swr();
    
    
    
    // 读出来的数据放到AVPacket 里面
    AVPacket pkt;
    av_init_packet(&pkt);
    
    usleep(500000); // 500 ms

    ret = av_read_frame(fmt_ctx, &pkt);
    printf("%d\n", ret);
    
    while (rec_status) {
        ret = av_read_frame(fmt_ctx, &pkt);

        if (ret == -35) {
//            printf("ret == -35, waiting...\n");
            usleep(100); // 短暂等待
            continue;
        }

        if (ret < 0) {
            printf("Error reading frame: %d\n", ret);
            break;
        }
        
        av_log(NULL, AV_LOG_INFO, "packet size is %d(%p)\n", pkt.size, pkt.data);

//      进行内存拷贝, 按字节拷贝
        memcpy((void *)src_data[0], pkt.data, pkt.size);
        swr_convert(swr_ctx,    // 重采样的上下文
                    dst_data,   // 输出结果缓冲区
                    512,        // 每个通道的采样数
                    (const uint8_t **)src_data, // 输入缓冲区
                    512);       // 输入单个通道的采样数
        if (pkt.size > 0 && pkt.data) {
//            fwrite(pkt.data, pkt.size, 1, outfile);
            fwrite(dst_data[0], 1, dst_linesize, outfile);
            fflush(outfile);
        }

        av_packet_unref(&pkt);
    }

    
    fclose(outfile);
    
    // 释放输入输出缓冲区
    if (src_data) {
        av_freep(&src_data[0]);
    }
    av_freep(&src_data);
    
    if (dst_data) {
        av_freep(&dst_data[0]);
    }
    av_freep(&dst_data);
    
    // 释放重采样的上下文
    swr_free(&swr_ctx);
    
    
    avformat_close_input(&fmt_ctx);
    
    
    av_log(NULL, AV_LOG_DEBUG, "finish!");
    return;
}
