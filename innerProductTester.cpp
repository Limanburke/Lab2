#include "innerProductTester.h"

void InnerProductTester::getRandomValues(std::vector<long long>& v)
{
	std::mt19937 gen(seed ? seed : std::random_device{}());
	std::uniform_int_distribution<> dist(minNumber, maxNumber);
	v.clear();
	v.reserve(ammount);
	for (int i = 0; i < ammount; i++)
	{
		v.push_back(dist(gen));
	}
}

void InnerProductTester::runAllTests(int mode)
{
	getRandomValues(vector1);
	getRandomValues(vector2);

	auto transform = getTransformFunc(mode);
	const char* modeNames[] = { "Basic (a * b)", "Heavy (sin(a)*cos(b))", "Moderate (a*b if even)" };

	std::cout << "Testing mode: " << modeNames[mode] << "\n";

	std::cout << "\n=== Inner product without policy ===\n";

		calculateVec([this, transform]() 
			{
			return std::inner_product(vector1.cbegin(), vector1.cend(), vector2.cbegin(), 0LL, std::plus<>(), transform);
			});


	std::cout << "\n=== Transform reduce with different policies ===\n";

		auto testPolicy = [&](const std::string& policyName, auto policy) 
			{
			std::cout << "\nPolicy: " << policyName << "\n";
			calculateVec([this, policy, transform]() 
				{
				return std::transform_reduce(policy,
					vector1.cbegin(), vector1.cend(), vector2.cbegin(), 0LL, std::plus<>(), transform);
				});
			};
		testPolicy("sequenced", std::execution::seq);
		testPolicy("unsequenced", std::execution::unseq);
		testPolicy("parallel", std::execution::par);
		testPolicy("parallel unsequenced", std::execution::par_unseq);


	std::cout << "\n=== Custom parallel implementation ===\n";
		calculateK(transform);
}

void InnerProductTester::calculateK(auto transform)
{
	std::cout << "\n=== Searching best k ===\n";

	int bestK = 1;
	double bestTime = std::numeric_limits<double>::max();
	unsigned int hw_threads = std::thread::hardware_concurrency();
	long long expected = std::inner_product(vector1.begin(), vector1.end(), vector2.begin(), 0LL, std::plus<long long>(), transform);
	bool showOutput = true;
	int analyzer = 0;

	std::cout << "Inner product of vectors with " << ammount << " random numbers: " << expected << "\n";
	std::cout << "----------------------------------------------\n";
	std::cout << "K\t\tTime\t\tStatus\n";
	std::cout << "----------------------------------------------\n";

	for (int k = 1; k < vector1.size(); k++)
	{
		long long result = 0;
		auto time = timeCheckVoid([&]() {
			result = parallel_inner_product(vector1.begin(), vector1.end(), vector2.begin(), 0LL, transform, k);
			}, 1);
		bool valid = (result == expected);
		double curTime = time;
		std::string text = "";

		if (curTime < bestTime && valid)
		{
			bestTime = curTime;
			bestK = k;
			analyzer = 0;
			text = "new best k";
			showOutput = true;
		}
		else
		{
			text = "";
			analyzer++;
		}

		if (!valid) 
		{
			std::cout << "STOPPED: Invalid result at k=" << k << "\n";
			break;
		}

		if (analyzer <= 3)
			std::cout << k << "\t\t" << std::fixed << std::setprecision(4) << curTime << "\t\t" << text << "\n";
		if (analyzer > 3 && showOutput)
		{
			std::cout << "...\n";
			showOutput = false;
		}
		if (analyzer >= hw_threads * 4 && k > hw_threads)
		{
			std::cout << "No further improvement expected after " << k << " iterations\n";
			std::cout << "----------------------------------------------\n";
			break;
		}
	}
	std::cout << "Best k: " << bestK << "(" << bestTime << " ms)\n";
	std::cout << "Hardware threads: " << std::thread::hardware_concurrency() << "\n";
	std::cout << "The ratio of the value of k to the number of hardware threads: " <<
		bestK << "/" << std::thread::hardware_concurrency() << " = " << (bestK / static_cast<double>(std::thread::hardware_concurrency()));
}

