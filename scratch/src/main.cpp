#include <cpprelude/fmt.h>
#include "benchmark.h"
#include <ldgr/dummy.h>
#include <ldgr/world.h>
#include <ldgr/type_utils.h>
using namespace cpprelude;

void
test_01()
{
	using namespace ldgr;

	World world;
	Entity batman = world.create_entity();
	Entity bruce = world.create_entity();
	world.remove_entity(batman);
	Entity koko = world.create_entity();
	world.remove_entity(bruce);
	world.remove_entity(koko);
	
	//Entity implant = world.create_entity();
	//Component<int> implant_length = world.create_component(implant, 10);
	//auto components = world.get_all_components(implant);
	//auto length = world.get_component<int>(implant);
	//int x = *length;
	//println(x);
}

i32
main()
{
	test_01();
	benchmark();
	// println("4 + 6 = ", ldgr::add(4, 6));
	// println("Hello, World!");
	// test_01();
	return 0;
}