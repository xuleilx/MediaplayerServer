#include "MyElement.h"
#include <string.h>
#include "INIReader.h"
#include "Log.h"

static const std::string TAG = "MyElement";

MyElement::MyElement(std::string name) : mName(name), mElement(nullptr)
{
}

MyElement::~MyElement()
{
}

bool MyElement::setup(std::shared_ptr<INIReader> reader)
{
    mElement = gst_element_factory_make(mName.data(), NULL);
    if (!mElement)
    {
        logError("%s::%s %s Not all elements could be created.\n", TAG.data(), __FUNCTION__, mName.data());
        return false;
    }

    mpINIReader = reader;
    if (mpINIReader && mpINIReader->ParseError() == 0)
    {
        // get valid ElementProperty
        mValidPropertyMap = getElementPropertyInfo(mName.data());
        setProperty();
    }
    else
    {
        logWarn("%s::%s config file not available, we will use default.\n", TAG.data(), __FUNCTION__);
        if (mpINIReader)
            logWarn("%s::%s line number of first error on parse error:%d\n", TAG.data(), __FUNCTION__, mpINIReader->ParseError());
    }
    return true;
}

bool MyElement::teardown()
{
    if (!mElement)
    {
        gst_object_unref(mElement);
    }
    return true;
}

int MyElement::setValue(std::string property, GType type)
{
    logInfo("%s::%s %s\t%s\t%s\n", TAG.data(), __FUNCTION__,
            property.data(), g_type_name(type),
            mpINIReader->GetString(mName.data(), property.data(), "").data());

    switch (type)
    {
    case G_TYPE_STRING:
    {
        g_object_set(G_OBJECT(mElement),
                     property.data(),
                     mpINIReader->GetString(mName.data(), property.data(), "").data(),
                     NULL);
        break;
    }
    case G_TYPE_BOOLEAN:
    {
        g_object_set(G_OBJECT(mElement),
                     property.data(),
                     mpINIReader->GetBoolean(mName.data(), property.data(), false),
                     NULL);
        break;
    }
    case G_TYPE_ULONG:
    case G_TYPE_LONG:
    case G_TYPE_UINT:
    case G_TYPE_INT:
    case G_TYPE_UINT64:
    case G_TYPE_INT64:
    {
        g_object_set(G_OBJECT(mElement),
                     property.data(),
                     mpINIReader->GetInteger(mName.data(), property.data(), 0),
                     NULL);
        break;
    }
    case G_TYPE_FLOAT:
    case G_TYPE_DOUBLE:
    {
        g_object_set(G_OBJECT(mElement),
                     property.data(),
                     mpINIReader->GetReal(mName.data(), property.data(), 0.0),
                     NULL);
        break;
    }
    case G_TYPE_CHAR:
    case G_TYPE_UCHAR:
    {
        break;
    }
    case G_TYPE_OBJECT:
    {
        break;
    }
        /* fall through */
    default:
    {
        if (strcmp(g_type_name(type), "GstElement") == 0)
        {
            // We have test, when we free bin ,the GstElements that the bin contains will also be freed.
            std::shared_ptr<MyElement> pMyElement = std::make_shared<MyElement>(mpINIReader->GetString(mName.data(), property.data(), ""));
            pMyElement->setup(mpINIReader);
            g_object_set(G_OBJECT(mElement), property.data(), pMyElement->element(), NULL);
        }
        else if (strcmp(g_type_name(type), "GstPlayFlags") == 0)
        {
            g_object_set(G_OBJECT(mElement),
                         property.data(),
                         mpINIReader->GetInteger(mName.data(), property.data(), 0),
                         NULL);
        }
        break;
    }
    }
}

int MyElement::setProperty()
{
    if (mValidPropertyMap.empty())
    {
        logWarn("%s::%s ValidPropertyMap is empty\n", TAG.data(), __FUNCTION__);
        return -1;
    }
    for (std::map<std::string, GType>::iterator itM = mValidPropertyMap.begin();
         itM != mValidPropertyMap.end();
         ++itM)
    {
        if (mpINIReader->HasValue(mName.data(), (itM->first).data()))
        {
            setValue(itM->first, itM->second);
        }
    }
    return 0;
}

GstElement *MyElement::element()
{
    return mElement;
}

std::map<std::string, GType> MyElement::getElementPropertyInfo(std::string elementName)
{
    std::map<std::string, GType> propertyMap;
    GstElementFactory *factory;
    GstElement *element;

    gst_init(NULL, NULL);
    // get
    factory = gst_element_factory_find(elementName.data());
    if (factory == NULL)
    {
        logError("%s::%s Get Factory Failed\n", TAG.data(), __FUNCTION__);
        goto exit;
    }
    factory =
        GST_ELEMENT_FACTORY(gst_plugin_feature_load(GST_PLUGIN_FEATURE(factory)));
    element = gst_element_factory_create(factory, NULL);

    GParamSpec **property_specs;
    guint num_properties, i;

    property_specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(element), &num_properties);
    for (i = 0; i < num_properties; i++)
    {
        GValue value = {
            0,
        };
        GParamSpec *param = property_specs[i];
        g_value_init(&value, param->value_type);

        // std::cout << g_param_spec_get_name(param) << '\t' << g_type_name(param->value_type) << std::endl;
        propertyMap.insert(std::pair<std::string, GType>(g_param_spec_get_name(param), param->value_type));
    }
    if (num_properties == 0)
        logWarn("%s::%s  none\n", TAG.data(), __FUNCTION__);

    g_free(property_specs);
    gst_object_unref(element);
    gst_object_unref(factory);
exit:
    return propertyMap;
}
