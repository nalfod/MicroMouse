#pragma once
#include <cstdint>
#include <memory>
#include <queue>
#include "motion_command_if.h"
#include "constants.h"
#include "maze/cell_position.h"

namespace MM
{
    class CommandExecuter
    {
        public:
            CommandExecuter( MM::CellPosition& cellPositionR, 
                             uint16_t const& dist_left, 
                             uint16_t const& dist_right, 
                             uint16_t const& dist_frontleft, 
                             uint16_t const& dist_frontright, 
                             int64_t const& encoderValue1, 
                             int64_t const& encoderValue2, 
                             float const& currentOriR, 
                             int16_t& leftMotorVoltage_mV, 
                             int16_t& rightMotorVoltage_mV,
                             bool& oriOffsetFlag);
            void execute();
            bool isFinished() const;
            // TODO: refactor this, very ugly!
            void addTravelCommandRelativeToActualPos(int directionToMove_deg, uint16_t numberOfCellsToMove);
            void printCurrentCommand() const;

            void addHalfCellTravelCommand();
            void addArcTravelCommand( float angleToTurn_deg );

            void parseRouteForSpeedRun(std::string route);

            void printActiveCommand() const;
        private:
            enum MovementPrimitives
            {
                FORWARD_MOVEMENT_BY_CELL_NUMBER = 1,
                FORWARD_MOVEMENT_FOR_ROT_ALIGNMENT = 2,
                ROTATING = 3,
                ROTATING_ON_GRID = 4, // need to be fixed!!!,
                ARC_MOVEMENT = 5,
                FORWARD_MOVEMENT_RAW = 6,
                BACKWARD_MOVEMENT_FOR_ALIGNMENT = 7,
                UPD_ORI_OFFSET_AND_CELL_POS_AT_BACKWALL = 8
            };
            // first pair value is the movement type, second one is the specific magnitude (eg.: distance or angle)
            using CommandToExecute = std::pair< MovementPrimitives, int >;

            void _actualizeCurrentCommand();
            bool _isFrontBlocked();
            std::unique_ptr<MotionCommandIF> _createCommandUsingCurrentPosition(CommandToExecute commandParams);
            float _getOffsetFromHomeInCellInCurrDir();
            
            bool _isAbleToStartWithSpeed( MovementPrimitives movementType );
            
            std::unique_ptr<MotionCommandIF> mCurrCommandToExecute;
            std::queue< CommandToExecute > mCommandsToExecute;
            
            MM::CellPosition& mCurrentCellPositionR;

            // measure points
            uint16_t const& mDistLeftR_mm;
            uint16_t const& mDistRightR_mm;
            uint16_t const& mDistFrontLeftR_mm;
            uint16_t const& mDistFrontRightR_mm;
            int64_t const& encoderValueLeftR_rev;
            int64_t const& encoderValueRightR_rev;
            float const& myCurrentOriR_deg;

            // controlled units
            int16_t& mLeftMotorVoltageR_mV;
            int16_t& mRightMotorVoltageR_mV;
            bool& mOriOffsetFlag;

            float mLastMovementEndSpeed_mm_per_s{0.0};
    };
}