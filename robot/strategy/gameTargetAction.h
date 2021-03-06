#ifndef GAME_TARGET_ACTION_H
#define GAME_TARGET_ACTION_H

#include "gameTargetActionItemList.h"

#include "../../common/commons.h"

#include "../../common/2d/2d.h"
#include "../../common/io/outputStream.h"

#include "../../navigation/location.h"

/**
 * Encapsulates the target action.
 */
typedef struct GameTargetAction {
    // Returns the first point which is needed to do the target Action
    Location* startLocation;
    // Returns the location of the target Action.
    Location* endLocation;
    // Time to achieve (in second)
    float timeToAchieve;
    // Reference to GameTargetActionListItem, can be NULL
    GameTargetActionItemList* actionItemList;
    /** Path (can be NULL) used to check if the target Action Item is available. */
    PathData* pathData;
} GameTargetAction;

/**
 * Initializes the structure.
 */
void initGameTargetAction(GameTargetAction* targetAction, 
                          Location* startLocation,
                          Location* endLocation,
                          float timeToAchieve,
                          GameTargetActionItemList* actionItemList,
                          PathData* pathData);

/**
* Clear the targetAction and the items if any.
*/
void clearTargetAction(GameTargetAction* targetAction);

/**
 * Used for test but not in real situation, just to debug if action could be done easily
 * @return true if all items were done successfully, false if there at least one problem
 */
bool doGameTargetAction(GameTargetAction* targetAction, int* context);

#endif
