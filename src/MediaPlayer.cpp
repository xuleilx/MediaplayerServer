#include "MediaPlayer.h"
#include "INIReader.h"
#include "Log.h"

static const std::string TAG = "MediaPlayer";

MediaPlayer::MediaPlayer(GMainLoop *loop, const std::shared_ptr<IMediaplayerCallbacks> &callbacks)
{
    /* Initialize GStreamer */
    gst_init(nullptr, nullptr);
    pMainLoop = loop;
    mCallbacks = callbacks;
    mCallbacks->setMediaPlayer(this);
}

MediaPlayer::~MediaPlayer()
{
    teardown();
}

/* Process messages from GStreamer */
gboolean MediaPlayer::handle_message(GstBus *bus, GstMessage *msg, gpointer data)
{
    GError *err;
    gchar *debug_info;
    MediaPlayer *pPlayer = static_cast<MediaPlayer *>(data);

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &err, &debug_info);
        logError("%s::%s Error received from element %s: %s\n", TAG.data(), __FUNCTION__, GST_OBJECT_NAME(msg->src), err->message);
        logError("%s::%s Debugging information: %s\n", TAG.data(), __FUNCTION__, debug_info ? debug_info : "none");
        if (pPlayer && pPlayer->mCallbacks)
            pPlayer->mCallbacks->notifyError(err->message);
        g_clear_error(&err);
        g_free(debug_info);
        g_main_loop_quit(pPlayer->pMainLoop);
        break;
    case GST_MESSAGE_EOS:
        logInfo("%s::%s End-Of-Stream reached.\n", TAG.data(), __FUNCTION__);
        if (pPlayer && pPlayer->mCallbacks)
            pPlayer->mCallbacks->notifyEOS();
        g_main_loop_quit(pPlayer->pMainLoop);
        break;
    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
        if (pPlayer && pPlayer->mpPlaybin && pPlayer->mpPlaybin->element())
            if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pPlayer->mpPlaybin->element()))
            {
                if (new_state == GST_STATE_PLAYING)
                {
                    /* Once we are in the playing state, analyze the streams */
                    // analyze_streams(data);
                }
            }
    }
    break;
    }

    /* We want to keep receiving messages */
    return TRUE;
}

bool MediaPlayer::setup()
{
    std::lock_guard<std::mutex> lck(mtx);

    gint flags;
    GstBus *bus;
    GstStateChangeReturn ret;
    std::string confFile("/etc/media.ini");
    std::shared_ptr<INIReader> reader = std::make_shared<INIReader>(confFile);

    if (reader->ParseError() < 0)
    {
        logWarn("%s::%s Can't load config file %s\n", TAG.data(), __FUNCTION__,confFile.data());
    }

    mpPlaybin = std::make_shared<MyElement>("playbin");
    if (mpPlaybin == nullptr)
    {
        logError("%s::%s playbin could be created.\n", TAG.data(), __FUNCTION__);
        return false;
    }

    mpPlaybin->setup(reader);

    if (!mpPlaybin->element())
    {
        logError("%s::%s Not all elements could be created.\n", TAG.data(), __FUNCTION__);
        return false;
    }

    /* Add a bus watch, so we get notified when a message arrives */
    bus = gst_element_get_bus(mpPlaybin->element());
    gst_bus_add_watch(bus, (GstBusFunc)handle_message, this);

    /* Set to ready */
    return changeState(GST_STATE_READY);
}
bool MediaPlayer::start()
{
    std::lock_guard<std::mutex> lck(mtx);
    return changeState(GST_STATE_PLAYING);
}
bool MediaPlayer::pause()
{
    std::lock_guard<std::mutex> lck(mtx);
    return changeState(GST_STATE_PAUSED);
}
bool MediaPlayer::stop()
{
    std::lock_guard<std::mutex> lck(mtx);
    return changeState(GST_STATE_NULL);
}

int64_t MediaPlayer::getDuration()
{
    std::lock_guard<std::mutex> lck(mtx);
    GstFormat format = GST_FORMAT_TIME;
    int64_t dur = 0;
    int duration = -1;

    if (mpPlaybin && mpPlaybin->element())
    {
        if (gst_element_query_duration(mpPlaybin->element(), format, &dur))
        {
            duration = GST_TIME_AS_SECONDS(dur);
        }
    }
    else
    {
        logError("%s::%s mpPlaybin or pPlaybin->element() is null\n", TAG.data(), __FUNCTION__);
    }

    return duration;
}

int64_t MediaPlayer::getPosition()
{
    std::lock_guard<std::mutex> lck(mtx);
    GstFormat format = GST_FORMAT_TIME;
    int64_t position = 0;
    int cur_pts = -1;

    if (mpPlaybin && mpPlaybin->element())
    {
        if (gst_element_query_position(mpPlaybin->element(), format, &position))
        {
            cur_pts = GST_TIME_AS_SECONDS(position);
        }
    }
    else
    {
        logError("%s::%s mpPlaybin or pPlaybin->element() is null\n", TAG.data(), __FUNCTION__);
    }

    return cur_pts;
}

bool MediaPlayer::seek(uint secs)
{
    std::lock_guard<std::mutex> lck(mtx);

    if (mpPlaybin && mpPlaybin->element())
    {
        GstSeekFlags seekFlag = GST_SEEK_FLAG_NONE;
        // seekFlag = (GstSeekFlags)((int)GST_SEEK_FLAG_FLUSH | (int)GST_SEEK_FLAG_KEY_UNIT | (int)GST_SEEK_FLAG_SKIP);
        seekFlag = (GstSeekFlags)((int)GST_SEEK_FLAG_FLUSH | (int)GST_SEEK_FLAG_ACCURATE);
        if (!gst_element_seek_simple(mpPlaybin->element(), GST_FORMAT_TIME, seekFlag, secs * GST_SECOND))
        {
            logError("%s::%s seek-to failed: gst_element_seek_simple(secs = %d)\n", TAG.data(), __FUNCTION__, secs);
            return false;
        }
    }
    else
    {
        logError("%s::%s mpPlaybin or pPlaybin->element() is null\n", TAG.data(), __FUNCTION__);
        return false;
    }

    return true;
}
bool MediaPlayer::setRate(double rate)
{
    std::lock_guard<std::mutex> lck(mtx);

    return send_seek_event(rate);
}
bool MediaPlayer::setFile(std::string filepath)
{
    std::lock_guard<std::mutex> lck(mtx);

    if (mpPlaybin && mpPlaybin->element())
    {
        GError *error = NULL;
        gchar *uriStr = gst_filename_to_uri(filepath.data(), &error);

        /* Set the URI to play */
        g_object_set(G_OBJECT(mpPlaybin->element()), "uri", uriStr, NULL);
        g_free(uriStr);
        return true;
    }
    else
    {
        logError("%s::%s mpPlaybin or pPlaybin->element() is null\n", TAG.data(), __FUNCTION__);
        return false;
    }
}
bool MediaPlayer::teardown()
{
    std::lock_guard<std::mutex> lck(mtx);

    if (mpPlaybin && mpPlaybin->element())
    {
        mpPlaybin->teardown();
        mpPlaybin = nullptr;
        return true;
    }
    else
    {
        logError("%s::%s mpPlaybin or pPlaybin->element() is null\n", TAG.data(), __FUNCTION__);
        return false;
    }
}

bool MediaPlayer::changeState(GstState state)
{
    if (mpPlaybin && mpPlaybin->element())
    {
        GstStateChangeReturn ret;
        logInfo("%s::%s Set the pipeline to the %s state.\n", TAG.data(), __FUNCTION__,
                getGstStateString(state).data());

        ret = gst_element_set_state(mpPlaybin->element(), state);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            logError("%s::%s Unable to set the pipeline to the %s state.\n", TAG.data(), __FUNCTION__,
                     getGstStateString(state).data());
            return false;
        }
        return true;
    }
    else
    {
        logError("%s::%s mpPlaybin or pPlaybin->element() is null\n", TAG.data(), __FUNCTION__);
        return false;
    }
}

std::string MediaPlayer::getGstStateString(GstState state)
{
    std::string stateStr;
    switch (state)
    {
    case GST_STATE_NULL:
        stateStr = "GST_STATE_NULL";
        break;
    case GST_STATE_READY:
        stateStr = "GST_STATE_READY";
        break;
    case GST_STATE_PAUSED:
        stateStr = "GST_STATE_PAUSED";
        break;
    case GST_STATE_PLAYING:
        stateStr = "GST_STATE_PLAYING";
        break;
    default:
        stateStr = "UNKNOWN";
        break;
    }
    return stateStr;
}

/* Send seek event to change rate */
bool MediaPlayer::send_seek_event(double rate)
{
    if (mpPlaybin && mpPlaybin->element())
    {
        gint64 position;
        GstEvent *seek_event;
        bool ret = FALSE;

        /* Obtain the current position, needed for the seek event */
        if (!gst_element_query_position(mpPlaybin->element(), GST_FORMAT_TIME, &position))
        {
            logError("%s::%s Unable to retrieve current position.\n", TAG.data(), __FUNCTION__);
            return false;
        }

        /* Create the seek event */
        if (rate > 0)
        {
            seek_event =
                gst_event_new_seek(rate, GST_FORMAT_TIME,
                                   (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET,
                                   position, GST_SEEK_TYPE_END, 0);
        }
        else
        {
            seek_event =
                gst_event_new_seek(rate, GST_FORMAT_TIME,
                                   (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET, 0,
                                   GST_SEEK_TYPE_SET, position);
        }

        /* Send the event */
        return gst_element_send_event(mpPlaybin->element(), seek_event);
    }
    else
    {
        logError("%s::%s mpPlaybin or pPlaybin->element() is null\n", TAG.data(), __FUNCTION__);
        return false;
    }
}
