#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <execution>
#include <numeric>
#include <iomanip>
#include <thread>
#include <random>
#include "timeCheck.h"


class InnerProductTester
{
	std::vector<long long> vector1;
	std::vector<long long> vector2;
	int ammount = 0;
	int minNumber = 0;
	int maxNumber = 0;
	int seed = 0;

	static long long basicTransform(long long a, long long b) 
	{
		return a * b;
	}
	static long long heavyTransform(long long a, long long b) 
	{
		return static_cast<long long>(std::abs(std::sin(a) * std::cos(b)) * 1000);
	}
	static long long moderateTransform(long long a, long long b) 
	{
		return ((a * b) % 2 == 0) ? a * b : 0LL;
	}
	auto getTransformFunc(int mode) 
	{
		switch (mode) 
		{
		case 0: return basicTransform;
		case 1: return heavyTransform;
		case 2: return moderateTransform;
		default: return basicTransform;
		}
	}

	template <class F>
	void calculateVec(F&& f)
	{
		std::cout << "Inner product of vectors with " << ammount << " random numbers: ";
		auto value = f();
		std::cout << value << std::endl;
		int repeat = 1;
		double avg_ms = timeCheck(f, repeat);
		if(repeat > 1)
			std::cout << "\nTaken time (avg over " << repeat << " runs): " << avg_ms << " ms\n";
		else 
			std::cout << "\nTaken time: " << avg_ms << " ms\n";
	}

	template<typename ExecPolicy>
	void testPolicy(const std::string& policyName, ExecPolicy policy)
	{
		std::cout << "\nPolicy: " << policyName << "\n";
			calculateVec([this, policy]() 
				{
					return std::transform_reduce(policy, vector1.cbegin(), vector1.cend(), vector2.cbegin(), 0LL);
				});
	}

	template <typename Iterator1, typename Iterator2, typename T, typename BinaryOp>
	struct inner_product_block
	{
		BinaryOp op;

		inner_product_block(BinaryOp op) : op(op) {}

		void operator()(Iterator1 first1, Iterator1 last1, Iterator2 first2, T& result)
		{
			result = std::inner_product(first1, last1, first2, T(0), std::plus<T>(), op);
		}
	};

	template <typename Iterator1, typename Iterator2, typename T, typename BinaryOp>
	T parallel_inner_product(Iterator1 first1, Iterator1 last1, Iterator2 first2, T init,
		BinaryOp op, unsigned long k = 0)
	{
		unsigned long const length = std::distance(first1, last1);
		if (!length)
			return init;

		if (k == 0)
		{
			unsigned long const min_per_thread = 25;
			unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
			unsigned long const hardware_threads = std::thread::hardware_concurrency();
			k = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
		}
		const unsigned long min_block_size = 1000UL;
		unsigned long max_allowed = std::max(1UL, length / min_block_size);

		if (k > max_allowed) 
			k = max_allowed;

		unsigned long  num_threads = k;
		if (num_threads <= 1)
			return std::inner_product(first1, last1, first2, init, std::plus<T>(), op);

		unsigned long  block_size = length / num_threads;
		if (block_size == 0) {
			num_threads = 1;
			block_size = length;
		}
		std::vector<T> results(num_threads, 0);
		std::vector<std::thread> threads(num_threads - 1);
		Iterator1 block_start1 = first1;
		Iterator2 block_start2 = first2;
		for (unsigned long i = 0; i < (num_threads - 1); ++i)
		{
			Iterator1 block_end1 = block_start1;
			Iterator2 block_end2 = block_start2;
			std::advance(block_end1, block_size);
			std::advance(block_end2, block_size);
			threads[i] = std::thread(inner_product_block<Iterator1, Iterator2, T, BinaryOp>(op),
				block_start1, block_end1, block_start2, std::ref(results[i]));
			block_start1 = block_end1;
			block_start2 = block_end2;
		}
		inner_product_block<Iterator1, Iterator2, T, BinaryOp> last_block(op);
		last_block(block_start1, last1, block_start2, results[num_threads - 1]);
		for (auto& entry : threads)
			entry.join();
		return std::accumulate(results.begin(), results.end(), init);
	}

	void getRandomValues(std::vector<long long>& v);
	void calculateK(auto transform);

public:
	InnerProductTester(int user_seed, int user_ammount, int user_minNumber, int user_maxNumber) : seed(user_seed), ammount(user_ammount), minNumber(user_minNumber),
		maxNumber(user_maxNumber) {}
	void runAllTests(int mode);
};