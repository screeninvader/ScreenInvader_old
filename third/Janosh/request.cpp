/*
 * request.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: elchaschab
 */

#include "request.hpp"

namespace janosh {

void read_request(Request& req, istream& is) {
  boost::archive::text_iarchive ia(is);
  ia >> req;
}

void write_request(Request& req, ostream& os) {
  boost::archive::text_oarchive oa(os);
  oa << req;
}

} /* namespace janosh */
