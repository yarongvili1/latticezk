#include <iostream>
#include <catch2/catch.hpp>

namespace LatticeZK {

TEST_CASE( "hello", "[latticezk]" ) {
	REQUIRE( 1 == 1 );
	std::cerr << "hello" << std::endl;
}

} // namespace LatticeZK
