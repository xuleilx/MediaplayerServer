#ifndef __MEDIAINFO_H__
#define __MEDIAINFO_H__
#include <iostream>
#include "IMediaInfo.h"
#include "gst-discoverer.h"

class MediaInfo : public IMediaInfo
{
public:
    MediaInfo();
    ~MediaInfo();

    std::string getTitle(std::string inFilePath);
    std::string getArtist(std::string inFilePath);
    std::string getAlbum(std::string inFilePath);
    bool getImage(std::string inFilePath, std::string inImagePath);

private:
    bool move(const std::string &src, const std::string &dst) const;

private:
    MediaInfo_s mMediaInfo;
    std::string mTempImg;
    std::string mFilePath;
};

#endif // __MEDIAINFO_H__