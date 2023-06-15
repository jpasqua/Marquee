#ifndef NewsClient_h
#define NewsClient_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------


class NewsClient {
public:
  static constexpr uint8_t MaxStories = 10;

  NewsClient(String& source, String& key);
  void updateSettings(String& source, String& key);
  bool update();
  void dump();

  String stories[MaxStories];
  int nStories = 0;

private:
  String _endpoint;
  String _source;
  String _key;
};



#endif  // NewsClient_h