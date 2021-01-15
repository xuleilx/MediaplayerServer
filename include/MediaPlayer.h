#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H
#include <memory>
#include <gst/gst.h>
#include <mutex>
#include "IMediaPlayer.h"
#include "IMediaplayerCallbacks.h"
#include "MyElement.h"

class MediaPlayer : public IMediaPlayer
{
public:
    MediaPlayer(GMainLoop *loop, const std::shared_ptr<IMediaplayerCallbacks> &callbacks);
    ~MediaPlayer();

    bool setup() override;
    bool start() override;
    bool pause() override;
    bool stop() override;
    int64_t getPosition() override;
    int64_t getDuration() override;
    bool seek(uint secs) override;
    bool setRate(double rate) override;
    bool setFile(std::string filepath) override;
    bool teardown() override;

private:
    bool changeState(GstState state);
    bool send_seek_event(double rate);
    std::string getGstStateString(GstState state);
    static gboolean handle_message(GstBus *bus, GstMessage *msg, gpointer data);

private:
    std::shared_ptr<IMediaplayerCallbacks> mCallbacks;
    std::shared_ptr<MyElement> mpPlaybin;
    GMainLoop *pMainLoop;
    std::mutex mtx;
};

#endif // MEDIAPLAYER_H