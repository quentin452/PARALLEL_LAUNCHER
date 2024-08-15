#ifndef SRC_RHDC_UI_RHDC_UI_STATE_HPP_
#define SRC_RHDC_UI_RHDC_UI_STATE_HPP_

enum class RhdcSorting {
	Name = 0,
	Downloads = 1,
	Rating = 2,
	Difficulty = 3,
	LastPlayed = 4,
	PlayTime = 5,
	StarCount = 6,
	FixedShuffle = 7,
	RandomShuffle = 8
};

struct RhdcUiState {
	RhdcSorting sortBy;
	bool sortAsc;
	string playlist;
	string hackId;
};

#endif /* SRC_RHDC_UI_RHDC_UI_STATE_HPP_ */
