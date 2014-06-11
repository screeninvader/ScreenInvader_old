/*
 * shared_pointers.hpp
 *
 *  Created on: Feb 22, 2014
 *      Author: elchaschab
 */

#include <memory>
#include <iostream>
#include <boost/asio.hpp>

#ifndef SHARED_POINTERS_HPP_
#define SHARED_POINTERS_HPP_
using std::shared_ptr;

typedef shared_ptr<std::ostream> ostream_ptr;
typedef shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
typedef shared_ptr<boost::asio::streambuf> streambuf_ptr;


#endif /* SHARED_POINTERS_HPP_ */
