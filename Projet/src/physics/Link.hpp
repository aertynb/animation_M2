#pragma once
#include "PMat.hpp"
#include <memory>

class Link
{
public:
  std::function<void()> _update;

  enum Type { HOOK, DAMPER, DAMPED_HOOK, COND_DAMPED_HOOK };

  // constructeurs

  Link(Type type, std::shared_ptr<PMat> M1, std::shared_ptr<PMat> M2, float &k,
      float &z, float &s);

  // moteurs

  void update_Hook();
  void update_Damper();
  void update_Damped_Hook();
  void update_Cond_Damped_Hook();

  // update the pmats in the link
  void update(float h)
  {
    _update();
    _M1->_update(h);
    _M2->_update(h);
  }

  void setType(Type type) {
      switch(type) {
        case Type::HOOK : {
            _update = [this]() {update_Hook();};
            break;
        }
        case Type::DAMPER : {
            _update = [this]() {update_Damper();};
            break;
        }
        case Type::DAMPED_HOOK : {
            _update = [this]() {update_Damped_Hook();};
            break;
        }
        case Type::COND_DAMPED_HOOK : {
            _update = [this]() {update_Cond_Damped_Hook();};
            break;
        }
    }
  }

private:
  float &_k, &_z, &_s, _l0;
  std::shared_ptr<PMat> _M1, _M2;
};