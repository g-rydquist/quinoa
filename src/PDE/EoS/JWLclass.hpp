// *****************************************************************************
/*!
  \file      src/PDE/EoS/JWLclass.hpp
  \copyright 2012-2015 J. Bakosi,
             2016-2018 Los Alamos National Security, LLC.,
             2019-2021 Triad National Security, LLC.
             All rights reserved. See the LICENSE file for details.
  \brief     Jones, Wilkins, and Lee (JWL) equation of state
  \details   This file defines functions for the JWL equation of
             state for the compressible flow equations. These functions are
             taken from 'JWL Equation of State', Menikoff, LA-UR-15-29536.
*/
// *****************************************************************************
#ifndef JWLclass_h
#define JWLclass_h

#include <cmath>
#include <iostream>
#include "Data.hpp"

namespace inciter {

class JWLclass {

  private:
    tk::real m_w, m_cv, m_rho0, m_de, m_rhor, m_er, m_a, m_b, m_r1, m_r2, m_tr;


    tk::real intEnergy( tk::real rho, tk::real pr )
    // *************************************************************************
    //! \brief Calculate specific internal energy using the JWL equation of
    //!   state
    //! \param[in] rho Material density
    //! \param[in] pr Material pressure
    //! \return Material internal energy calculated using the JWL EoS
    //! \details By inverting Eqn. 1 in 'JWL Equation of State', Menikoff,
    //!   LA-UR-15-29536
    // *************************************************************************
    {
      tk::real e = - m_de + 1.0/m_w/rho*( pr
                    - m_a*(1.0 - m_w*rho/m_r1/m_rho0)*exp(-m_r1*m_rho0/rho)
                    - m_b*(1.0 - m_w*rho/m_r2/m_rho0)*exp(-m_r2*m_rho0/rho) );

      return e;
    }


    tk::real bisection( tk::real a, tk::real b, tk::real p_known, tk::real t_known )
    // *************************************************************************
    //! \brief Calculate density from known pressure and temperature using
    //!   bisection root finding method for JWL equation of state
    //! \param[in] a Left density bound for root finding
    //! \param[in] b Right density bound for root finding
    //! \param[in] p_known Known pressure
    //! \param[in] t_known Known temperature
    //! \return Material density calculated by inverting JWL pressure equation
    // *************************************************************************
    {
      tk::real tol = 1e-12;
      std::size_t maxiter = 1000;
      std::size_t i(0);
      tk::real c;
      tk::real root(0);
      std::size_t idebug = 0;

      // function to minimize: fcn = p_known - PfromRT
      // bounds b > a

      while (i < maxiter)
      {
        c = (a + b)/2.0;
        auto fcn = p_known - PfromRT( c, t_known);
        if ( idebug == 1)
        {
          std::cout << "Bisection iter:      " << i << std::endl;
          std::cout << "fcn:  " << fcn << std::endl;
          std::cout << "(b - a)/2.0: " << (b - a)/2.0 << std::endl;
        }

        if ( std::abs(fcn) <= 1e-16 or (b - a)/2.0 < tol )
        {
          root = c;
          break;
        }

        i++;
        if ( static_cast< int > (std::copysign( 1.0, p_known - PfromRT( c, t_known) )) ==
             static_cast< int > (std::copysign( 1.0, p_known - PfromRT( a, t_known) )) )
        {
          a = c;
        }
        else
        {
          b = c;
        }

        if ( i == maxiter )
        {
          Throw("JWL Bisection for density failed to converge after iterations"
          + std::to_string(i));
        }

      }
      return root;
    }


    tk::real PfromRT( tk::real rho, tk::real T)
    // *************************************************************************
    //! \brief Calculate pressure from density and temperature using JWL
    //!   equation of state
    //! \param[in] rho Material density
    //! \param[in] T Material temperature
    //! \return Material pressure calculated using the JWL EoS
    //! \details From Eqn. 14 in 'JWL Equation of State', Menikoff, LA-UR-15-29536
    // *************************************************************************
    {
      return ( m_a*exp(-m_r1*m_rho0/rho) + m_b*exp(-m_r2*m_rho0/rho) +
        m_w*(m_cv*T*rho) );
    }

  public:
    // *************************************************************************
    //  Default constructor
    // *************************************************************************
    JWLclass(){}


    // *************************************************************************
    //  Constructor
    //! \param[in] w Grueneisen coefficient
    //! \param[in] cv Specific heat at constant volume
    //! \param[in] rho0 Density of initial state
    //! \param[in] de Heat of detonation for products. For reactants, it is
    //!   chosen such that the ambient internal energy (e0) is 0.
    //! \param[in] rhor Density of reference state
    //! \param[in] er Energy of reference state
    //! \param[in] A Parameter A
    //! \param[in] B Parameter B
    //! \param[in] R1 Parameter R1
    //! \param[in] R2 Parameter R2
    // *************************************************************************
    JWLclass( tk::real w, tk::real cv, tk::real rho0, tk::real de, tk::real rhor,
         tk::real er, tk::real A, tk::real B, tk::real R1, tk::real R2 ) :
      m_w(w),
      m_cv(cv),
      m_rho0(rho0),
      m_de(de),
      m_rhor(rhor),
      m_er(er),
      m_a(A),
      m_b(B),
      m_r1(R1),
      m_r2(R2)
    {
      // reference temperature from Eqn (15)
      m_tr = 1.0/m_cv * (m_er + de -
        (m_a/m_r1*exp(-m_r1*m_rho0/m_rhor) +
         m_b/m_r2*exp(-m_r2*m_rho0/m_rhor)) / m_rho0);
    }


    tk::real eos_density( tk::real pr,
                          tk::real temp )
    // *************************************************************************
    //! \brief Calculate density from the material pressure and temperature
    //!   using the stiffened-gas equation of state
    //! \param[in] pr Material pressure
    //! \param[in] temp Material temperature
    //! \return Material density calculated using the stiffened-gas EoS
    // *************************************************************************
    {
      tk::real r_guessL = 1e-2*m_rhor;  // left density bound
      tk::real r_guessR = 1e2*m_rhor;   // right density bound
      tk::real rho;

      rho = bisection( r_guessL, r_guessR, pr, temp );

      return rho;
    }


    tk::real eos_pressure( tk::real arho,
                           tk::real u,
                           tk::real v,
                           tk::real w,
                           tk::real arhoE,
                           tk::real alpha=1.0,
                           std::size_t imat=0 )
    // *************************************************************************
    //! \brief Calculate pressure from the material density, momentum and total
    //!   energy using the stiffened-gas equation of state
    //! \param[in] arho Material partial density (alpha_k * rho_k)
    //! \param[in] u X-velocity
    //! \param[in] v Y-velocity
    //! \param[in] w Z-velocity
    //! \param[in] arhoE Material total energy (alpha_k * rho_k * E_k)
    //! \param[in] alpha Material volume fraction. Default is 1.0, so that for
    //!   the single-material system, this argument can be left unspecified by
    //!   the calling code
    //! \param[in] imat Material-id who's EoS is required. Default is 0, so that
    //!   for the single-material system, this argument can be left unspecified
    //!   by the calling code
    //! \return Material partial pressure (alpha_k * p_k) calculated using the
    //!   stiffened-gas EoS
    //! \details From Eqn. 1 in 'JWL Equation of State', Menikoff, LA-UR-15-29536
    // *************************************************************************
    {
      // reference energy (input quantity, might need for calculation)
//      tk::real e0 = a/r1*exp(-r1*rho0/rho) + b/r2*exp(-r2*rho0/rho);
      // specific internal energy
      tk::real e = (arhoE - 0.5*arho*(u*u + v*v + w*w))/arho;

      tk::real partpressure =
        m_a*(alpha - m_w*arho/(m_rho0*m_r1))*exp(-m_r1*alpha*m_rho0/arho) +
        m_b*(alpha - m_w*arho/(m_rho0*m_r2))*exp(-m_r2*alpha*m_rho0/arho) +
        m_w*arho*(e + m_de);

      // check partial pressure divergence
      if (!std::isfinite(partpressure)) {
        std::cout << "Material-id:      " << imat << std::endl;
        std::cout << "Volume-fraction:  " << alpha << std::endl;
        std::cout << "Partial density:  " << arho << std::endl;
        std::cout << "Total energy:     " << arhoE << std::endl;
        std::cout << "Velocity:         " << u << ", " << v << ", " << w
          << std::endl;
        Throw("Material-" + std::to_string(imat) +
          " has nan/inf partial pressure: " + std::to_string(partpressure) +
          ", material volume fraction: " + std::to_string(alpha));
      }

      return partpressure;
    }


    tk::real eos_soundspeed( tk::real arho,
                             tk::real apr,
                             tk::real alpha=1.0,
                             std::size_t imat=0 )
    // *************************************************************************
    //! Calculate speed of sound from the material density and material pressure
    //! \param[in] arho Material partial density (alpha_k * rho_k)
    //! \param[in] apr Material partial pressure (alpha_k * p_k)
    //! \param[in] alpha Material volume fraction. Default is 1.0, so that for
    //!   the single-material system, this argument can be left unspecified by
    //!   the calling code
    //! \param[in] imat Material-id who's EoS is required. Default is 0, so that
    //!   for the single-material system, this argument can be left unspecified
    //!   by the calling code
    //! \return Material speed of sound using the stiffened-gas EoS
    // *************************************************************************
    {
      // limiting pressure to near-zero
      auto apr_eff = std::max( 1.0e-15, apr );

      auto co1 = m_rho0*alpha*alpha/(arho*arho);
      auto co2 = alpha*(1.0+m_w)/arho;

      tk::real ss = m_a*(m_r1*co1 - co2) * exp(-m_r1*alpha*m_rho0/arho)
                  + m_b*(m_r2*co1 - co2) * exp(-m_r2*alpha*m_rho0/arho)
                  + (1.0+m_w)*apr_eff/arho;

      ss = std::sqrt(ss);

      // check sound speed divergence
      if (!std::isfinite(ss)) {
        std::cout << "Material-id:      " << imat << std::endl;
        std::cout << "Volume-fraction:  " << alpha << std::endl;
        std::cout << "Partial density:  " << arho << std::endl;
        std::cout << "Partial pressure: " << apr << std::endl;
        Throw("Material-" + std::to_string(imat) + " has nan/inf sound speed: "
          + std::to_string(ss) + ", material volume fraction: " +
          std::to_string(alpha));
      }

      return ss;
    }


    tk::real eos_totalenergy( tk::real rho,
                              tk::real u,
                              tk::real v,
                              tk::real w,
                              tk::real pr )
    // *************************************************************************
    //! \brief Calculate material specific total energy from the material
    //!   density, momentum and material pressure
    //! \param[in] rho Material density
    //! \param[in] u X-velocity
    //! \param[in] v Y-velocity
    //! \param[in] w Z-velocity
    //! \param[in] pr Material pressure
    //! \return Material specific total energy using the stiffened-gas EoS
    // *************************************************************************
    {
      // reference energy (input quantity, might need for calculation)
//      tk::real e0 = a/r1*exp(-r1*rho0/rho) + b/r2*exp(-r2*rho0/rho);

      tk::real rhoE = rho*intEnergy( rho, pr )
                    + 0.5*rho*(u*u + v*v + w*w);

      return rhoE;
    }


    tk::real eos_temperature( tk::real arho,
                              tk::real u,
                              tk::real v,
                              tk::real w,
                              tk::real arhoE,
                              tk::real alpha=1.0 )
    // *************************************************************************
    //! \brief Calculate material temperature from the material density, and
    //!   material specific total energy
    //! \param[in] arho Material partial density (alpha_k * rho_k)
    //! \param[in] u X-velocity
    //! \param[in] v Y-velocity
    //! \param[in] w Z-velocity
    //! \param[in] arhoE Material total energy (alpha_k * rho_k * E_k)
    //! \param[in] alpha Material volume fraction. Default is 1.0, so that for
    //!   the single-material system, this argument can be left unspecified by
    //!   the calling code
    //! \return Material temperature using the stiffened-gas EoS
    // *************************************************************************
    {
      tk::real rho = arho/alpha;

      // reference energy (input quantity, might need for calculation)
//      tk::real e0 = a/r1*exp(-r1*rho0/rho) + b/r2*exp(-r2*rho0/rho);

      tk::real t = ((arhoE - 0.5*arho*(u*u + v*v + w*w))/arho + m_de -
        1.0/m_rho0*( m_a/m_r1*exp(-m_r1*m_rho0/rho)
                   + m_b/m_r2*exp(-m_r2*m_rho0/rho) ))/m_cv;

      return t;
    }


    tk::real min_eff_pressure( tk::real min )
    // *************************************************************************
    //! Compute the minimum effective pressure
    //! \param[in] min Minimum threshold in positivity preserving limiting
    //! \return Minimum effective pressure
    // *************************************************************************
    {
      return min;  // TBN: double check to make sure this is appropriate for JWL
    }


    // Destructor
    ~JWLclass() {}


    /** @name Charm++ pack/unpack serializer member functions */
    ///@{
    //! \brief Pack/Unpack serialize member function
    //! \param[in,out] p Charm++'s PUP::er serializer object reference
    void pup( PUP::er &p ) /*override*/ {
      p | m_w;
      p | m_cv;
      p | m_rho0;
      p | m_de;
      p | m_rhor;
      p | m_er;
      p | m_a;
      p | m_b;
      p | m_r1;
      p | m_r2;
      p | m_tr;
    }
    //! \brief Pack/Unpack serialize operator|
    //! \param[in,out] p Charm++'s PUP::er serializer object reference
    //! \param[in,out] i JWLclass object reference
    friend void operator|( PUP::er& p, JWLclass& i ) { i.pup(p); }
    //@}
};

} //inciter::

#endif // JWLclass_h