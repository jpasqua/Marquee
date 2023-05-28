

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

char *makeAHole(size_t holeSize) {
  // The following call to malloc is actually there to *reduce* fragmentation! What happens normally
  // is we allocate a huge chunk for the the JSON document, read it in, and then allocate the various
  // FlexScreen objects. They are allocated "later" on the heap than the JSON document, so when we free
  // it, there is a big hole left over (hence the fragmentation). By allocating the placeholder first,
  // then allocating the JSON doc, then freeing the placeholder, we leave space "before" the JSON doc
  // for the FlexScreen items to consume. Then when the JSON doc is freed, it doesnt leave a hole since
  // it is freed from the end of the heap.
  char *placeHolder = nullptr;

  if (GenericESP::getMaxFreeBlockSize() > holeSize) {
    placeHolder = (char*)malloc(holeSize);
    placeHolder[1] = 'C';   // Touch the memory or the compiler may optimize away the malloc
  }
  return placeHolder;
}

bool NewsClient::update() {
  if (_source.isEmpty() || _key.isEmpty()) return false;

  StaticJsonDocument<128> filter;
  filter["articles"][0]["title"] = true;
  // filter["articles"][0]["description"] = true;
  // filter["articles"][0]["url"] = true;

  char* hole = makeAHole(1024);
  DynamicJsonDocument *doc = newsService.issueGET(_endpoint, 2048, &filter);
  if (hole) free(hole);

  if (!doc) {
    Log.warning(F("Failed to get news info"));
    return false;
  }
  // serializeJsonPretty(*doc, Serial); Serial.println();

  stories.clear();
  int index = 0;
  JsonArrayConst articles = (*doc)["articles"].as<JsonArrayConst>();
  for (JsonObjectConst article : articles) {
    Story s;
    s.title = article["title"].as<String>();
    // s.description = article["description"].as<String>();
    // s.url = article["url"].as<String>();
    stories.push_back(s);
    if (++index == MaxStories) break;
  }

  delete doc;
  return true;
}

void NewsClient::dump() {
  for (Story story : stories) {
    Log.verbose("Title: %s", story.title.c_str());
    // Log.verbose("Description: %s", story.description.c_str());
    // Log.verbose("URL: %s", story.url.c_str());
  }
}







