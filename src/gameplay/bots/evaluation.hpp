#pragma once

#include <vector>
#include <memory>

class ResultsMatrix {
public:
	using Index = int;
	using FlatIndex = size_t;

	ResultsMatrix(int _numPlayers, int _numResultStates);

	FlatIndex GetPairingIdx(Index _player0, Index _player1) const;
	//int& Get(Index _player0, Index _player1, Index _result);
	int* Get(FlatIndex _pairingIdx);
	const int* Get(FlatIndex _pairingIdx) const;

	void Print(bool _symmetric = false) const;
private:
	int m_numPlayers;
	int m_numResultStates;
	std::vector<int> m_elements;
};

extern std::unique_ptr<ResultsMatrix> g_resultsMatrix;