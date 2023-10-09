#include <fmt/core.h>
#include <iostream>

#include "avprocessor.h"

namespace vision_service
{
    AVProcessor::AVProcessor()
    {
    }

    AVProcessor::~AVProcessor()
    {
    }

    auto AVProcessor::start(const std::string& rtspUrl) -> bool
    {        
        avformat_network_init();

        int ret;

        //open stream
        AVFormatContext* format_ctx = NULL;
        ret = avformat_open_input(&format_ctx, rtspUrl.c_str(), NULL,NULL);
        if (ret < 0) {

            std::cerr << fmt::format("Failed to open the video stream: {}", rtspUrl) << std::endl;
            return false;
        }
        std::cout<< fmt::format("Successfully opened video stream: {}\n", rtspUrl)
            << fmt::format("Name: {}\n", format_ctx->iformat->long_name)
            << fmt::format("Duration: {}\n", format_ctx->duration) 
            << std::endl;

        
        // get stream info
        // The following blockes the thread when real time streaming.. commenting out
        /*
        ret = avformat_find_stream_info(format_ctx, NULL);
        if (ret < 0) {
            std::cerr << fmt::format("Can't get stream info. Result={}", ret) << std::endl;
            return false;
        }
        std::cout << "Found stream" << std::endl;
        */

        // find primary video stream       
        int video_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if (video_stream_index < 0) {
            std::cerr << fmt::format("fail to av_find_best_stream: ret={}", ret) << std::endl;
            return 2;
        }

        std::cout << fmt::format("Found video stream index: {}", video_stream_index) << std::endl;
                
        AVStream* vstrm = format_ctx->streams[video_stream_index];        
        AVCodecParameters *origin_par = vstrm->codecpar;
        
        const AVCodec* codec = nullptr;
        codec = avcodec_find_decoder(origin_par->codec_id);
        if (!codec) {
            std::cerr << "Can't find a decoder" << std::endl;
            return false;
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
        if (!codec_ctx) {
            std::cerr << "Can't allocate decoder context" << std::endl;
            return false;
        }

        ret = avcodec_parameters_to_context(codec_ctx, origin_par);
        if (ret < 0) {
            std::cerr << fmt::format("Can't copy deocder context. Result={}", ret) << std::endl;
            return false;            
        }
        
        std::cout << "Creating context for codec: \n"
            << fmt::format("name: {}, long name: {}\n", codec->name, codec->long_name)
            << fmt::format("media type: {}", av_get_media_type_string(codec->type))
            << std::endl;
        // open video decoder context
        ret = avcodec_open2(codec_ctx, codec, nullptr);
        if (ret < 0) {
            std::cerr << fmt::format("fail to avcodec_open2: ret={}", ret) << std::endl;
            return false;
        }

        // start reading packets from stream and write them to file        
        AVFrame* frame = av_frame_alloc();
        if (!frame) {
            std::cerr << "Can't allocate frame" << std::endl;
            return false;
        }

        AVPacket* pkt =  av_packet_alloc();
        if (!pkt) {
            std::cerr << "Can't allocate packet" << std::endl;
            return false;
        }

        size_t width = 640;
        size_t height = 480;
        AVPixelFormat pix_fmt = AV_PIX_FMT_RGB24;
        if (codec_ctx->width > 0 && codec_ctx->height > 0) {
            width = codec_ctx->width;
            height = codec_ctx->height;
            pix_fmt = codec_ctx->pix_fmt;
        }
        std::cout << "Codec Context info:\n"
            << fmt::format("Width: {}, Height: {}\n", width, height)
            << std::endl;

        int byte_buffer_size = av_image_get_buffer_size(codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height, 16);
        
        uint8_t* byte_buffer = NULL;
        av_malloc(byte_buffer_size);        

        std::cout 
            << fmt::format("Num: {}, Den: {}", format_ctx->streams[video_stream_index]->time_base.num, format_ctx->streams[video_stream_index]->time_base.den)
            << std::endl;
        
        int i = 0;
        
        ret = 0;
        std::cout << "Starting processing the frames ..." << std::endl;
        while (ret >= 0) {
            ret = av_read_frame(format_ctx, pkt);
            std::cout << "gets here" << std::endl;
            if (ret >= 0 && pkt->stream_index != video_stream_index) {
                av_packet_unref(pkt);
                continue;
            }

            if (ret < 0) {
                ret = avcodec_send_packet(codec_ctx, NULL);
            } else {
                if (pkt->pts == AV_NOPTS_VALUE) {
                    pkt->pts = pkt->dts = i;
                }
                ret = avcodec_send_packet(codec_ctx, pkt);
            }
            av_packet_unref(pkt);

            if (ret < 0) {
                std::cerr << "Error submitting a packet for decoding" << std::endl;
                return false;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret == AVERROR_EOF) {
                    goto finish;
                } else if (ret == AVERROR(EAGAIN)) {
                    ret = 0;
                    break;
                } else if (ret < 0) {
                    std::cerr << "Error decoding frame" << std::endl;
                    return false;
                }

                int number_of_written_bytes = av_image_copy_to_buffer(byte_buffer, byte_buffer_size,
                    (const uint8_t* const *)frame->data, (const int*) frame->linesize,
                    codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height, 1);
                
                if (number_of_written_bytes < 0) {
                    std::cerr << "Can't copy image to buffer" << std::endl;
                    av_frame_unref(frame);
                    return false;
                }
                //fmt::format("{}, {}, {}, {}, {}, {}", video_stream_index,
                //    av_err2str(frame->pts), av_err2str(frame->pkt_dts), frame->duration,
                //    number_of_written_bytes, av_adler32_update(0, (const uint8_t*)byte_buffer, number_of_written_bytes));
                std::cout << fmt::format("{}, {}", video_stream_index, frame->format) << std::endl;

                av_frame_unref(frame);
            }
            i++;
        }

    finish:
        av_packet_free(&pkt);
        av_frame_free(&frame);
        avformat_close_input(&format_ctx);
        avcodec_free_context(&codec_ctx);
        av_freep(&byte_buffer);
        avformat_network_deinit();

        return true;
    }
}

/*
        std::cout << "Found primary video stream.\n" 
            << fmt::format("name: {}\n", vcodec->name)
            //<< fmt::format("type: {}\n", av_get_media_type_string(vcodec->type))
            //<< fmt::format("wrapper name: {} ", vcodec->wrapper_name)
            << std::endl;
        std::cout << "Video stream info:\n"
            << fmt::format("Num of frames: {}\n", vstrm->nb_frames)
            << fmt::format("Avg frame rate: num={}, den={}\n", vstrm->avg_frame_rate.num, vstrm->avg_frame_rate.den)
            << fmt::format("Real frame rate: num={}, den={}\n", vstrm->r_frame_rate.num, vstrm->r_frame_rate.den)
            << fmt::format("Codec type: {}\n", av_get_media_type_string(vstrm->codecpar->codec_type))
            << fmt::format("Frame size: {}", vstrm->codecpar->frame_size)
            << std::endl;        
            AVFrame* dec_frame = av_frame_alloc();
        AVPacket pkt = { nullptr, 0 };
        unsigned nb_frames = 0;
        int got_pic = 0;
        std::cout << fmt::format("Packet size: {}", pkt.size) << std::endl;
        std::cout << "Video decoder set." << std::endl;
       
        std::cout << fmt::format("{} frames decoded", nb_frames) << std::endl;
        bool end_of_stream = false;
             do {
            if (!end_of_stream) {
                std::cout << "Reading the frame" << std::endl;

                ret = av_read_frame(format_ctx, &pkt);
                std::cout << fmt::format("Read stream ret={}", ret) << std::endl;
                if (ret < 0 && ret != AVERROR_EOF) {
                    std::cerr << fmt::format("fail to av_read_frame: ret={}",ret) << std::endl;
                    return false;
                }
                std::cout << fmt::format("Packet index: {}", pkt.stream_index);
                if (ret == 0 && pkt.stream_index != video_stream_index) {
                    goto next_packet;
                }
                end_of_stream = (ret == AVERROR_EOF);
            }

            if (end_of_stream) {
                // null packet for bumping process
                av_init_packet(&pkt);
                pkt.data = nullptr;
                pkt.size = 0;            
            }

            // decode video frame
            //avcodec_decode_video2(codec_ctx, dec_frame, &got_pic, &pkt);
            std::cout << fmt::format("gets here") << std::endl;
            ret = avcodec_send_packet(codec_ctx, &pkt);
            std::cout << fmt::format("ret={}", ret) << std::endl;
            if (ret < 0 && ret != AVERROR_EOF) {
                std::cerr << "Can't decode the packet" << std::endl;
                return false;
            } else {
                std::cout << " got frame decoded..." << std::endl;
            }
            if (!got_pic) {
                goto next_packet;
            }
             ++nb_frames;
next_packet:
            //av_free_packet(&pkt);
        } while(!end_of_stream);

        */