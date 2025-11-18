#pragma once

extern bool _enable_verbose;
extern bool is_no_entry;
void verbose(const char* format, ...);

class ProgressBar {
public:
	int barWidth = 50;
	int current = 0;
	int total = 0;

	ProgressBar(int total);
	void Add(int val = 1);
	void Print();
	void AddAndPrint(int val = 1);
private:
	float progress = 0.0f;
	void Update();
};