#include "benchmark.h"
#include "ldgr/world.h"
#include <cpprelude/micro_benchmark.h>
#include <cpprelude/dynamic_array.h>
#include <cpprelude/allocator.h>
#include <entt/entt.hpp>
using namespace cpprelude;
using namespace ldgr;


arena_t arena(MEGABYTES(100));
struct koko: Base_Entity
{
	int x;
};

usize
bm_entites(workbench *bench, usize limit)
{
	usize result = 0;
	usize offset = rand();

	World world;

	bench->watch.start();
		Entity entity;
		for(usize i = 0; i < limit; ++i)
			world.insert_entity(entity);

		for (usize i = 0; i < limit; ++i)
		{
			if ((i + offset) % 2 == 0)
			{
				if (world.remove_entity(i)) ++result;
			}
		}
	bench->watch.stop();
	return result;
}

usize
bm_entt_entities(workbench *bench, usize limit)
{
	usize result = 0;
	usize offset = rand();

	entt::DefaultRegistry world;

	bench->watch.start();
	for (usize i = 0; i < limit; ++i)
		world.create();

	for (usize i = 0; i < limit; ++i)
	{
		if ((i + offset) % 2 == 0)
		{
			world.destroy(i);
			++result;
		}
	}
	bench->watch.stop();
	return result;
}

void
benchmark()
{
	srand(time(0));
	usize limit = 10000;

	compare_benchmark(std::cout, {
		CPPRELUDE_BENCHMARK(bm_entt_entities, limit),
		CPPRELUDE_BENCHMARK(bm_entites, limit)
	});
}