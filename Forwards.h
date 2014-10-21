#ifndef FORWARDS_H_INCLUDED
#define FORWARDS_H_INCLUDED

class Org;
typedef Org *OrgPtr;
class PopBase {
public:
  virtual OrgPtr GetRandomOrg() {return NULL;}
};

#endif // FORWARDS_H_INCLUDED
