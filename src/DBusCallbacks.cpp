#include "DBusCallbacks.h"
#include "Log.h"
#include "MediaInfo.h"

#define MEDIAPLAYER_DBUS_NAME "com.hsae.mediaplayerserver.mediaplayer"
#define MEDIAPLAYER_DBUS_OBJECT_PATH "/com/hsae/mediaplayerserver"
#define MEDIAINFO_DBUS_NAME "com.hsae.mediaplayerserver.mediainfo"
#define MEDIAINFO_DBUS_OBJECT_PATH "/com/hsae/mediaplayerserver"

static const std::string TAG = "DBusCallbacks";

DBusCallbacks::DBusCallbacks() : mMediaplayerInterface(nullptr), mMediainfoInterface(nullptr), mLoop(nullptr) {}
DBusCallbacks::~DBusCallbacks()
{
    if (mMediaplayerInterface)
    {
        g_clear_object(&mMediaplayerInterface);
        mMediaplayerInterface = nullptr;
    }
    if (mMediainfoInterface)
    {
        g_clear_object(&mMediainfoInterface);
        mMediainfoInterface = nullptr;
    }

    if (mLoop)
    {
        g_main_loop_quit(mLoop);
        g_main_loop_unref(mLoop);
    }

    mLoopThread.get();
}

void DBusCallbacks::init()
{
    mLoop = g_main_loop_new(nullptr, FALSE);
    mLoopThread = std::async(std::launch::async, std::bind(&DBusCallbacks::runmainloop, this));

    registerMediaplayerServer();
    registerMediainfoServer();

    g_timeout_add_seconds(1, (GSourceFunc)updateProgress, this);

    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck);
}

void DBusCallbacks::notifyProgress(uint position, uint duration)
{
    logDebug("%s::%s MW ====> HMI, position = %d, duration = %d\n", TAG.data(), __FUNCTION__,
             position, duration);
    if (mMediaplayerInterface)
    {
        mediaplayer_emit_notify_progress(mMediaplayerInterface, position, duration);
    }
}

void DBusCallbacks::notifyError(std::string messge)
{
    logInfo("%s::%s MW ====> HMI, messge = %s\n", TAG.data(), __FUNCTION__,
            messge.data());
    if (mMediaplayerInterface)
    {
        mediaplayer_emit_notify_error(mMediaplayerInterface, messge.data());
    }
}

void DBusCallbacks::notifyEOS()
{
    logInfo("%s::%s MW ====> HMI\n", TAG.data(), __FUNCTION__);
    if (mMediaplayerInterface)
    {
        mediaplayer_emit_notify_eos(mMediaplayerInterface);
    }
}

void DBusCallbacks::runmainloop()
{
    if (mLoop != nullptr)
    {
        logInfo("%s::%s run.\n", TAG.data(), __FUNCTION__);
        g_main_loop_run(mLoop);
    }
    else
    {
        logError("%s::%s Invalid GDBUS Loop pointer!\n", TAG.data(), __FUNCTION__);
    }
}

void DBusCallbacks::registerMediaplayerServer()
{
    guint ownerID = g_bus_own_name(G_BUS_TYPE_SESSION,
                                   MEDIAPLAYER_DBUS_NAME,
                                   G_BUS_NAME_OWNER_FLAGS_NONE,
                                   onMediaplayerBusAcquired,
                                   NULL,
                                   NULL,
                                   this,
                                   NULL);
    if (0 != ownerID)
    {
        logInfo("%s::%s MediaPlayerServer registered on DBus as %i \n", TAG.data(), __FUNCTION__, ownerID);
    }
}

void DBusCallbacks::registerMediainfoServer()
{
    guint ownerID = g_bus_own_name(G_BUS_TYPE_SESSION,
                                   MEDIAINFO_DBUS_NAME,
                                   G_BUS_NAME_OWNER_FLAGS_NONE,
                                   onMediainfoBusAcquired,
                                   NULL,
                                   NULL,
                                   this,
                                   NULL);
    if (0 != ownerID)
    {
        logInfo("%s::%s MediaInfoServer registered on DBus as %i \n", TAG.data(), __FUNCTION__, ownerID);
    }
}

gboolean
DBusCallbacks::handleStart(Mediaplayer *proxy, GDBusMethodInvocation *invocation, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW\n", TAG.data(), __FUNCTION__);
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);
    bool ret = false;
    if (pData && pData->mMediaPlayer)
    {
        ret = pData->mMediaPlayer->start();
    }
    else
    {
        logError("%s::%s pData or mMediaPlayer is null\n", TAG.data(), __FUNCTION__);
    }

    mediaplayer_complete_start(proxy, invocation, ret);
    return TRUE;
}

gboolean
DBusCallbacks::handleStop(Mediaplayer *proxy, GDBusMethodInvocation *invocation, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW\n", TAG.data(), __FUNCTION__);
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);
    bool ret = false;
    if (pData && pData->mMediaPlayer)
    {
        ret = pData->mMediaPlayer->stop();
    }
    else
    {
        logError("%s::%s pData or mMediaPlayer is null\n", TAG.data(), __FUNCTION__);
    }
    mediaplayer_complete_stop(proxy, invocation, ret);
    return TRUE;
}

gboolean
DBusCallbacks::handlePause(Mediaplayer *proxy, GDBusMethodInvocation *invocation, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW\n", TAG.data(), __FUNCTION__);
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);
    bool ret = false;
    if (pData && pData->mMediaPlayer)
    {
        ret = pData->mMediaPlayer->pause();
    }
    else
    {
        logError("%s::%s pData or mMediaPlayer is null\n", TAG.data(), __FUNCTION__);
    }
    mediaplayer_complete_pause(proxy, invocation, ret);
    return TRUE;
}

gboolean DBusCallbacks::handleSeek(Mediaplayer *proxy, GDBusMethodInvocation *invocation, const guint secs, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW secs:%u\n", TAG.data(), __FUNCTION__, secs);
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);
    bool ret = false;
    if (pData && pData->mMediaPlayer)
    {
        ret = pData->mMediaPlayer->seek(secs);
    }
    else
    {
        logError("%s::%s pData or mMediaPlayer is null\n", TAG.data(), __FUNCTION__);
    }
    mediaplayer_complete_seek(proxy, invocation, ret);
    return TRUE;
}

gboolean DBusCallbacks::handleSetRate(Mediaplayer *proxy, GDBusMethodInvocation *invocation, const gdouble rate, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW rate:%lf\n", TAG.data(), __FUNCTION__, rate);
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);
    bool ret = false;
    if (pData && pData->mMediaPlayer)
    {
        ret = pData->mMediaPlayer->setRate(rate);
    }
    else
    {
        logError("%s::%s pData or mMediaPlayer is null\n", TAG.data(), __FUNCTION__);
    }
    mediaplayer_complete_set_rate(proxy, invocation, ret);
    return TRUE;
}

gboolean DBusCallbacks::handleSetFile(Mediaplayer *proxy, GDBusMethodInvocation *invocation, const gchar *filepath, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW filename:%s\n", TAG.data(), __FUNCTION__, filepath);
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);
    bool ret = false;
    if (pData && pData->mMediaPlayer)
    {
        ret = pData->mMediaPlayer->setFile(filepath);
    }
    else
    {
        logError("%s::%s pData or mMediaPlayer is null\n", TAG.data(), __FUNCTION__);
    }
    mediaplayer_complete_set_file(proxy, invocation, ret);
    return TRUE;
}

gboolean DBusCallbacks::handleGetTags(Mediainfo *proxy, GDBusMethodInvocation *invocation, const gchar *filepath, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW filepath:%s\n", TAG.data(), __FUNCTION__, filepath);
    std::shared_ptr<IMediaInfo> pMediaInfo = std::make_shared<MediaInfo>();
    std::string title = pMediaInfo->getTitle(filepath);
    std::string artist = pMediaInfo->getArtist(filepath);
    std::string album = pMediaInfo->getAlbum(filepath);

    mediainfo_complete_get_tags(proxy, invocation, title.data(), artist.data(), album.data());
}

gboolean DBusCallbacks::handleGetImage(Mediainfo *proxy, GDBusMethodInvocation *invocation, const gchar *filepath, const gchar *imagepath, gpointer user_data)
{
    logInfo("%s::%s HMI ====> MW filepath:%s imagepath:%s\n", TAG.data(), __FUNCTION__, filepath, imagepath);
    std::shared_ptr<IMediaInfo> pMediaInfo = std::make_shared<MediaInfo>();
    pMediaInfo->getImage(filepath,imagepath);

    mediainfo_complete_get_image(proxy, invocation);
}

gboolean DBusCallbacks::updateProgress(gpointer user_data)
{
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);
    if (pData && pData->mMediaPlayer)
    {
        int64_t position = pData->mMediaPlayer->getPosition();
        int64_t duration = pData->mMediaPlayer->getDuration();
        if (position != -1 && duration != -1)
            pData->notifyProgress(position, duration);
    }
    else
    {
        logError("%s::%s pData or mMediaPlayer is null\n", TAG.data(), __FUNCTION__);
        return FALSE;
    }
    return TRUE;
}

void DBusCallbacks::onMediaplayerBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    if (user_data == NULL)
        return;
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);

    pData->mMediaplayerInterface = mediaplayer_skeleton_new();
    logInfo("%s::%s MediaPlayerServer dbus acquired: %s %p\n", TAG.data(), __FUNCTION__, name, pData->mMediaplayerInterface);
    g_signal_connect(pData->mMediaplayerInterface,
                     "handle-start",
                     G_CALLBACK(handleStart),
                     user_data);
    g_signal_connect(pData->mMediaplayerInterface,
                     "handle-stop",
                     G_CALLBACK(handleStop),
                     user_data);
    g_signal_connect(pData->mMediaplayerInterface,
                     "handle-pause",
                     G_CALLBACK(handlePause),
                     user_data);
    g_signal_connect(pData->mMediaplayerInterface,
                     "handle-seek",
                     G_CALLBACK(handleSeek),
                     user_data);
    g_signal_connect(pData->mMediaplayerInterface,
                     "handle-set-rate",
                     G_CALLBACK(handleSetRate),
                     user_data);
    g_signal_connect(pData->mMediaplayerInterface,
                     "handle-set-file",
                     G_CALLBACK(handleSetFile),
                     user_data);

    GError **error = NULL;
    if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(pData->mMediaplayerInterface),
                                          connection,
                                          MEDIAPLAYER_DBUS_OBJECT_PATH,
                                          error))
    {
        logError("%s::%s MediaPlayerServer export skeleton failed:%u  %d  %s\n", TAG.data(), __FUNCTION__, (*error)->domain, (*error)->code, (*error)->message);
    }
    std::unique_lock<std::mutex> lck(pData->mtx);
    pData->cv.notify_one();
    logInfo("%s::%s MediaPlayerServer dbus acquired end\n", TAG.data(), __FUNCTION__);
}

void DBusCallbacks::onMediainfoBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    if (user_data == NULL)
        return;
    DBusCallbacks *pData = static_cast<DBusCallbacks *>(user_data);

    pData->mMediainfoInterface = mediainfo_skeleton_new();
    logInfo("%s::%s MediaInfoServer dbus acquired: %s %p\n", TAG.data(), __FUNCTION__, name, pData->mMediainfoInterface);

    g_signal_connect(pData->mMediainfoInterface,
                     "handle-get-tags",
                     G_CALLBACK(handleGetTags),
                     user_data);
    g_signal_connect(pData->mMediainfoInterface,
                     "handle-get-image",
                     G_CALLBACK(handleGetImage),
                     user_data);

    GError **error = NULL;
    if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(pData->mMediainfoInterface),
                                          connection,
                                          MEDIAINFO_DBUS_OBJECT_PATH,
                                          error))
    {
        logError("%s::%s MediaInfoServer export skeleton failed:%u  %d  %s\n", TAG.data(), __FUNCTION__, (*error)->domain, (*error)->code, (*error)->message);
    }
    logInfo("%s::%s MediaInfoServer dbus acquired end\n", TAG.data(), __FUNCTION__);
}
