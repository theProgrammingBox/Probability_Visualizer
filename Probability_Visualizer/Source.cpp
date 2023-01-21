#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>
#include <chrono>
#include <algorithm>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::cout;
using std::ifstream;
using std::max;

using olc::Pixel;

class Visualizer : public olc::PixelGameEngine
{
public:
	uint32_t ITERATIONS;
	uint32_t AGENTS;
	uint32_t ACTIONS;
	uint32_t SIZE;
	float* data;
	
	float y;
	float yv;
	
	Visualizer(uint32_t iterations, uint32_t agents, uint32_t actions, uint32_t size, float* data)
	{
		sAppName = "Probability Visualizer";
		ITERATIONS = iterations;
		AGENTS = agents;
		ACTIONS = actions;
		SIZE = size;
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
		Clear(Pixel(30, 0, 30));
		
		yv -= 0.4f * (GetMouseWheel() > 0);
		yv += 0.4f * (GetMouseWheel() < 0);
		yv *= 0.988f;
		y += yv;
		
		uint32_t snap = max(0, (int)y);
		int index = snap * ACTIONS * AGENTS;
		uint32_t drawX;
		
		for (uint32_t i = y + ScreenHeight() - snap; i--;)
		{
			if (index >= SIZE) break;
			
			drawX = 1;
			for (uint32_t j = 0; j < AGENTS; j++)
			{
				for (uint32_t k = ACTIONS; k--;)
					Draw(drawX++, i, Pixel(0, data[index++] * 255, 0));
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
	uint32_t SIZE;
	float* data;

	ifstream file("data.txt");
	file >> ITERATIONS >> AGENTS >> ACTIONS;
	SIZE = ACTIONS * AGENTS * ITERATIONS;

	data = new float[SIZE];
	for (int i = 0; i < SIZE; i++)
		file >> data[i];
	
	/*for (int i = 0; i < SIZE; i += 2)
		cout << data[i] << " " << data[i + 1] << "\n";*/

	file.close();
	
	uint32_t totalXElements = 1 + AGENTS + ACTIONS * AGENTS;
	uint32_t xPixels = 1920 / totalXElements;
	float yPixels = 2;
	uint32_t totalYElements = 1080 / yPixels;
	
	Visualizer visualizer(ITERATIONS, AGENTS, ACTIONS, SIZE, data);
	if (visualizer.Construct(totalXElements, totalYElements, xPixels, yPixels))
		visualizer.Start();
	return 0;
}