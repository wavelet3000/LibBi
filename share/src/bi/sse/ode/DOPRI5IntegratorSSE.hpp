/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_SSE_ODE_DOPRI5INTEGRATORSSE_HPP
#define BI_SSE_ODE_DOPRI5INTEGRATORSSE_HPP

namespace bi {
/**
 * @copydoc DOPRI5Integrator
 */
template<class B, class S, class T1>
class DOPRI5IntegratorSSE {
public:
  /**
   * @copydoc DOPRI5Integrator::integrate()
   */
  static void update(const T1 t1, const T1 t2, State<B,ON_HOST>& s);
};
}

#include "../sse_host.hpp"
#include "../math/function.hpp"
#include "../math/control.hpp"
#include "../../host/ode/DOPRI5VisitorHost.hpp"
#include "../../host/ode/IntegratorConstants.hpp"
#include "../../state/Pa.hpp"
#include "../../typelist/front.hpp"
#include "../../typelist/pop_front.hpp"

template<class B, class S, class T1>
void bi::DOPRI5IntegratorSSE<B,S,T1>::update(const T1 t1, const T1 t2,
    State<B,ON_HOST>& s) {
  /* pre-condition */
  BI_ASSERT(t1 < t2);

  typedef host_vector_reference<sse_real> vector_reference_type;
  typedef Pa<ON_HOST,B,host,host,sse_host,sse_host> PX;
  typedef DOPRI5VisitorHost<B,S,S,real,PX,sse_real> Visitor;
  static const int N = block_size<S>::value;
  const int P = s.size();

  #pragma omp parallel
  {
    sse_real buf[10*N];
    vector_reference_type x0(buf, N);
    vector_reference_type x1(buf + N, N);
    vector_reference_type x2(buf + 2*N, N);
    vector_reference_type x3(buf + 3*N, N);
    vector_reference_type x4(buf + 4*N, N);
    vector_reference_type x5(buf + 5*N, N);
    vector_reference_type x6(buf + 6*N, N);
    vector_reference_type err(buf + 7*N, N);
    vector_reference_type k1(buf + 8*N, N);
    vector_reference_type k7(buf + 9*N, N);

    sse_real e, e2;
    real t, h, logfacold, logfac11, fac, e2max;
    int n, id, p;
    bool k1in;
    PX pax;

    #pragma omp for
    for (p = 0; p < P; p += BI_SSE_SIZE) {
      t = t1;
      h = h_h0;
      logfacold = bi::log(BI_REAL(1.0e-4));
      k1in = false;
      n = 0;
      sse_host_load<B,S>(s, p, x0);

      /* integrate */
      while (t < t2 && n < h_nsteps) {
        if (BI_REAL(0.1)*bi::abs(h) <= bi::abs(t)*h_uround) {
          // step size too small
        }
        if (t + BI_REAL(1.01)*h - t2 > BI_REAL(0.0)) {
          h = t2 - t;
          if (h <= BI_REAL(0.0)) {
            t = t2;
            break;
          }
        }

        /* stages */
        Visitor::stage1(t, h, s, p, pax, x0.buf(), x1.buf(), x2.buf(), x3.buf(), x4.buf(), x5.buf(), x6.buf(), k1.buf(), err.buf(), k1in);
        k1in = true; // can reuse from previous iteration in future
        sse_host_store<B,S>(s, p, x1);

        Visitor::stage2(t, h, s, p, pax, x0.buf(), x2.buf(), x3.buf(), x4.buf(), x5.buf(), x6.buf(), err.buf());
        sse_host_store<B,S>(s, p, x2);

        Visitor::stage3(t, h, s, p, pax, x0.buf(), x3.buf(), x4.buf(), x5.buf(), x6.buf(), err.buf());
        sse_host_store<B,S>(s, p, x3);

        Visitor::stage4(t, h, s, p, pax, x0.buf(), x4.buf(), x5.buf(), x6.buf(), err.buf());
        sse_host_store<B,S>(s, p, x4);

        Visitor::stage5(t, h, s, p, pax, x0.buf(), x5.buf(), x6.buf(), err.buf());
        sse_host_store<B,S>(s, p, x5);

        Visitor::stage6(t, h, s, p, pax, x0.buf(), x6.buf(), err.buf());

        /* compute error */
        Visitor::stageErr(t, h, s, p, pax, x0.buf(), x6.buf(), k7.buf(), err.buf());

        /* determine largest error among trajectories */
        e2 = BI_REAL(0.0);
        for (id = 0; id < N; ++id) {
          e = err[id]*h/(bi::max(bi::abs(x0(id)), bi::abs(x6(id)))*h_rtoler + h_atoler);
          e2 += e*e;
        }
        #ifdef ENABLE_SINGLE
        e2max = bi::max(bi::max(e2.unpacked.a, e2.unpacked.b), bi::max(e2.unpacked.c, e2.unpacked.d));
        #else
        e2max = bi::max(e2.unpacked.a, e2.unpacked.b);
        #endif
        e2max /= N;

        if (e2max <= BI_REAL(1.0)) {
          /* accept */
          t += h;
          x0.swap(x6);
          k1.swap(k7);
        }
        sse_host_store<B,S>(s, p, x0);

        /* compute next step size */
        if (t < t2) {
          logfac11 = h_expo*bi::log(e2max);
          if (e2max > BI_REAL(1.0)) {
            /* step was rejected */
            h *= bi::max(h_facl, bi::exp(h_logsafe - logfac11));
          } else {
            /* step was accepted */
            fac = bi::exp(h_beta*logfacold + h_logsafe - logfac11); // Lund-stabilization
            fac = bi::min(h_facr, bi::max(h_facl, fac)); // bound
            h *= fac;
            logfacold = BI_REAL(0.5)*bi::log(bi::max(e2max, BI_REAL(1.0e-8)));
          }
        }

        ++n;
      }
    }
  }
}

#endif
