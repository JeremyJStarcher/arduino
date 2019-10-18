#ifndef IO_LINE_H
#define IO_LINE_H
#include <stdint.h>

class IoLine {
  public:
    virtual int readbyte() = 0;

    //    virtual void f() = 0; // pure virtual
    //    virtual void g() {}  // non-pure virtual
    //    ~Abstract() {
    //        g(); // OK: calls Abstract::g()
    //        // f(); // undefined behavior
    //        Abstract::f(); // OK: non-virtual call
    //    }
};

#endif
