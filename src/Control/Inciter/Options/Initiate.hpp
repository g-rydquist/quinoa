// *****************************************************************************
/*!
  \file      src/Control/Inciter/Options/Initiate.hpp
  \copyright 2012-2015 J. Bakosi,
             2016-2018 Los Alamos National Security, LLC.,
             2019-2021 Triad National Security, LLC.
             All rights reserved. See the LICENSE file for details.
  \brief     Initiation options for initial conditions
  \details   Initiation options for initial conditions
*/
// *****************************************************************************
#ifndef InitiateOptions_h
#define InitiateOptions_h

#include <brigand/sequences/list.hpp>

#include "Toggle.hpp"
#include "Keywords.hpp"
#include "PUPUtil.hpp"

namespace inciter {
namespace ctr {

//! Initiation types
enum class InitiateType : uint8_t { IMPULSE
                                  , LINEAR };

//! Pack/Unpack InitiateType: forward overload to generic enum class packer
inline void operator|( PUP::er& p, InitiateType& e ) { PUP::pup( p, e ); }

//! \brief Initiation options: outsource to base templated on enum type
class Initiate : public tk::Toggle< InitiateType > {

  public:
    //! Valid expected choices to make them also available at compile-time
    using keywords = brigand::list< kw::impulse
                                  , kw::linear
                                  >;

    //! \brief Options constructor
    //! \details Simply initialize in-line and pass associations to base, which
    //!    will handle client interactions
    explicit Initiate() :
      tk::Toggle< InitiateType >(
        //! Group, i.e., options, name
        kw::initiate::name(),
        //! Enums -> names (if defined, policy codes, if not, name)
        { { InitiateType::IMPULSE, kw::impulse::name() },
          { InitiateType::LINEAR, kw::linear::name() } },
        //! keywords -> Enums
        { { kw::impulse::string(), InitiateType::IMPULSE },
          { kw::linear::string(), InitiateType::LINEAR } } )
    {}

};

} // ctr::
} // inciter::

#endif // InitiateOptions_h
