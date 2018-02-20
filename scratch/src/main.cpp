#include <cpprelude/fmt.h>
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
	
	Entity implant = world.create_entity();
	Component<int> implant_length = world.create_component(implant, 10);

	println(implant_length._type_utils->stuff());

	auto toto = ldgr::type_utils<int>();
	println(toto->name);
}

i32
main()
{
	println("4 + 6 = ", ldgr::add(4, 6));
	println("Hello, World!");
	test_01();
	return 0;
}