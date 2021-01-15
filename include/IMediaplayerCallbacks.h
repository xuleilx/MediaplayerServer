#ifndef IMEDIAPLAYERCALLBACKS_H
#define IMEDIAPLAYERCALLBACKS_H

#include <iostream>
#include <memory>

class IMediaPlayer;
/**
 * This class represents an interface that every player implementation must subclass if they
 * wish to implement IMediaPlayer. You should look at the documentation for class
 * IMediaPlayer for a pseudocode example of how to set up an IMediaPlayer.
 */
class IMediaplayerCallbacks
{
public:
    virtual ~IMediaplayerCallbacks() {}
    virtual void setMediaPlayer(IMediaPlayer *mediaPlayer){mMediaPlayer = mediaPlayer; }
    /**
     * Progress bar refresh
     * @param position A location in which to store the current position
     * @param duration A location in which to store the total duration
     */
    virtual void notifyProgress(uint position, uint duration) = 0;

    /**
     * Error message
     * @param messge The reason of the error.
     */
    virtual void notifyError(std::string messge) = 0;

    /**
     * End of stream
     */
    virtual void notifyEOS() = 0;

protected:
    IMediaPlayer *mMediaPlayer;
};

#endif // IMEDIAPLAYERCALLBACKS_H
