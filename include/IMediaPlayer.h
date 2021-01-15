#ifndef IMEDIAPLAYER_H
#define IMEDIAPLAYER_H
#include <iostream>

class IMediaPlayer
{
public:
    virtual ~IMediaPlayer() {}
    /**
     * Create mediaplayer. Construct pipeline, and set the pipeline to GST_STATE_READY.
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool setup() = 0;

    /**
     * Play the media. set pipeline in GST_STATE_PLAYING state.
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool start() = 0;

    /**
     * Pause the media. set pipeline in GST_STATE_PAUSED state.
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool pause() = 0;

    /**
     * Stop playing. set pipeline in GST_STATE_NULL state.
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool stop() = 0;

    /**
     * Seek position
     * @param secs Seek position in seconds.(0 ~ duration)
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool seek(uint secs) = 0;

    /**
     * get position
     * @return -1 if the call failed, otherwise the seconds.
     */
    virtual int64_t getPosition() = 0;

    /**
     * get Duration
     * @param -1 if the call failed, otherwise the seconds.
     */
    virtual int64_t getDuration() = 0;

    /**
     * Set play speed
     * @param rate Play speed.
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool setRate(double rate) = 0;

    /**
     * Set media file
     * @param filepath File absolute path.
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool setFile(std::string filepath) = 0;

    /**
     * Destory mediaplayer. Destruct pipeline.
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool teardown() = 0;
};

#endif // IMEDIAPLAYER_H
