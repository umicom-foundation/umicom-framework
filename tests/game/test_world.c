/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/game/test_world.c
 *
 * PURPOSE:
 *   Verify stable game entities and deterministic multiplayer acknowledgements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/game/game.h"

/* Exercise both world ownership and transport-neutral session confirmation. */
int main(void)
{
    UmiGameWorld *world = NULL;
    UmiGameSession *session = NULL;
    UmiGameEntityId hero_id = 0U;
    UmiGameEntity hero;
    UmiGameTransform transform = umi_game_transform_identity();
    UmiGameWorldSnapshot world_snapshot;
    UmiGameSessionSnapshot session_snapshot;
    uint64_t tick = 0U;

    /* A moved entity must still be found by the stable identity returned at spawn. */
    assert(umi_game_world_create(
        UMI_GAME_WORLD_3D, 4U, &world) == UMI_STATUS_OK);
    assert(umi_game_world_spawn(
        world, UMI_GAME_ENTITY_ACTOR, "Hero", &hero_id) == UMI_STATUS_OK);
    transform.position_x = 12.5;
    assert(umi_game_world_set_transform(
        world, hero_id, &transform) == UMI_STATUS_OK);
    assert(umi_game_world_set_tags(world, hero_id, UINT64_C(3)) ==
           UMI_STATUS_OK);
    assert(umi_game_world_find(world, hero_id, &hero) == UMI_STATUS_OK);
    assert(strcmp(hero.name, "Hero") == 0);
    assert(hero.transform.position_x == 12.5);
    assert(umi_game_world_snapshot(world, &world_snapshot) == UMI_STATUS_OK);
    assert(world_snapshot.entity_count == 1U);

    /* A tick becomes globally confirmed only after every participant accepts it. */
    assert(umi_game_session_create(
        UMI_GAME_SESSION_HOST, 2U, &session) == UMI_STATUS_OK);
    assert(umi_game_session_join(session, "player-one") == UMI_STATUS_OK);
    assert(umi_game_session_join(session, "player-two") == UMI_STATUS_OK);
    assert(umi_game_session_advance(session, &tick) == UMI_STATUS_OK);
    assert(tick == 1U);
    assert(umi_game_session_acknowledge(
        session, "player-one", tick) == UMI_STATUS_OK);
    assert(umi_game_session_acknowledge(
        session, "player-two", tick) == UMI_STATUS_OK);
    assert(umi_game_session_snapshot(session, &session_snapshot) ==
           UMI_STATUS_OK);
    assert(session_snapshot.confirmed_tick == 1U);

    /* Destruction order follows ownership and leaves no borrowed pointer alive. */
    umi_game_session_destroy(session);
    umi_game_world_destroy(world);
    return 0;
}
