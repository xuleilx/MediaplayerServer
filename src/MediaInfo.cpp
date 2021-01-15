#include "MediaInfo.h"
#include <string.h>
#include <fstream>
#include "Log.h"

static const std::string TAG = "MediaPlayer";

MediaInfo::MediaInfo()
{
    memset(&mMediaInfo, 0, sizeof(mMediaInfo));
    mTempImg="/tmp/tmp.jpg";
}

MediaInfo::~MediaInfo()
{
}

std::string MediaInfo::getTitle(std::string inFilePath)
{
    if (mFilePath.compare(inFilePath))
    {
        mFilePath = inFilePath;
        getTags(mFilePath.data(), mTempImg.data(), &mMediaInfo);
    }

    return mMediaInfo.title;
}

std::string MediaInfo::getArtist(std::string inFilePath)
{
    if (mFilePath.compare(inFilePath))
    {
        mFilePath = inFilePath;
        getTags(mFilePath.data(), mTempImg.data(), &mMediaInfo);
    }
    return mMediaInfo.artist;
}

std::string MediaInfo::getAlbum(std::string inFilePath)
{
    if (mFilePath.compare(inFilePath))
    {
        mFilePath = inFilePath;
        getTags(mFilePath.data(), mTempImg.data(), &mMediaInfo);
    }
    return mMediaInfo.album;
}

bool MediaInfo::getImage(std::string inFilePath, std::string inImagePath)
{
    if (mFilePath.compare(inFilePath))
    {
        mFilePath = inFilePath;
        getTags(mFilePath.data(), mTempImg.data(), &mMediaInfo);
    }
    return move(mTempImg.data(), inImagePath.data());
}

bool MediaInfo::move(const std::string &src, const std::string &dst) const
{
    if (src.compare(dst) == 0)
        return true;

    std::ifstream ifs(src, std::ios::binary);
    std::ofstream ofs(dst, std::ios::binary);

    if (!ifs.is_open())
    {
        logError("%s::%s Can not open %s\n",TAG.data(),__FUNCTION__,src.data());
        return false;
    }
    if (!ofs.is_open())
    {
        logError("%s::%s Can not open %s\n",TAG.data(),__FUNCTION__,dst.data());
        return false;
    }
    ofs << ifs.rdbuf();
    ifs.close();
    ofs.close();

    if (0 != remove(src.c_str()))
    {
        logError("%s::%s Can not remove %s\n",TAG.data(),__FUNCTION__,src.data());
    }
    return true;
}