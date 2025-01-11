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
    
SwrContext* init_swr(int64_t in_ch_layout, enum AVSampleFormat in_sample_fmt, int in_sample_rate,
                     int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate) {
    SwrContext *swr_ctx = swr_alloc_set_opts(
        NULL,
        out_ch_layout,
        out_sample_fmt,
        out_sample_rate,
        in_ch_layout,
        in_sample_fmt,
        in_sample_rate,
        0,
        NULL);

    if (!swr_ctx) {
        fprintf(stderr, "Failed to allocate SwrContext\n");
        return NULL;
    }

    if (swr_init(swr_ctx) < 0) {
        fprintf(stderr, "Failed to initialize SwrContext\n");
        swr_free(&swr_ctx);
        return NULL;
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
    char *devicename = ":2";
    rec_status = 1;
    avdevice_register_all();
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    
    AVDictionary *options = NULL;
    // add
    av_dict_set(&options, "sample_rate", "48000", 0); // 采样率 48 kHz
    av_dict_set(&options, "channels", "1", 0);       // 单声道
    av_dict_set(&options, "sample_format", "f32le", 0); // 浮点音频格式

    
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
    
    

    SwrContext *swr_ctx = init_swr(
        AV_CH_LAYOUT_MONO,    // 输入通道布局（单声道）
        AV_SAMPLE_FMT_FLT,    // 输入采样格式（16 位有符号整型）
        48000,                // 输入采样率
        AV_CH_LAYOUT_STEREO,  // 输出通道布局（立体声，假如需要）
        AV_SAMPLE_FMT_FLT,    // 输出采样格式（32 位浮点型，假如需要）
        48000                 // 输出采样率（根据需要调整）
    );
    
    int package_size = 2048;
    int in_samples = package_size / 2;
//    int out_samples = av_rescale_rnd(swr_get_delay(swr_ctx, 48000) + in_samples, 44100, 48000, AV_ROUND_UP);
    int out_samples = av_rescale_rnd(
                                                   swr_get_delay(swr_ctx, 48000) + in_samples,
                                                   48000,  // 输出采样率
                                                   48000,  // 输入采样率
                                                   AV_ROUND_UP
                                               );
    printf("out_samples = %d", out_samples);
//    int total_out_samples = av_rescale_rnd(
//        swr_get_delay(swr_ctx, 48000) + in_samples,
//        48000,
//        48000,
//        AV_ROUND_UP
//    );
//    int single_channel_samples = total_out_samples / 2;

    // 我们之前采集的数据是4096
    // packet size is 4096(0x7fe6a3880a00)
    //  4096 / 2 = 2048 // 因为s16是2个字节

    // av_samples_alloc_array_and_samples(&src_data, &src_linesize, 1, 512, AV_SAMPLE_FMT_FLT, 0);
    // av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, 1, 512, AV_SAMPLE_FMT_S16, 0);
    //创建输入缓冲区
    av_samples_alloc_array_and_samples(&src_data,  // 输出参数 缓冲区地址
                                       &src_linesize, // 缓冲区大小
                                       1,           // 通道个数
                                       in_samples,         // 单通道采样个数
                                       AV_SAMPLE_FMT_FLT,   // 采样格式
                                       0);
    //创建输出缓冲区
    av_samples_alloc_array_and_samples(&dst_data,  // 输出参数 缓冲区地址
                                       &dst_linesize, // 缓冲区大小
                                       2,           // 通道个数
                                       out_samples / 2,         // 单通道采样个数
                                       AV_SAMPLE_FMT_FLT,   // 采样格式
                                       0);
    

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

////      进行内存拷贝, 按字节拷贝
//        memcpy((void *)src_data[0], pkt.data, pkt.size);
//        swr_convert(swr_ctx,    // 重采样的上下文
//                    dst_data,   // 输出结果缓冲区
//                    out_samples,        // 每个通道的采样数
//                    (const uint8_t **)src_data, // 输入缓冲区
//                    in_samples);       // 输入单个通道的采样数
//        if (pkt.size > 0 && pkt.data) {
////            fwrite(pkt.data, pkt.size, 1, outfile);
//            fwrite(dst_data[0], 1, dst_linesize, outfile);
//            fflush(outfile);
//        }
        
        memcpy((void *)src_data[0], pkt.data, pkt.size);
        int converted_samples = swr_convert(
            swr_ctx,    // 重采样的上下文
            dst_data,   // 输出结果缓冲区
            out_samples,// 每个通道的采样数
            (const uint8_t **)src_data, // 输入缓冲区
            in_samples  // 输入单通道的采样数
        );
//        if (converted_samples > 0) {
//            size_t bytes_to_write = converted_samples * 2 * 4; // 2 通道 * 每样本 4 字节 (FLT)
//            fwrite(dst_data[0], 1, bytes_to_write, outfile);
//            fflush(outfile);
//        }

//
        if (pkt.size > 0 && pkt.data) {
//            fwrite(pkt.data, pkt.size, 1, outfile);
//            size_t bytes_to_write = converted_samples * 2 * 4; // 2 通道 * 每样本 4 字节 (FLT)
//            fwrite(dst_data[0], 1, bytes_to_write, outfile);
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
