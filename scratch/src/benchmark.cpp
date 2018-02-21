#include "benchmark.h"
#include "ldgr/world.h"
#include <cpprelude/micro_benchmark.h>
#include <cpprelude/dynamic_array.h>
#include <entt/entt.hpp>
using namespace cpprelude;
using namespace ldgr;


usize
bm_entites(workbench *bench, usize limit)
{
	usize result = 0;
	usize offset = rand();

	World world;

	bench->watch.start();
		for(usize i = 0; i < limit; ++i)
			world.create_entity();

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

	entt::Registry<u64> world;

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
	usize limit = 100000;

	compare_benchmark(std::cout, {
		CPPRELUDE_BENCHMARK(bm_entt_entities, limit),
		CPPRELUDE_BENCHMARK(bm_entites, limit)
	});
}