#ifndef LUGAR_H_INCLUDED
#define LUGAR_H_INCLUDED

#include "Org.h"

/* ********************************************************************** */
class Lugar;
typedef Lugar *LugarPtr;
typedef std::vector<LugarPtr> LugarVec;
class Lugar {
public:
  Org *tenant;
  Org *tenant_next;
  uint32_t numnbrs;
  LugarPtr *nbrs;// array of nbrs for whatever topology
  LugarVec nbrsv;
  /* ********************************************************************** */
  Lugar() {
    this->numnbrs = 0;
    this->nbrs = allocsafe(LugarPtr, this->numnbrs);
    this->tenant = NULL;
    this->tenant_next = NULL;
  }
  /* ********************************************************************** */
  ~Lugar() {
    nbrsv.clear();
    this->numnbrs = 0;
    freesafe(this->nbrs);
    delete this->tenant;
    delete this->tenant_next;
  }
  /* ********************************************************************** */
  void Attach_Nbr(LugarPtr nbr0) {
    this->nbrsv.push_back(nbr0);
    nbr0->nbrsv.push_back(this);
  }
  /* ********************************************************************** */
  void Attach_Tenant(Org *tenant0) {
    this->tenant = tenant0;
    tenant->home = this;
  }
  /* ********************************************************************** */
  void Attach_Next_Tenant(Org *tenant0) {
    this->tenant_next = tenant0;
    tenant_next->home = this;
  }
  /* ********************************************************************** */
  void Rollover_Tenant() {
    delete this->tenant;
    this->tenant = tenant_next;
    this->tenant_next = NULL;
  }
};

#endif // LUGAR_H_INCLUDED
