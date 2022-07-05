#include "sugarpp/lazy/lazy.hpp"
#include "sugarpp/io/io.hpp"
#include "sugarpp/range/range.hpp"
#include <chrono>

using namespace SugarPP;

int main()
{
	/*"Declare" a lazy variable with inferred type from the initializer*/
	Lazy lazyInt{ [] { return 1; } };
	print(lazyInt.value());

	/*Lazy variable with ThreadSafetyMode::Synchronized*/
	Lazy lazyDouble
	{
		[]
		{
			int const sleepMilliseconds = (rand() % 10) * 100; //randomly sleep for 0~1 seconds
			print("Thread #", std::this_thread::get_id(), " sleep for ", sleepMilliseconds, " ms");
			std::this_thread::sleep_for(std::chrono::milliseconds{ sleepMilliseconds });
			return 2.0;
		},
		ThreadSafetyMode::Synchronized
	};
		//make 10 threads trying to access its value concurrently
	std::vector<std::thread> threads;
	threads.reserve(10);
	for (auto i : Range(0, 10))
	{
		threads.emplace_back([&lazyDouble]() mutable 
		{ 
			print("Thread #", std::this_thread::get_id(), " result: ", lazyDouble.value());
		});
	}
	for (auto& thread : threads)
		thread.join();

}