#ifndef IOJACK_H_INCLUDED
#define IOJACK_H_INCLUDED

#include "Base.h"

typedef uint64_t IoDexType; // #define IoDexType uint64_t

/* ********************************************************************** */
typedef class IoJackBase *IoJackBasePtr;
class IoJackBase {
public:
  IoDexType PortId;
  uint32_t refcnt;
  double Value;
  bool Deprecated;
  IoJackBase() {
    this->refcnt=0;
    this->Value=0;
    this->Deprecated = false;
  }
  IoJackBasePtr AddRef() {
    this->refcnt++;
    return this;
  }
  uint32_t UnRef(IoJackBasePtr *meref) {
    this->refcnt--;
    return this->refcnt;
  }
//  IoJackBasePtr AddRef(IoJackBasePtr *meref){
//    (*meref) = this;
//    this->refcnt++;
//    return this;
//  }
//  uint32_t UnRef(IoJackBasePtr *meref){
//    this->refcnt--;
//    (*meref) = NULL;
//    return this->refcnt;
//  }
};

/* ********************************************************************** */
typedef class IoJack *IoJackPtr;
typedef std::vector<IoJackPtr> IoJackVec;
class IoJack : public IoJackBase {
public:
  IoJackBasePtr UpStream;
  IoJack() {
    this->refcnt=0;
    this->Value=0;
    this->UpStream=NULL;
  }
};

/* ********************************************************************** */
typedef class GlobalIoJack *GlobalIoJackPtr;
class GlobalIoJack : public IoJackBase {
public:
  GlobalIoJack() {
    this->refcnt=0;
    this->Value=0;
  }
};

#endif // IOJACK_H_INCLUDED
