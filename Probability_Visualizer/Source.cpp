#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::cout;
using std::ifstream;

using olc::Pixel;

class Visualizer : public olc::PixelGameEngine
{
public:
	uint32_t ITERATIONS;
	uint32_t AGENTS;
	uint32_t ACTIONS;
	float* data;
	
	float y;
	float yv;
	
	Visualizer(uint32_t iterations, uint32_t agents, uint32_t actions, float* data)
	{
		sAppName = "Probability Visualizer";
		ITERATIONS = iterations;
		AGENTS = agents;
		ACTIONS = actions;
		this->data = data;
	}

public:
	bool OnUserCreate() override
	{
		y = 0;
		yv = 0;
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		
		yv -= 0.4f * (GetMouseWheel() > 0);
		yv += 0.4f * (GetMouseWheel() < 0);
		yv *= 0.988f;
		y += yv;

		// the data is 1 float for color(Green/Red) and ACTIONS floats for the probabilities
		// there are AGENTS number of these sets of data and ITERATIONS number of these sets of AGENTS
		
		// Display the probabilities of each agent in each iteration side by side
		uint32_t totalXElements = 1 + AGENTS + ACTIONS * AGENTS;
		uint32_t iterationShift = 1 + ACTIONS;
		uint32_t index = (-y + 100) * AGENTS * iterationShift;

		for (uint32_t i = y + 100; i++;)
		{
			uint32_t drawX = 1;
			uint32_t drawY = i - y;

			if (drawY >= 768 || index >= (1 + ACTIONS) * AGENTS * ITERATIONS)
				break;
			
			for (uint32_t j = 0; j < AGENTS; j++)
			{
				float isSurvivor = data[index++];
				Pixel color = Pixel(255 * isSurvivor, 255 * (1 - isSurvivor), 0);	// Green for survivor, Red for dead
				
				for (uint32_t k = ACTIONS; k--;)
					Draw(drawX++, drawY, color * data[index++]);
				drawX++;
			}
		}
		
		return true;
	}
};

int main()
{
	uint32_t ITERATIONS;
	uint32_t AGENTS;
	uint32_t ACTIONS;
	float* data;

	ifstream file("data.txt");
	file >> ITERATIONS >> AGENTS >> ACTIONS;
	cout << "Iterations: " << ITERATIONS << "\nAgents: " << AGENTS << "\nActions: " << ACTIONS << "\n";

	data = new float[(1 + ACTIONS) * AGENTS * ITERATIONS];
	for (int i = 0; i < (1 + ACTIONS) * AGENTS * ITERATIONS; i++)
		file >> data[i];

	/*for (int i = 0; i < (1 + ACTIONS) * AGENTS * ITERATIONS; i++)
		cout << data[i] << "\n";*/
	
	uint32_t totalXElements = 1 + AGENTS + ACTIONS * AGENTS;
	uint32_t xResolution = 1536 / totalXElements;
	float yPixels = 1;
	uint32_t yResolution = 768 / yPixels;
	
	Visualizer visualizer(ITERATIONS, AGENTS, ACTIONS, data);
	if (visualizer.Construct(totalXElements, yResolution, xResolution, yPixels))
		visualizer.Start();
	return 0;
}




/*static class Random
{
public:
	static void MakeSeed(uint32_t* result, uint32_t seed)	// make seed from time and seed
	{
		*result = seed;
		*result = Hash((uint8_t*)result, sizeof(*result), nanosecond());
		*result = Hash((uint8_t*)result, sizeof(*result), microsecond());
	}

	void Seed(uint32_t* seed)	// seed the random number generator
	{
		state[0] = Hash((uint8_t*)seed, sizeof(*seed), *seed);
		state[1] = Hash((uint8_t*)seed, sizeof(*seed), state[0]);
	}

	uint32_t Ruint32()	// XORSHIFT128+
	{
		uint64_t a = state[0];
		uint64_t b = state[1];
		state[0] = b;
		a ^= a << 23;
		state[1] = a ^ b ^ (a >> 18) ^ (b >> 5);
		return uint32_t((state[1] + b) >> 32);
	}

	float Rfloat(float min, float max) { return min + (max - min) * Ruint32() * 2.3283064371e-10; }

	static uint32_t Hash(const uint8_t* key, size_t len, uint32_t seed)	// MurmurHash3
	{
		uint32_t h = seed;
		uint32_t k;
		for (size_t i = len >> 2; i; i--) {
			memcpy(&k, key, sizeof(uint32_t));
			key += sizeof(uint32_t);
			h ^= murmur_32_scramble(k);
			h = (h << 13) | (h >> 19);
			h = h * 5 + 0xe6546b64;
		}
		k = 0;
		for (size_t i = len & 3; i; i--) {
			k <<= 8;
			k |= key[i - 1];
		}
		h ^= murmur_32_scramble(k);
		h ^= len;
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;
		return h;
	}

private:
	uint64_t state[2];

	static uint32_t murmur_32_scramble(uint32_t k) {
		k *= 0xcc9e2d51;
		k = (k << 15) | (k >> 17);
		k *= 0x1b873593;
		return k;
	}

	static uint32_t nanosecond() { return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count(); }
	static uint32_t microsecond() { return duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count(); }
};*/