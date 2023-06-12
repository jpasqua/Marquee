#ifndef DateScreen_h
#define DateScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
#include <TimeLib.h>
//                                  Local Includes
#include <screens/matrix/ScrollScreen.h>
//--------------- End:    Includes ---------------------------------------------


class DateScreen : public ScrollScreen {
public:
  DateScreen();
  virtual void innerActivation() override;



private:
  String dateOrdinal(int dom);
};

#endif  // DateScreen_h
