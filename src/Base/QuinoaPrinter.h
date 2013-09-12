//******************************************************************************
/*!
  \file      src/Base/QuinoaPrinter.h
  \author    J. Bakosi
  \date      Thu Sep 12 10:21:11 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Quinoa's printer
  \details   Quinoa's printer
*/
//******************************************************************************
#ifndef QuinoaPrinter_h
#define QuinoaPrinter_h

#include <Printer.h>
#include <QuinoaControl.h>

namespace quinoa {

//! QuinoaPrinter : Printer
class QuinoaPrinter : public Printer {

  public:
    //! Constructor
    explicit QuinoaPrinter() = default;

    //! Destructor
    ~QuinoaPrinter() noexcept override {}

    //! Echo vector of vector of element names
    //! Fields of vector<vector< struct{field, name, plot} >> must exist
    //! See src/Control/QuinoaControlTypes.h for the definitions of operator <<
    //! for outputing Term and vector<Term>, and operator <<= for outputing
    //! requested (i.e., plotted) Term
    template<typename... tags>
    void vecvecNames(const QuinoaControl& ctr,
                     const std::string& msg,
                     const bool req = false) const {
      std::cout << m_item_name_fmt % m_item_indent % msg;
      if (req) for (auto& v : ctr.get<tags...>()) std::cout <<= v;
      else for (auto& v : ctr.get<tags...>()) std::cout << v;
      std::cout << '\n';
    }

//     //! Echo vector of Option.names
//     template<class OptionType, typename... tags>
//     void echoVecOptName(const std::string& msg) const {
//       control::Option<OptionType> opt;
//       std::cout << "   - " << msg << ": {";
//       for (auto& v : get<tags...>()) {
//         std::cout << " " << opt.name(v);
//       }
//       std::cout << " }" << std::endl;
//     }

  private:
    //! Don't permit copy constructor
    QuinoaPrinter(const QuinoaPrinter&) = delete;
    //! Don't permit copy assigment
    QuinoaPrinter& operator=(const QuinoaPrinter&) = delete;
    //! Don't permit move constructor
    QuinoaPrinter(QuinoaPrinter&&) = delete;
    //! Don't permit move assigment
    QuinoaPrinter& operator=(QuinoaPrinter&&) = delete;
};

} // namespace quinoa

#endif // QuinoaPrinter_h