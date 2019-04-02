//
//
//

#include "Pedigree/Debug.hpp"

#include <iostream>
// using std::cerr

#include <exception>
// using std::exception
// using std::terminate

#ifdef BOOST_NO_EXCEPTIONS

void boost::throw_exception(std::exception const &e) {
  std::cerr << e.what() << '\n';

  std::terminate();
}

#endif // BOOST_NO_EXCEPTIONS

