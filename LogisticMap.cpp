#include <iostream>
#include <fstream>
#include <thread>
using namespace std;

const float logisticmap(float x, const float& r, const int& I)
{
	for (int i = 0; i < I; i++)
	{
		x = r * x * (1 - x);
	}
	return x;
}

void lm_thread(const int& START, const int& END, const int& H, const int& W, char** bifurcation, const int& I, const float& P)
{
	for (int x = START; x < END; x++)
	{
		const float x_f = x * P;
		for (int r = 0; r < W; r++)
		{
			const float r_f = r * P;
			const int x_r = logisticmap(x_f, r_f, I) * (1.0 / P);
			bifurcation[H - 1 - x_r][r] = 49;
		}
	}
}

int main()
{
	const int H = 10000, W = 40000, I = 30;
	const float P = 0.0001;

	cout << "Allocating Bifurcation Array\n";

	char** bifurcation = new char*[H];

	for (int i = 0; i < H; i++)
	{
		bifurcation[i] = new char[W];
		for (int k = 0; k < W; k++)
		{
			bifurcation[i][k] = 48;
		}
	}

	cout << "Mapping Logistic Map onto Bifurcation Array\n";
	
	const int threadsize = thread::hardware_concurrency() / 2;
	thread* threadarr = new thread[threadsize];

	for (int i = 0; i < threadsize; i++)
	{
		const int start = i == 0 ? 1 : (H * i) / threadsize;
		const int end = i == threadsize - 1 ? H : (H * (i + 1)) / threadsize;
		threadarr[i] = thread(lm_thread, start, end, H, W, bifurcation, I, P);
	}

	for (int i = 0; i < threadsize; i++)
	{
		threadarr[i].join();
	}

	delete[] threadarr;
	threadarr = nullptr;

	cout << "Forming PBM from Bifurcation Array\n";
	
	ofstream ostr("bifurcation.pbm");
	
	ostr << "P1\n";
	ostr << W << " " << H << "\n";

	for (int i = 0; i < H; i++)
	{
		ostr.write(bifurcation[i], W);
	}

	ostr.close();

	cout << "Deallocating Bifurcation Array\n";

	for (int i = 0; i < H; i++)
	{
		delete[] bifurcation[i];
		bifurcation[i] = nullptr;
	}

	delete[] bifurcation;
	bifurcation = nullptr;

	char exit;
	cout << "\nDone! Press any key then Enter to exit.\n";
	cin >> exit;

	return 0;
}
