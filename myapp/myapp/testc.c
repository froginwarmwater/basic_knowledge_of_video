//
//  testc.c
//  myapp
//
//  Created by 李昕阳 on 31/12/2024.
//

#include "testc.h"
#include <unistd.h>


void haha(void) {
    
    char errors[1024];
    AVFormatContext *fmt_ctx = NULL;
    //  [[videodevice]: [audiodevice]]
    char *devicename = ":0";
    
    avdevice_register_all();
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    
    AVDictionary *options = NULL;
    
    
    int ret = 0;
    ret = avformat_open_input(&fmt_ctx, devicename, iformat, &options);
    if (ret < 0) {
        av_strerror(ret, errors, 1024);
        fprintf(stderr, "Fail to open audio device[%d]%s\n", ret, errors);
    }
    
    // 读出来的数据放到AVPacket 里面
    AVPacket pkt;
    av_init_packet(&pkt);
    int count = 0;
    ret = av_read_frame(fmt_ctx, &pkt);
    printf("%d\n", ret);
    while (count++ < 500) {
        
        ret = av_read_frame(fmt_ctx, &pkt);
        
        // 这里可能读取的时候，ret返回-35 表示设备还没准备好, 先睡眠1s
        if (ret == -35) {
            sleep(1);
            continue;
        }
        
        printf("read frame --- ret %d \n",ret);
        if (ret < 0) {
            break;
        }
        printf("pkt size is %d(%p) %d\n" , pkt.size, pkt.data, count);
        av_packet_unref(&pkt);
        
    }
    
    avformat_close_input(&fmt_ctx);
    
    av_log_set_level(AV_LOG_DEBUG);
    av_log(NULL, AV_LOG_DEBUG, "hello world");
    return;
}
