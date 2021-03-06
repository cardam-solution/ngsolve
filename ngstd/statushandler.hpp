#ifndef _STATUSHANDLER_HPP
#define _STATUSHANDLER_HPP

namespace ngstd
{

  /** Access to statusbar. (and more)
   */

  class NGS_DLL_HEADER BaseStatusHandler
  {
  public:
    virtual ~BaseStatusHandler () { ; }
    virtual void PushStatus (const char * str) const = 0;

    virtual void PopStatus () const = 0;

    virtual void SetThreadPercentage (double percent) const = 0;

    virtual void GetStatus (string & str, double & percent) const = 0;

    virtual void SetTerminate(void) const = 0;
    virtual void UnSetTerminate(void) const = 0;
    virtual bool ShouldTerminate(void) const = 0;
  
  };

}

#endif // _STATUSHANDLER_HPP
