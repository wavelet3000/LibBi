/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_HOST_UPDATER_STATICMAXLOGDENSITYMATRIXVISITORHOST_HPP
#define BI_HOST_UPDATER_STATICMAXLOGDENSITYMATRIXVISITORHOST_HPP

namespace bi {
/**
 * Visitor for StaticMaxLogDensityHost.
 */
template<class B, class S, class PX, class OX>
class StaticMaxLogDensityMatrixVisitorHost {
public:
  template<class T1>
  static void accept(State<B,ON_HOST>& s, const int p, const PX& pax, OX& x,
      T1& lp);
};

/**
 * @internal
 *
 * Base case of StaticMaxLogDensityMatrixVisitorHost.
 */
template<class B, class PX, class OX>
class StaticMaxLogDensityMatrixVisitorHost<B,empty_typelist,PX,OX> {
public:
  template<class T1>
  static void accept(State<B,ON_HOST>& s, const int p, const PX& pax, OX& x,
      T1& lp) {
    //
  }
};
}

#include "../../typelist/front.hpp"
#include "../../typelist/pop_front.hpp"
#include "../../traits/target_traits.hpp"

template<class B, class S, class PX, class OX>
template<class T1>
void bi::StaticMaxLogDensityMatrixVisitorHost<B,S,PX,OX>::accept(
    State<B,ON_HOST>& s, const int p, const PX& pax, OX& x, T1& lp) {
  typedef typename front<S>::type front;
  typedef typename pop_front<S>::type pop_front;

  front::maxLogDensities(s, p, pax, x, lp);
  StaticMaxLogDensityMatrixVisitorHost<B,pop_front,PX,OX>::accept(s, p, pax,
      x, lp);
}

#endif
