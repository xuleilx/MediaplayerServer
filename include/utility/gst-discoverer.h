#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        char title[1024];
        char artist[1024];
        char album[1024];
    } MediaInfo_s;

    int getTags(const char *inFilePath,const char *outImagePath, MediaInfo_s *mediainfo);
#ifdef __cplusplus
}
#endif