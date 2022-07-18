#include <LittleFS.h>

const String path = "/config.json";

void enableConfig()
{
    LittleFS.begin();
}
bool haveConfig()
{
    return LittleFS.exists(path);
}
String readConfig()
{
    File file = LittleFS.open(path, "r");
    String content = "";
    if (file.available())
    {
        content = file.readString();
        file.close();
    }
    return content;
}

void writeConfig(String content)
{
    File file = LittleFS.open(path, "w+");
    file.print(content);
    file.close();
}
