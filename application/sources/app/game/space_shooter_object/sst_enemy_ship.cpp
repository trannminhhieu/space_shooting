#include "sst_ship.h"

EnemyShip myEnemyShip;

/**
 * @brief Initializes the enemy ship.
 *
 * @details This function sets the initial values for the enemy ship's position, visibility, action image,
 * health, and number of missiles.
 *
 * @param: None
 * @return: None
 */
void enemy_ship_init()
{
    APP_DBG_SIG("Enemy ship init\n");
    myEnemyShip.ship.visible = BLACK;
    myEnemyShip.ship.x = 0;
    myEnemyShip.ship.y = 0;
    myEnemyShip.ship.action_image = rand() % 3 + 1;
    myEnemyShip.health = SHIP_LIFE;
    myEnemyShip.num_missiles = MAX_NUM_OF_ENEMY_MISSILE;
    APP_DBG_SIG("Enemy ship health %d\n", myEnemyShip.health);
    APP_DBG_SIG("Enemy max missile %d\n", myEnemyShip.num_missiles);
}

uint8_t ship_action;
enum Actions
{
    MOVE_UP,
    MOVE_DOWN,
    FIRE,
    DO_NOTHING
};
/**
 * @brief Randomizes the action for the enemy ship.
 *
 * @details This function generates a random number between 0 and 99 and assigns a action to the enemy ship based on the generated number.
 * @param: None
 * @return: The action for the enemy ship. Possible values are MOVE_UP, MOVE_DOWN, FIRE, or DO_NOTHING.
 */
int8_t randomize_enemy_ship_control()
{
    // TODO: use better randomize function
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<int> dis(0, 99);
    // int random = dis(gen);
    uint8_t random = rand() % 100;

    // 5% chance to move up
    // 5% chance to move down
    // 7% chance to fire
    // TODO: increase these chances as the game progresses
    uint16_t rate = (myShip.fly_speed - 1) * 2;
    if (random < 5 + rate)
    {
        task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_MOVE_SIG);
        return MOVE_UP;
    }
    else if (random < 10 + rate)
    {
        task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_MOVE_SIG);
        return MOVE_DOWN;
    }
    else if (random < 17 + rate)
    {
        task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_FIRE_SIG);
        return FIRE;
    }

    return DO_NOTHING;
}

uint8_t strategy_based_enemy_decide_action()
{
    static uint8_t decision_interval = 0;
    int random_factor = rand() % 100; // Random number between 0 and 99

    if (decision_interval == 0)
    {
        if (myShip.ship.y < myEnemyShip.ship.y)
        {
            if (random_factor < 60) // 60% chance to move up if player is above
            {
                task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_MOVE_SIG);
                return MOVE_UP;
            }
            else if (random_factor < 80) // 20% chance to fire if player is above
            {
                task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_FIRE_SIG);
                return FIRE;
            }
        }
        else if (myShip.ship.y > myEnemyShip.ship.y)
        {
            if (random_factor < 60) // 60% chance to move down if player is below
            {
                task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_MOVE_SIG);
                return MOVE_DOWN;
            }
            else if (random_factor < 80) // 20% chance to fire if player is below
            {
                task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_FIRE_SIG);
                return FIRE;
            }
        }
        else
        {
            // Player is at the same y-coordinate, decide whether to fire or not with some randomness
            if (random_factor < 20) // 20% chance to fire when player is at the same y-coordinate
            {
                task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_FIRE_SIG);
                return FIRE;
            }
        }

        decision_interval = (rand() % 5 + 10) - (myShip.fly_speed - 1); // Randomize decision interval between 10 and 14 frames, reduce interval based on player speed
    }
    else
    {
        decision_interval--;
    }

    return DO_NOTHING;
}

uint8_t better_random_enemy_decide_action()
{
    static uint8_t decision_interval = 0;
    int8_t relative_player_enemy_position = myShip.ship.y - myEnemyShip.ship.y; // Pre-calculate relative player-enemy position

    if (decision_interval == 0)
    {
        // TODO: take in Player(x, y) and Enemy(x, y) as parameter to generate random number
        uint8_t random_factor = rand() % 100; // Random number between 0 and 99
        uint8_t base_probability = 20 + myShip.fly_speed;

        // Vary action probabilities based on game state or difficulty level
        uint8_t move_up_probability = base_probability;   // Base probability for moving up
        uint8_t move_down_probability = base_probability; // Base probability for moving down
        uint8_t fire_probability = base_probability - 10; // Base probability for firing

        // Adjust probabilities based on relative position
        if (relative_player_enemy_position < -10)
        {
            // Player is significantly above, bias towards moving up or firing
            move_up_probability += 30;
            move_down_probability -= 10;
            fire_probability += 10;
        }
        else if (relative_player_enemy_position > 10)
        {
            // Player is significantly below, bias towards moving down or firing
            move_up_probability -= 10;
            move_down_probability += 30;
            fire_probability += 10;
        }
        else
        {
            // Player is at a similar level, bias towards firing
            fire_probability += 20;
        }

        // Determine action based on probabilities
        if (random_factor < move_up_probability)
        {
            task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_MOVE_SIG);
            ship_action = MOVE_UP;
        }
        else if (random_factor < move_up_probability + move_down_probability)
        {
            task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_MOVE_SIG);
            ship_action = MOVE_DOWN;
        }
        else if (random_factor < move_up_probability + move_down_probability + fire_probability)
        {
            task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_FIRE_SIG);
            ship_action = FIRE;
        }
        else
        {
            ship_action = DO_NOTHING;
        }
        // Reset decision interval and update previous player position
        decision_interval = (rand() % 10 + 9) - (myShip.fly_speed - 1); // Random base interval between 9 and 18 frames, depending on player speed
        APP_DBG_SIG("Decision interval: %d\n", decision_interval);
    }
    else
    {
        decision_interval--;
        APP_DBG_SIG("Decision interval: %d\n", decision_interval);
    }

    return ship_action;
}

/**
 * @brief Initializes the enemy ship for takeoff.
 *
 * @details This function sets the initial values for the enemy ship's position, visibility, action image, and action.
 * @param: None
 * @return: None
 */
void enemy_ship_takeoff()
{
    APP_DBG_SIG("Enemy ship takeoff\n");

    // Set enemy ship position
    myEnemyShip.ship.x = 150;
    myEnemyShip.ship.y = 20;

    // Set enemy ship visibility and action image
    myEnemyShip.ship.visible = WHITE;
    myEnemyShip.ship.action_image = 1;
}

// TODO: add this function. @brief Retreats the enemy ship when "x" time have passed
void enemy_ship_retreat()
{
    APP_DBG_SIG("Enemy ship retreat\n");

    // Check if enemy ship is within the screen boundaries
    if (myEnemyShip.ship.x + SIZE_BITMAP_SHIP_X < LCD_WIDTH)
    {
        // Move enemy ship towards the right side of the screen
        myEnemyShip.ship.x++;
    }
    else
    {
        // Make enemy ship invisible
        myEnemyShip.ship.visible = BLACK;
    }
}

/**
 * @brief Adjusts the flight of the enemy ship.
 *
 * @details This function is responsible for adjusting the flight of the enemy ship based on its visibility and action image.
 * If the ship's x position is greater than 100, it moves the ship to the left by 2 units. If the action image is 1, it calls the
 * randomize_enemy_ship_control function to determine the ship's next action.
 *
 * @param None
 * @return None
 */
void enemy_ship_flight()
{
    // Check if the enemy ship is visible
    if (myEnemyShip.ship.visible != WHITE)
    {
        return; // If not visible, return and do nothing
    }

    // Adjust the ship's position based on conditions
    if (myEnemyShip.ship.x > 100)
    {
        // Fly enemy ship in range
        myEnemyShip.ship.x -= 2;
    }
    else
    {
        // Call the randomize_enemy_ship_control function to determine the ship's next action
        if (myShip.score > 800)
        {
            ship_action = strategy_based_enemy_decide_action();
        }
        else
        {
            ship_action = better_random_enemy_decide_action();
        }
    }

    // Increment the action image of the ship
    myEnemyShip.ship.action_image++;

    // Reset the action image if it reaches 4
    if (myEnemyShip.ship.action_image == 4)
    {
        myEnemyShip.ship.action_image = 1;
    }
}
void enemy_ship_health_control()
{
    // Check if the health of the enemy ship is less than or equal to 0
    if (myEnemyShip.health <= 0)
    {
        // Make the enemy ship invisible
        myEnemyShip.ship.visible = BLACK;

        // TODO: Send message with points value data for player ship
        // Increase the player's score by 100
        myShip.score += 100;

        // Reset the enemy ship and enemy missile tasks
        task_post_pure_msg(ENEMY_SHIP_TASK_ID, ENEMY_SHIP_RESET_SIG);
        task_post_pure_msg(ENEMY_MISSILE_TASK_ID, ENEMY_MISSILE_RESET_SIG);

        // Print debug message
        APP_DBG_SIG("Enemy ship dead\n");

        // Change the game stage to GAME_STAGE_ASTEROID_FEILD
        task_post_pure_msg(ASTEROID_TASK_ID, ASTEROID_SPAWN_SIG);
        game_stage = GAME_STAGE_ASTEROID_FEILD;

        // Return from the function
        return;
    }
}

/**
 * @brief Moves the enemy ship up or down based on its visibility and ship action.
 *
 * @details This function checks if the enemy ship is visible and its x position is less than 100.
 * If the ship is visible and the ship action is MOVE_UP, the ship is moved up by SHIP_Y_STEP units.
 * If the ship is visible and the ship action is MOVE_DOWN, the ship is moved down by SHIP_Y_STEP units.
 *
 * @param None
 * @return None
 */
void enemy_ship_move()
{
    // Check if the enemy ship is visible and its x position is less than 100
    if (myEnemyShip.ship.visible != WHITE || myEnemyShip.ship.x > 100)
    {
        return;
    }

    // Move the ship up if it is visible and the ship_action is MOVE_UP
    if (myEnemyShip.ship.y > 0 && ship_action == MOVE_UP)
    {
        // Print debug message
        APP_DBG_SIG("Enemy ship move up\n");

        // Update the ship's y position
        myEnemyShip.ship.y -= SHIP_Y_STEP;
    }

    // Move the ship down if it is visible and the ship_action is MOVE_DOWN
    if (myEnemyShip.ship.y < LCD_HEIGHT - SHIP_Y_STEP && ship_action == MOVE_DOWN)
    {
        // Print debug message
        APP_DBG_SIG("Enemy ship move down\n");

        // Update the ship's y position
        myEnemyShip.ship.y += SHIP_Y_STEP;
    }
}

/**
 * @brief Fires a missile from the enemy ship.
 *
 * @details This function checks if the ship action is set to FIRE and if the enemy ship has any missiles remaining.
 * If both conditions are met, a message is sent to fire a missile and the number of missiles is decremented by one.
 *
 * @param None
 * @return None
 */
void enemy_ship_fire()
{
    // Check if the ship action is set to FIRE and if the enemy ship has any missiles remaining
    if (myEnemyShip.num_missiles > 0 && ship_action == FIRE)
    {
        // Decrement the number of missiles by one
        myEnemyShip.num_missiles--;

        // Send a message to fire a missile
        APP_DBG_SIG("Enemy ship fire missile\n");
        task_post_pure_msg(ENEMY_MISSILE_TASK_ID, ENEMY_MISSILE_FIRE_SIG);
    }
}

/**
 * @brief Resets the enemy ship to its initial state.
 *
 * @details This function resets the enemy ship's position, visibility, action image, number of missiles, and health to their initial values.
 *
 * @param None
 * @return None
 */
void enemy_ship_reset()
{
    APP_DBG_SIG("Enemy ship reset\n");
    myEnemyShip.ship.visible = BLACK;
    myEnemyShip.ship.x = 0;
    myEnemyShip.ship.y = 0;
    myEnemyShip.ship.action_image = rand() % 3 + 1;
    myEnemyShip.health = SHIP_LIFE + (myShip.fly_speed);
    myEnemyShip.num_missiles = MAX_NUM_OF_ENEMY_MISSILE + (myShip.fly_speed);
    APP_DBG_SIG("Enemy ship health %d\n", myEnemyShip.health);
    APP_DBG_SIG("Enemy max missile %d\n", myEnemyShip.num_missiles);
}

void enemy_ship_handler(ak_msg_t *msg)
{
    switch (msg->sig)
    {
    case ENEMY_SHIP_INIT_SIG:
        enemy_ship_init();
        break;
    case ENEMY_SHIP_TAKEOFF_SIG:
        enemy_ship_takeoff();
        break;
    case ENEMY_SHIP_FLIGHT_SIG:
        enemy_ship_flight();
        enemy_ship_health_control();
        break;
    case ENEMY_SHIP_MOVE_SIG:
        enemy_ship_move();
        break;
    case ENEMY_SHIP_FIRE_SIG:
        enemy_ship_fire();
        break;
    case ENEMY_SHIP_RESET_SIG:
        enemy_ship_reset();
        break;
    default:
        break;
    }
}