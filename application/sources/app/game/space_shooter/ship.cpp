#include "ship.h"
#include "asteroid.h"

Ship myShip;

/**
 * @brief Initialize the ship with default values.
 *
 * @param None
 * @return None
 */
void ship_init()
{
    APP_DBG_SIG("Ship init\n");
    myShip.x = SHIP_X_COORDINATE;
    myShip.y = SHIP_Y_COORDINATE;
    myShip.visible = WHITE;
    myShip.action_image = 1;
    myShip.score = 0;
}

/**
 * @brief Display the ship animation.
 *
 * @param None
 * @return None
 */
void ship_flight()
{
    // Cycle through the ship animation
    myShip.action_image++;
    if (myShip.action_image == 4)
    {
        myShip.action_image = 1;
    }
}

/**
 * @brief Fire a missile.
 *
 * @param None
 * @return None
 */
void ship_fire()
{
    // Send message to fire a missile
    APP_DBG_SIG("Ship fire missile\n");
    task_post_pure_msg(MISSILE_TASK_ID, MISSILE_FIRE_SIG);
}

/**
 * @brief Move the ship up by SHIP_Y_STEP.
 *
 * @param None
 * @return None
 */
void ship_move_up()
{
    APP_DBG_SIG("Ship move up\n");
    if (myShip.y > 0)
    {
        myShip.y -= SHIP_Y_STEP;
    }
}

/**
 * @brief Move the ship down by SHIP_Y_STEP.
 *
 * @param None
 * @return None
 */
void ship_move_down()
{
    APP_DBG_SIG("Ship move down\n");
    if (myShip.y < LCD_HEIGHT - SHIP_Y_STEP)
    {
        myShip.y += SHIP_Y_STEP;
    }
}

/**
 * @brief Reset the ship.
 *
 * @param None
 * @return None
 */
void ship_reset()
{
    APP_DBG_SIG("Ship reset\n");
    myShip.x = SHIP_X_COORDINATE;
    myShip.y = SHIP_Y_COORDINATE;
    myShip.visible = BLACK;
    myShip.action_image = 1;
    myShip.score = 0;
    ;
}

/**
 * @brief Handle the ship task.
 *
 * @param None
 * @return None
 */
void ship_handler(ak_msg_t *msg)
{
    switch (msg->sig)
    {
    case SHIP_INIT_SIG:
        ship_init();
        break;
    case SHIP_FLIGHT_SIG:
        ship_flight();
        break;
    case SHIP_FIRE_SIG:
        ship_fire();
        break;
    case SHIP_MOVE_UP_SIG:
        ship_move_up();
        break;
    case SHIP_MOVE_DOWN_SIG:
        ship_move_down();
        break;
    case MISSILE_DESTROY_SIG:
        myShip.score += 10;
        APP_DBG_SIG("Ship score %d\n", myShip.score);
        break;
    case SHIP_RESET_SIG:
        ship_reset();
        break;
    default:
        break;
    }
}
