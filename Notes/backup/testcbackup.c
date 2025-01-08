// Updated version of your code

#include "testc.h"
#include <unistd.h>
#include <string.h>
#include <libavutil/log.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>

static int rec_status = 0;

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

    char errors[1024] = {0};

    uint8_t **src_data = NULL;
    int src_linesize = 0;

    uint8_t **dst_data = NULL;
    int dst_linesize = 0;

    AVFormatContext *fmt_ctx = NULL;
    char *devicename = ":0";

    avdevice_register_all();
    AVInputFormat *iformat = av_find_input_format("avfoundation");

    AVDictionary *options = NULL;
    av_dict_set(&options, "sample_rate", "48000", 0);
    av_dict_set(&options, "channels", "1", 0);
    av_dict_set(&options, "sample_format", "fltp", 0);

    int ret = avformat_open_input(&fmt_ctx, devicename, iformat, &options);
    if (ret < 0) {
        av_strerror(ret, errors, sizeof(errors));
        fprintf(stderr, "Failed to open audio device: [%d] %s\n", ret, errors);
        return;
    }

    char *out = "/Users/froginwarmwater/Documents/audio_dev/audio.pcm";
    FILE* outfile = fopen(out, "wb+");
    if (!outfile) {
        fprintf(stderr, "Failed to open output file\n");
        avformat_close_input(&fmt_ctx);
        return;
    }

    av_samples_alloc_array_and_samples(&src_data, &src_linesize, 1, 512, AV_SAMPLE_FMT_FLT, 0);
    av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, 1, 512, AV_SAMPLE_FMT_S16, 0);

    SwrContext *swr_ctx = init_swr(AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_FLT, 48000,
                                   AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 48000);
    if (!swr_ctx) {
        fclose(outfile);
        avformat_close_input(&fmt_ctx);
        return;
    }

    AVPacket pkt;
    av_init_packet(&pkt);

    while (rec_status) {
        ret = av_read_frame(fmt_ctx, &pkt);
        if (ret == AVERROR(EAGAIN)) {
            usleep(100); // Wait briefly if no data is available
            continue;
        } else if (ret < 0) {
            fprintf(stderr, "Error reading frame: %d\n", ret);
            break;
        }

        memcpy(src_data[0], pkt.data, pkt.size);
        swr_convert(swr_ctx, dst_data, 512, (const uint8_t **)src_data, 512);

        fwrite(dst_data[0], 1, dst_linesize, outfile);
        fflush(outfile);

        av_packet_unref(&pkt);
    }

    fclose(outfile);
    if (src_data) {
        av_freep(&src_data[0]);
    }
    av_freep(&src_data);

    if (dst_data) {
        av_freep(&dst_data[0]);
    }
    av_freep(&dst_data);

    swr_free(&swr_ctx);
    avformat_close_input(&fmt_ctx);

    av_log(NULL, AV_LOG_DEBUG, "Recording finished!\n");
}
