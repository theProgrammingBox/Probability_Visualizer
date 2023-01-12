#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::cout;


static class Random
{
public:
	void MakeSeed(uint32_t* result, uint32_t seed)	// make seed from time and seed
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

	uint32_t Hash(const uint8_t* key, size_t len, uint32_t seed)	// MurmurHash3
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

	uint32_t nanosecond() { return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count(); }
	uint32_t microsecond() { return duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count(); }
};

class Visualizer : public olc::PixelGameEngine
{
public:
	Random random;
	uint32_t seed;
	
	Visualizer() { sAppName = "Probability Visualizer"; }

public:
	bool OnUserCreate() override
	{
		random.MakeSeed(&seed, 0);
		//random.Seed(&seed);

		cout << "seed: ";
		for (int i = 0; i < 32; i++)
		{
			cout << (seed & 1);
			seed >>= 1;
		}
		cout << "\n";

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				uint32_t arr[2] = { x, y };
				uint32_t hash = random.Hash((uint8_t*)arr, sizeof(arr), 0);
				Draw(x, y, olc::Pixel(hash & 0xFF, (hash >> 8) & 0xFF, (hash >> 16) & 0xFF));
			}
		
		return true;
	}
};

int main()
{
	Visualizer visualizer;
	if (visualizer.Construct(512, 256, 3, 3))
		visualizer.Start();
	return 0;
}