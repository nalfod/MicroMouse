#pragma once
#include <cstdint>
#include "cell_position.h"

namespace MM
{
class WallDetector
{
    public:
        WallDetector(uint16_t const& distLeft, uint16_t const& distRight, uint16_t const& distFrLeft, uint16_t const& distFrRight);
        int getWallMaskOfCurrentCell( CellPosition const& currentPositionR );
        int getWallMaskOfCurrentCellBayesian( CellPosition const& currentPositionR );
    private:
        uint16_t const& mDistLeft;
        uint16_t const& mDistRight;
        uint16_t const& mDistFrLeft;
        uint16_t const& mDistFrRight;

        // --- Bayesian wall probabilities (persist between updates for current cell) ---
        float mProbWallFront1{0.5f};
        float mProbWallFront2{0.5f};
        float mProbWallOnLeft{0.5f};
        float mProbWallOnRight{0.5f};

        // Bayesian helper methods
        void _resetWallProbabilities();
        float _bayesianWallUpdate(float prior, bool detected, float pDetectGivenWall, float pDetectGivenNoWall);
};

}