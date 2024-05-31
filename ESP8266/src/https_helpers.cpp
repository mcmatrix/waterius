#include <ESP8266WiFi.h>
#include "ESP8266HTTPClient.h"
#include "Logging.h"
#include "utils.h"

bool post_data(const String &url, const char *key, const char *email, const String &payload)
{
    void *pClient = nullptr;
    HTTPClient httpClient;
    bool result = false;
    LOG_INFO(F("HTTP: Send JSON POST request"));
    LOG_INFO(F("HTTP: URL:") << url);
    LOG_INFO(F("HTTP: Body:") << payload);

    String proto = get_proto(url);
    LOG_INFO(F("HTTP: Protocol: ") << proto);

    // Set wc client
    if (proto == PROTO_HTTP)
    {
        LOG_INFO(F("HTTP: Create insecure client"));
        pClient = new WiFiClient;
    }
    else if (proto == PROTO_HTTPS)
    {
        LOG_INFO(F("HTTP: Create secure client"));
        pClient = new WiFiClientSecure;
        (*(WiFiClientSecure *)pClient).setInsecure(); // доверяем всем сертификатам
    }

    // HTTP settings
    httpClient.setTimeout(SERVER_TIMEOUT);
    httpClient.setReuse(false); // будет сразу закрывать подключение после отправки

    if (httpClient.begin(*(WiFiClient *)pClient, url))
    {
        httpClient.addHeader(F("Content-Type"), F("application/json"));
        if (key)
        {
            httpClient.addHeader(F("Waterius-Token"), key);
        }
        if (email)
        {
            httpClient.addHeader(F("Waterius-Email"), email);
        }
        LOG_INFO(F("HTTP: Post request"));

        int response_code = httpClient.POST(payload);
        LOG_INFO(F("HTTP: Response code: ") << response_code);
        result = response_code == 200;
        String response_body = httpClient.getString();
        LOG_INFO(F("HTTP: Response body: ") << response_body);
        httpClient.end();
        (*(WiFiClient *)pClient).stop();
    }

    if (proto == PROTO_HTTP)
    {
        delete (WiFiClient *)pClient;
    }
    else if (proto == PROTO_HTTPS)
    {
        delete (WiFiClientSecure *)pClient;
    }

    return result;
}
