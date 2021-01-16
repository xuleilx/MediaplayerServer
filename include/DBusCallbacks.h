#ifndef MEDIAPLAYERCALLBACKS_H
#define MEDIAPLAYERCALLBACKS_H
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <future>             // std::async, std::future
#include "IMediaplayerCallbacks.h"
#include "IMediaPlayer.h"
#include "GDBusMediaplayerServer.h"

class DBusCallbacks : public IMediaplayerCallbacks
{
public:
    DBusCallbacks();
    ~DBusCallbacks();

    void init();

    void notifyProgress(uint position, uint duration);
    void notifyError(std::string messge);
    void notifyEOS();

private:
    void runmainloop();
    void registerMediaplayerServer();
    void registerMediainfoServer();

    static gboolean handleStart(Mediaplayer *proxy, GDBusMethodInvocation *invocation, gpointer user_data);
    static gboolean handleStop(Mediaplayer *proxy, GDBusMethodInvocation *invocation, gpointer user_data);
    static gboolean handlePause(Mediaplayer *proxy, GDBusMethodInvocation *invocation, gpointer user_data);
    static gboolean handleSeek(Mediaplayer *proxy, GDBusMethodInvocation *invocation, const guint secs, gpointer user_data);
    static gboolean handleSetRate(Mediaplayer *proxy, GDBusMethodInvocation *invocation, const gdouble rate, gpointer user_data);
    static gboolean handleSetFile(Mediaplayer *proxy, GDBusMethodInvocation *invocation, const gchar *filepath, gpointer user_data);

    static gboolean handleGetTags(Mediainfo *proxy, GDBusMethodInvocation *invocation, const gchar *filepath, gpointer user_data);
    static gboolean handleGetImage(Mediainfo *proxy, GDBusMethodInvocation *invocation, const gchar *filepath, const gchar *imagepath, gpointer user_data);

    static gboolean updateProgress(gpointer user_data);

    static void onMediaplayerBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);
    static void onMediainfoBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

private:
    GMainLoop *mLoop;
    std::future<void> mLoopThread;
    Mediaplayer *mMediaplayerInterface;
    Mediainfo *mMediainfoInterface;

    std::mutex mtx;
    std::condition_variable cv;
};

#endif // MEDIAPLAYERCALLBACKS_H
