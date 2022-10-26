#include "evaluation.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

ResultsMatrix::ResultsMatrix(int _numPlayers, int _numResultStates)
	: m_numPlayers(_numPlayers),
	m_numResultStates(_numResultStates),
	m_elements(_numPlayers * _numPlayers * _numResultStates, 0)
{}

ResultsMatrix::FlatIndex ResultsMatrix::GetPairingIdx(Index _player0, Index _player1) const
{
	return static_cast<FlatIndex>(_player0 * m_numPlayers + _player1);
}

int* ResultsMatrix::Get(FlatIndex _pairingIdx)
{
	return &m_elements[_pairingIdx * 3];
}

const int* ResultsMatrix::Get(FlatIndex _pairingIdx) const
{
	return &m_elements[_pairingIdx * 3];
}

void ResultsMatrix::Print(bool _symmetric) const
{
	std::cout << "   | ";
	for (Index j = 0; j < m_numPlayers; ++j)
		std::cout << fmt::format("{:2}           | ", j);
	std::cout << "\n";
	for (Index i = 0; i < m_numPlayers; ++i)
	{
		std::cout << fmt::format("{:2} | ", i);
		if (_symmetric)
		{
			for (Index j = 0; j < i; ++j)
			{
				const int* ptr0 = Get(GetPairingIdx(i, j));
				const int* ptr1 = Get(GetPairingIdx(j, i));

				std::cout << fmt::format("{:4}", ptr0[0] + ptr1[1]);
				std::cout << fmt::format("{:4}", ptr0[1] + ptr1[0]);
				for (Index r = 2; r < m_numResultStates; ++r)
					std::cout << fmt::format("{:4}", ptr0[r] + ptr1[r]);
				std::cout << " | ";
			}
			const int* ptr = Get(GetPairingIdx(i, i));
			for (Index r = 0; r < m_numResultStates; ++r)
				std::cout << fmt::format("{:4}", ptr[r]);
			std::cout << " | ";
		}
		else
		{
			for (Index j = 0; j < m_numPlayers; ++j)
			{
				const int* ptr = Get(GetPairingIdx(i, j));
				for (Index r = 0; r < m_numResultStates; ++r)
					std::cout << fmt::format("{:4}", ptr[r]);
				std::cout << " | ";
			}
		}
		std::cout << "\n";
	}
}

std::unique_ptr<ResultsMatrix> g_resultsMatrix;