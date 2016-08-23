// *****************************************************************************
/*!
  \file      src/PDE/CompFlowProblem.h
  \author    J. Bakosi
  \date      Fri 22 Jul 2016 02:49:02 PM MDT
  \copyright 2012-2015, Jozsef Bakosi, 2016, Los Alamos National Security, LLC.
  \brief     Problem configurations for the compressible flow equations
  \details   This file defines policy classes for the compressible flow
    equations, defined in PDE/CompFlow.h.

    General requirements on flow equations problem policy classes:

    - Must define the static function _type()_, returning the enum value of the
      policy option. Example:
      \code{.cpp}
        static ctr::ProblemType type() noexcept {
          return ctr::ProblemType::USER_DEFINED;
        }
      \endcode
      which returns the enum value of the option from the underlying option
      class, collecting all possible options for coefficients policies.
*/
// *****************************************************************************
#ifndef CompFlowProblem_h
#define CompFlowProblem_h

#include <boost/mpl/vector.hpp>

#include "Types.h"
#include "Inciter/Options/Problem.h"

namespace inciter {

//! CompFlow system of PDEs problem: user defined
class CompFlowProblemUserDefined {
  public:

    //! Set initial conditions
    //! \param[in] deck Input deck
    //! \param[in] coord Mesh node coordinates
    //! \param[in,out] unk Array of unknowns
    //! \param[in] e Equation system index, i.e., which compressible
    //!   flow equation system we operate on among the systems of PDEs
    //! \param[in] offset System offset specifying the position of the system of
    //!   PDEs among other systems
    template< class eq >
    static void init( const ctr::InputDeck& deck,
                      const std::array< std::vector< tk::real >, 3 >& coord,
                      tk::MeshNodes& unk,
                      tk::ctr::ncomp_type e,
                      tk::ctr::ncomp_type offset )
    {
      IGNORE(deck);
      IGNORE(e);
      const auto& x = coord[0];
      for (ncomp_t i=0; i<x.size(); ++i) {
         unk( i, 0, offset ) = 1.0;     // density
         unk( i, 1, offset ) = 0.0;     // density * velocity
         unk( i, 2, offset ) = 0.0;
         unk( i, 3, offset ) = -1.0;
         unk( i, 4, offset ) = 293.0;     // density * specific total energy
      }
    }

    static ctr::ProblemType type() noexcept
    { return ctr::ProblemType::USER_DEFINED; }
};

//! List of all CompFlow problem policies
using CompFlowProblems = boost::mpl::vector< CompFlowProblemUserDefined >;

} // inciter::

#endif // CompFlowProblem_h