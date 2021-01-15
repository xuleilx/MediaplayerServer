#ifndef __PLAYBIN_H__
#define __PLAYBIN_H__
#include <iostream>
#include <map>
#include <memory>

#include <gst/gst.h>

class AudioSink;
class VideoSink;
class INIReader;

class MyElement
{
public:
    MyElement(std::string name);
    ~MyElement();

    bool setup(std::shared_ptr<INIReader> reader);
    bool teardown();
    GstElement *element();

private:
    int setValue(std::string name, GType type);
    int setProperty();
    std::map<std::string,GType> getElementPropertyInfo (std::string elementName);

private:
    GstElement *mElement;
    std::string mName;
    std::shared_ptr<INIReader> mpINIReader;
    std::map<std::string, GType> mValidPropertyMap;
};
#endif // __PLAYBIN_H__