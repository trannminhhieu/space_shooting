#include "missile.h"
#include "ship.h"
#include "asteroid.h"

Missile myMissile;

/**
 * @brief Initialize the missile with default values.
 *
 * @param None
 * @return None
 */
void missile_inint()
{
    APP_DBG_SIG("Missile init\n");
    myMissile.x = 0;
    myMissile.y = 0;
    myMissile.visible = BLACK;
    myMissile.action_image = 1;
}

/**
 * Checks if the missile is armed.
 *
 * @param None
 * @return True if the missile is armed, False otherwise.
 */
bool is_armed()
{
    // If the missile is visible it is not armed
    if (myMissile.visible == BLACK)
    {
        return true; // It is armed
    }
    APP_DBG_SIG("Missile is armed\n");
    return false; // Otherwise, it is armed
}

/**
 * @brief Fire the missile if it is armed.
 *
 * This funciton set the y-coordinate of the missile and make it visible.
 *
 * @param None
 * @return None
 */
void missile_fired()
{
    const uint8_t SHIP_Y_OFFSET = 5;
    // Check if the missile is armed
    if (!is_armed())
    {
        return;
    }
    APP_DBG_SIG("Missile fired\n");
    // Set the y-coordinate of the missile
    myMissile.y = myShip.y + SHIP_Y_OFFSET;
    myMissile.visible = WHITE;
}

/**
 * @brief Move the missile if it is visible.
 *
 * This funciton moves the missile if it is visible.
 *
 * @param None
 * @return None
 */
void missile_flight()
{
    if (myMissile.visible == WHITE)
    {
        APP_DBG_SIG("Missile in flight\n");
        if (myMissile.x < MAX_MISSILE_DISTANCE)
        {
            myMissile.x += MISSILE_SPEED; // Move the missile by "MISSILE_SPEED" pixels
        }
        // If the missile fly to "MAX_MISSILE_DISTANCE", reset it
        else if (myMissile.x >= MAX_MISSILE_DISTANCE)
        {
            myMissile.visible = BLACK;
            is_armed(); // Display missile armed message
            myMissile.x = 0;
        }
    }
}

/**
 * @brief Reset the missile.
 *
 * @param None
 * @return None
 */
void missile_reset()
{
    APP_DBG_SIG("Missile reset\n");
    myMissile.visible = BLACK;
    myMissile.x = 0;
    myMissile.y = 0;
}

/**
 * @brief Handle the message of the missile.
 *
 * @param msg The message to be handled.
 * @return None
 */
void missile_handler(ak_msg_t *msg)
{
    switch (msg->sig)
    {
    case MISSILE_INIT_SIG:
        missile_inint();
        break;
    case MISSILE_FIRE_SIG:
        missile_fired();
        break;
    case MISSILE_FLIGHT:
        missile_flight();
        break;
    case MISSILE_RESET_SIG:
        missile_reset();
        break;
    default:
        break;
    }
}