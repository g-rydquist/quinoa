// *****************************************************************************
/*!
  \file      src/NoWarning/transform.hpp
  \copyright 2012-2015 J. Bakosi,
             2016-2018 Los Alamos National Security, LLC.,
             2019-2021 Triad National Security, LLC.
             All rights reserved. See the LICENSE file for details.
  \brief     Include brigand/algorithms/transform.hpp with turning off specific
             compiler warnings
*/
// *****************************************************************************
#ifndef nowarning_transform_h
#define nowarning_transform_h

#include "Macro.hpp"

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wundef"
  #pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif

#include <brigand/algorithms/transform.hpp>

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif // nowarning_transform_h
