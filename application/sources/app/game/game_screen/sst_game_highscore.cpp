#include "sst_game_highscore.h"

game_score_t sst_game_score;

static void highscore_render();

view_dynamic_t dyn_view_sst_game_highscore = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    highscore_render};

view_screen_t sst_game_highscore = {
    &dyn_view_sst_game_highscore,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

void highscore_render()
{
    view_render.clear();

    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(5, 5);
    view_render.print("Highscore");

    view_render.setCursor(5, 25);
    view_render.print("1. ");
    view_render.setCursor(35, 25);
    view_render.print(sst_game_score.first_place);

    view_render.setCursor(5, 35);
    view_render.print("2. ");
    view_render.setCursor(35, 35);
    view_render.print(sst_game_score.second_place);

    view_render.setCursor(5, 45);
    view_render.print("3. ");
    view_render.setCursor(35, 45);
    view_render.print(sst_game_score.third_place);
}

void score_sort()
{
    if (sst_game_score.current_score > sst_game_score.first_place)
    {
        sst_game_score.third_place = sst_game_score.second_place;
        sst_game_score.second_place = sst_game_score.first_place;
        sst_game_score.first_place = sst_game_score.current_score;
    }
    else if (sst_game_score.current_score > sst_game_score.second_place)
    {
        sst_game_score.third_place = sst_game_score.second_place;
        sst_game_score.second_place = sst_game_score.current_score;
    }
    else if (sst_game_score.current_score > sst_game_score.third_place)
    {
        sst_game_score.third_place = sst_game_score.current_score;
    }
}

void game_highscore_handler(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_HIGHSCORE_ENTRY\n");
        view_render.initialize();
        view_render_display_on();

        score_sort();

        timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);
    }
    break;

    case AC_DISPLAY_BUTTON_UP_LONG_PRESSED:
    {
        APP_DBG_SIG("HIGH_SCORE_RESET\n");
        sst_game_score.first_place = 0;
        sst_game_score.second_place = 0;
        sst_game_score.third_place = 0;
    }
    break;

    case AC_DISPLAY_BUTTON_MODE_RELEASED:
    {
        APP_DBG_SIG("OUT_HIGH_SCORE\n");
        SCREEN_TRAN(game_menu_handler, &sst_game_menu);
    }
    break;

    case AC_DISPLAY_SHOW_IDLE:
    {
        SCREEN_TRAN(game_idle_handler, &sst_game_idle);
    }
    default:
        break;
    }
}
