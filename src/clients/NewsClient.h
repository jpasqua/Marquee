#ifndef NewsClient_h
#define NewsClient_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <vector>
//                                  Third Party Libraries
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------


class NewsClient {
public:
  static constexpr uint8_t MaxStories = 10;

  struct Story {
    String title;
    // String description;
    // String url;
  };

  NewsClient(String& source, String& key);
  bool update();
  void dump();

  std::vector<Story> stories;

private:
  String _endpoint;
  String _source;
  String _key;
};



#endif  // NewsClient_h