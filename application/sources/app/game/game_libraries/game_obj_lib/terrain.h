#include <stdio.h>
#include <vector>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "perlin_noise.h"

class TerrainCoordinates
{
public:
    int x;
    int y;

    TerrainCoordinates();
    void terrainMover();
};
std ::vector<TerrainCoordinates> v_terrain;
