#include <iostream>
#include "MediaPlayer.h"
#include "DBusCallbacks.h"
#include <unistd.h>
#include "Log.h"

int main(int argc, char *argv[])
{
    logInit();

    logDebug("==========================\n");
    logInfo ("         MediaPlayer      \n");
    logWarn ("     %s %s\n", __DATE__, __TIME__);
    logError("==========================\n");

    /* init dbus for communicate with hmi */
    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    std::shared_ptr<DBusCallbacks> pDbusCallbacks = std::make_shared<DBusCallbacks>();
    pDbusCallbacks->init();

    std::shared_ptr<IMediaPlayer> pPlayer = std::make_shared<MediaPlayer>(main_loop, pDbusCallbacks);

    while (1)
    {
        pPlayer->setup();
        g_main_loop_run(main_loop);

        /* Free resources */
        pPlayer->stop();
        pPlayer->teardown();
    }
}
