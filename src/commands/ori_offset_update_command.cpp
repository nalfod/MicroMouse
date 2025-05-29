#include "ori_offset_update_command.h"

#include "utils/logging.h"

MM::OriOffsetUpdater::OriOffsetUpdater(bool& offsetFlag):
mOffsetFlag(offsetFlag)
{ 

}

void MM::OriOffsetUpdater::execute()
{
    if( !mStarted )
    {
        mStarted = true;
        mOffsetFlag = true;
    }
    else
    {
        if( !mOffsetFlag )
        {
            mFinished = true;
        }
    }
}

bool MM::OriOffsetUpdater::isFinished() const
{
    return mFinished;
}

MM::CommandResult MM::OriOffsetUpdater::getResult()
{
    return CommandResult();
}

void MM::OriOffsetUpdater::print() const
{
    LOG_INFO("OriOffsetUpdater: mStarted= %d mFinished: %d mOffsetFlag: %d \n",
        static_cast<int> ( mStarted ),
        static_cast<int> ( mFinished ),
        static_cast<int> ( mOffsetFlag )
    );
}