#ifndef __IMEDIAINFO_H__
#define __IMEDIAINFO_H__

#include <iostream>

class IMediaInfo
{
public:
    virtual ~IMediaInfo() {}
    /**
     * get title
     * @param inFilePath music file path
     * @return title, otherwise null_ptr.
     */
    virtual std::string getTitle(std::string inFilePath) = 0;

    /**
     * get artist
     * @param inFilePath music file path
     * @return artist, otherwise null_ptr.
     */
    virtual std::string getArtist(std::string inFilePath) = 0;

    /**
     * get album
     * @param inFilePath music file path
     * @return album, otherwise null_ptr.
     */
    virtual std::string getAlbum(std::string inFilePath) = 0;

    /**
     * get image
     * This will get the id3 image of the inFilePath file, and put the image at inImagePath.
     * @param inFilePath music file path
     * @param inImagePath image file path
     * @return true if the call succeeded, an error code otherwise.
     */
    virtual bool getImage(std::string inFilePath, std::string inImagePath) = 0;
};

#endif // __IMEDIAINFO_H__