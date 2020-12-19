#pragma once

inline void ioInit()
{
}

inline void ioDelay(int x)
{
}

inline bool ioRead()
{
	char line[100];
	std::cin.getline(line, 100);

	return line[0] == 'y';
}
