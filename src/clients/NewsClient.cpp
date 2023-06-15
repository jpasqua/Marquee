

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//#include <ESP8266WiFi.h>
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <JSONService.h>
#include <GenericESP.h>
//                                  WebThing Includes
//                                  Local Includes
#include "NewsClient.h"
//--------------- End:    Includes ---------------------------------------------

static constexpr char NewsServer[] = "newsapi.org";
static constexpr uint16_t NewsPort = 80;

static JSONService newsService(ServiceDetails(NewsServer, NewsPort));

NewsClient::NewsClient(String& source, String& key) {
  updateSettings(source, key);
  _endpoint.reserve(132);
}

void NewsClient::updateSettings(String& source, String& key) {
  _source = source;
  _key = key;
  _endpoint ="/v2/top-headlines?sources=";
  _endpoint.concat(_source);
  _endpoint.concat("&apiKey=");
  _endpoint.concat(_key);
}

bool NewsClient::update() {
  if (_source.isEmpty() || _key.isEmpty()) return false;

  StaticJsonDocument<128> filter;
  filter["title"] = true;

  WiFiClient* newsResponse = newsService.initiateRequest(_endpoint.c_str(), GET, "");
  if (!newsResponse) {
    Log.warning(F("Failed to get news info"));
    return false;
  }

  newsResponse->find("\"articles\":[");  // Position the stream at beginning of array

  nStories = 0;
  StaticJsonDocument<256> doc;
  do {
    auto error = deserializeJson(doc, *newsResponse, DeserializationOption::Filter(filter));
    if (error) {
      Log.warning(F("Error (%s) while parsing news feed"), error.c_str());
      newsResponse->stop();
      delete newsResponse;
      // TO DO: Any other cleanup needed?
      return false;
    }
    stories[nStories++] = doc["title"].as<String>();
  } while ((nStories < MaxStories) && (newsResponse->findUntil(",", "]")));

int totalLength = 0;
for (int i = 0; i < nStories; i++) {totalLength+=stories[i].length();}
Log.verbose("Read %d news stories with a total of %d chars", nStories, totalLength);

  newsResponse->stop();
  delete newsResponse;
  return true;
}

void NewsClient::dump() {
  for (int i = 0; i < nStories; i++) {
    Log.verbose("Title: %s", stories[i].c_str());
  }
}







