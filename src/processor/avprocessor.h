#include<string>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/common.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libswscale/swscale.h>
}

namespace vision_service
{
class AVProcessor
{
public:
    AVProcessor();
    ~AVProcessor();

    auto start(const std::string& rtspUrl) -> bool;
};
}