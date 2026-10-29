/*
 * Turoran Roguelike "Mobile Casting".
 *
 * A player who knows (or carries the aura of) the spell configured as
 * Roguelike.MobileCastingSpell in worldserver.conf may start and keep casting
 * cast-time spells and channels while moving. 0 (default) disables it.
 * The value is read once at first use; changing it needs a restart.
 */

#ifndef TURORAN_MOBILE_CASTING_H
#define TURORAN_MOBILE_CASTING_H

#include "Define.h"

class Unit;

namespace Turoran
{
    // Configured spell id (0 = feature off).
    uint32 MobileCastingSpellId();
    // True if movement must not interrupt this unit's casts/channels.
    bool CanCastWhileMoving(Unit const* unit);
}

#endif
