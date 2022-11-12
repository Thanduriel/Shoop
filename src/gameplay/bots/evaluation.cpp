#include "evaluation.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>

ResultsMatrix::ResultsMatrix(int _numPlayers, int _numResultStates)
	: m_numPlayers(_numPlayers),
	m_numResultStates(_numResultStates),
	m_elements(_numPlayers * _numPlayers * _numResultStates, 0)
{}

ResultsMatrix::FlatIndex ResultsMatrix::GetPairingIdx(Index _player0, Index _player1) const
{
	return static_cast<FlatIndex>(_player0 * m_numPlayers + _player1);
}

int ResultsMatrix::Get(Index _player0, Index _player1, Index _result) const
{
	return m_elements[GetPairingIdx(_player0, _player1) * m_numResultStates + _result];
}

int* ResultsMatrix::Get(FlatIndex _pairingIdx)
{
	return &m_elements[_pairingIdx * m_numResultStates];
}

const int* ResultsMatrix::Get(FlatIndex _pairingIdx) const
{
	return &m_elements[_pairingIdx * m_numResultStates];
}

void ResultsMatrix::Print(bool _symmetric) const
{
	int maxDigits = 0;
	for (size_t i = 0; i < m_elements.size(); ++i)
		maxDigits = std::max(static_cast<int>(1+std::log10(m_elements[i])), maxDigits);
	// +1 to always have a gap
	++maxDigits;

	std::stringstream bufStream;
	bufStream << "   | ";
	for (Index j = 0; j < m_numPlayers; ++j)
		bufStream << fmt::format("{:^{}} | ", j, maxDigits * m_numResultStates);
	bufStream << "\n";
	for (Index i = 0; i < m_numPlayers; ++i)
	{
		bufStream << fmt::format("{:2} | ", i);
		if (_symmetric)
		{
			for (Index j = 0; j < i; ++j)
			{
				const int* ptr0 = Get(GetPairingIdx(i, j));
				const int* ptr1 = Get(GetPairingIdx(j, i));

				bufStream << fmt::format("{:{}}", ptr0[0] + ptr1[1], maxDigits);
				bufStream << fmt::format("{:{}}", ptr0[1] + ptr1[0], maxDigits);
				for (Index r = 2; r < m_numResultStates; ++r)
					bufStream << fmt::format("{:{}}", ptr0[r] + ptr1[r], maxDigits);
				bufStream << " | ";
			}
			const int* ptr = Get(GetPairingIdx(i, i));
			for (Index r = 0; r < m_numResultStates; ++r)
				bufStream << fmt::format("{:{}}", ptr[r], maxDigits);
			bufStream << " | ";
		}
		else
		{
			for (Index j = 0; j < m_numPlayers; ++j)
			{
				const int* ptr = Get(GetPairingIdx(i, j));
				for (Index r = 0; r < m_numResultStates; ++r)
					bufStream << fmt::format("{:{}}", ptr[r], maxDigits);
				bufStream << " | ";
			}
		}
		bufStream << "\n";
	}
	std::cout << bufStream.str();
}

std::unique_ptr<ResultsMatrix> g_resultsMatrix;