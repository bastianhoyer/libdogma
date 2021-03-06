/* libdogma
 * Copyright (C) 2012, 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "test.h"

/* This is dirty, but we need the data. Don't do this in real
 * applications! */
#include "../src/core/tables.h"
#include "../src/core/tables.c"

#include <stdio.h>

dogma_context_t* ctx;
dogma_key_t module_slot, implant_slot;

static void try_all_char_attribs(void);
static void try_all_implant_attribs(void);
static void try_all_skill_attribs(dogma_typeid_t);
static void try_all_ship_attribs(void);
static void try_all_module_attribs(void);
static void try_all_charge_attribs(void);
static void try_all_drone_attribs(dogma_typeid_t);

int main(void) {
	int i;

	dogma_init();
	dogma_init_context(&ctx);

	try_all_char_attribs();
	try_all_skill_attribs(TYPE_Gunnery);

	/* To be perfectly thorough, some of these for loops should be
	 * nested in one another. Try it if you have spare time! */

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Implant) continue;

		dogma_add_implant(ctx, dogma_table_types[i].id, &implant_slot);
		try_all_char_attribs();
		try_all_implant_attribs();
		dogma_remove_implant(ctx, implant_slot);
	}

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Implant
		|| dogma_table_types[i].groupid != GROUP_Booster) continue;

		dogma_add_implant(ctx, dogma_table_types[i].id, &implant_slot);
		try_all_char_attribs();
		try_all_implant_attribs();

		/* Try all chance-based attributes (booster side effects) */
		const dogma_type_effect_t** te;
		const dogma_effect_t* e;
		dogma_array_t effects;
		dogma_key_t index = 0;

		dogma_get_type_effects(dogma_table_types[i].id, &effects);
		JLF(te, effects, index);
		while(te != NULL) {
			const dogma_location_t loc = {
				.type = DOGMA_LOC_Implant,
				.implant_index = implant_slot,
			};
			double out;

			dogma_get_effect((*te)->effectid, &e);
			if(e->fittingusagechanceattributeid == 0) continue;

			dogma_get_chance_based_effect_chance(ctx, loc, e->id, &out);
			dogma_toggle_chance_based_effect(ctx, loc, e->id, true);
			dogma_toggle_chance_based_effect(ctx, loc, e->id, false);

			JLN(te, effects, index);
		}

		dogma_remove_implant(ctx, implant_slot);
	}

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Drone) continue;

		dogma_add_drone(ctx, dogma_table_types[i].id, 1);
		try_all_drone_attribs(dogma_table_types[i].id);
		dogma_remove_drone(ctx, dogma_table_types[i].id);
	}

	dogma_add_drone(ctx, TYPE_WarriorII, 1);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Ship) continue;

		dogma_set_ship(ctx, dogma_table_types[i].id);
		try_all_char_attribs();
		try_all_ship_attribs();
	}

	dogma_set_ship(ctx, 587);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Module
		   && dogma_table_types[i].categoryid != CAT_Subsystem) continue;

		dogma_add_module(ctx, dogma_table_types[i].id, &module_slot);
		dogma_set_module_state(ctx, module_slot, DOGMA_STATE_Overloaded);
		try_all_char_attribs();
		try_all_ship_attribs();
		try_all_drone_attribs(TYPE_WarriorII);
		try_all_module_attribs();
		dogma_remove_module(ctx, module_slot);
	}

	dogma_add_module(ctx, 2873, &module_slot);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Charge) continue;

		dogma_add_charge(ctx, module_slot, dogma_table_types[i].id);
		try_all_char_attribs();
		try_all_ship_attribs();
		try_all_module_attribs();
		try_all_charge_attribs();
		dogma_remove_charge(ctx, module_slot);
	}

	dogma_free_context(ctx);
	return 0;
}

static void try_all_char_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_character_attribute(ctx, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_implant_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_implant_attribute(ctx, implant_slot, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_skill_attribs(dogma_typeid_t id) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_skill_attribute(ctx, id, dogma_table_attributes[i].id, &v);
	}
}


static void try_all_ship_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_ship_attribute(ctx, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_module_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_module_attribute(ctx, module_slot, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_charge_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_charge_attribute(ctx, module_slot, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_drone_attribs(dogma_typeid_t drone) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_drone_attribute(ctx, drone, dogma_table_attributes[i].id, &v);
	}
}
