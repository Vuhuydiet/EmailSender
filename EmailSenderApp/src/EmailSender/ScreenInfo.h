#pragma once

#include "Line.h"

#include <vector>

class ScreenInfo 
{
public:
	
	void PushBack(const Line& info) { m_Lines.push_back(info); }
	template <typename ... Args>
	void EmplaceBack(Args&&... args) { m_Lines.emplace_back(std::forward<Args>(args)...); }
	void Clear() { m_Lines.clear(); }

	std::vector<Line>::iterator begin() { return m_Lines.begin(); }
	std::vector<Line>::iterator end() { return m_Lines.end(); }
	std::vector<Line>::const_iterator rbegin() const { return m_Lines.cbegin(); }
	std::vector<Line>::const_iterator rend() const { return m_Lines.cend(); }

private:
	std::vector<Line> m_Lines;
};